import easyocr
from ultralytics import YOLO
from datetime import datetime
import re
import cv2
import numpy as np
from foodClasses import textClasses, pluMapping, openImageFoodItemList, openImagePackageItemList
import os

try:
    reader = easyocr.Reader(['en'])
except Exception as e:
    raise Exception(f"ERROR: OCR model loading failed: {str(e)}")
try:
    yolo = YOLO('yolov8x-oiv7.pt')
except Exception as e:
    raise Exception(f"ERROR: YOLO model loading failed: {str(e)}")

def cleanText(text):
    """ Normalize and clean extracted OCR text while keeping numbers. """
    return re.sub(r'[^a-zA-Z0-9\s]', '', text).strip().lower()

def isFoodClass(text):
    """ Check if extracted text belongs to a known classification. """
    words = cleanText(text).split()
    for word in words:
        if len(word) > 2 and word in textClasses:
            return {"type": "classification", "value": word}  # Return food category

    return None

def isPLUClass(text):
    words = cleanText(text).split()
    for word in words:
        if(len(word) == 4 and word.isdigit() and int(word) in pluMapping):
            return {"type": "classification", "value": pluMapping.get(int(word))}
        
    return None

def extractExpirationDate(textList):
    """Extract expiration dates from text and normalize them to YYYY/MM/DD format."""
    
    # List of valid month abbreviations and full names
    validMonths = {
        "jan": 1, "feb": 2, "mar": 3, "apr": 4, "may": 5, "jun": 6,
        "jul": 7, "aug": 8, "sep": 9, "oct": 10, "nov": 11, "dec": 12,
        "january": 1, "february": 2, "march": 3, "april": 4, "may": 5, "june": 6,
        "july": 7, "august": 8, "september": 9, "october": 10, "november": 11, "december": 12
    }

    # Common expiration date patterns
    datePatterns = [
        re.compile(r'\b(\d{1,2})[./-](\d{1,2})[./-](\d{2,4})\b'),  # MM/DD/YYYY or DD/MM/YYYY or YYYY/MM/DD
        re.compile(r'\b(\d{4})[./-](\d{1,2})[./-](\d{1,2})\b'),  # YYYY-MM-DD or YYYY/MM/DD
        re.compile(r'\b(\d{1,2}) ([A-Za-z]{3,}) (\d{2,4})\b', re.IGNORECASE),  # 12 Jan 2024 or 5 July 23
        re.compile(r'\b([A-Za-z]{3,}) (\d{1,2}),? (\d{2,4})\b', re.IGNORECASE),  # Jan 12, 2024 or July 5 23
        re.compile(r'\b(\d{1,2})[./-](\d{1,2})\b'),  # MM/DD or DD/MM (short format)
    ]

    detectedDates = []

    for text in textList:
        for pattern in datePatterns:
            match = pattern.search(text)
            if match:
                groups = match.groups()
                
                try:
                    if len(groups) == 3:
                        if groups[0].isalpha():  # Format: "Jan 12, 2024"
                            month = validMonths.get(groups[0].lower())
                            day, year = int(groups[1]), int(groups[2])
                        elif groups[1].isalpha():  # Format: "12 Jan 2024"
                            month = validMonths.get(groups[1].lower())
                            day, year = int(groups[0]), int(groups[2])
                        else:  # Format: "MM/DD/YYYY" or "DD/MM/YYYY"
                            first, second, third = map(int, groups)
                            if first > 31:  # Assume YYYY/MM/DD
                                year, month, day = first, second, third
                            elif third > 31:  # Assume MM/DD/YYYY
                                month, day, year = first, second, third
                            else:  # Assume DD/MM/YYYY
                                day, month, year = first, second, third
                    elif len(groups) == 2:  # Short format "MM/DD"
                        month, day = map(int, groups)
                        year = datetime.today().year  # Assume current year
                    
                    if year < 100:  # Convert two-digit years to four-digit
                        year += 2000 if year < 50 else 1900

                    normalizedDate = f"{year:04d}/{month:02d}/{day:02d}"
                    detectedDates.append(normalizedDate)
                except (ValueError, TypeError):
                    continue  # Skip invalid dates

    return detectedDates

def preprocessImage(img):
    print("Preprocessing image")
    
    if img is None:
        return("Image not found")

    # Convert to grayscale
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    # Apply Contrast Limited Adaptive Histogram Equalization (CLAHE)
    clahe = cv2.createCLAHE(clipLimit=3.0, tileGridSize=(8, 8))
    gray = clahe.apply(gray)

    # Use Morphological Gradient to highlight text regions
    kernel = np.ones((3, 3), np.uint8)
    grad = cv2.morphologyEx(gray, cv2.MORPH_GRADIENT, kernel)

    # Apply adaptive thresholding (handles variable lighting)
    binary = cv2.adaptiveThreshold(grad, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, 
                                   cv2.THRESH_BINARY, 11, 2)

    # Find text contours
    contours, _ = cv2.findContours(binary, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # Create a mask to extract text regions
    mask = np.zeros_like(gray)
    for cnt in contours:
        x, y, w, h = cv2.boundingRect(cnt)
        if w > 30 and h > 10:  # Ignore small non-text contours
            mask[y:y+h, x:x+w] = 255

    # Apply the mask
    processed = cv2.bitwise_and(gray, gray, mask=mask)

    print("Image Preprocessed")
    return processed  # Return improved image as a NumPy array
    
def performOCR(image):
    print("Performing OCR")
    result = {}
    foodLabels = []
    textResults = None

    processedImage = preprocessImage(image)
    if isinstance(processedImage, str):
        return {"Error": processedImage}


    try:
        print("Running YOLO to detect object...")
        #run yolo on unprocessed image
        modelResults = yolo(image, conf=.5) #yolo(image) returns a list of 'results', we should only have one because only a single image
        print("Detection complete. Filtering objects...")
        # Load the image using OpenCV
        processedImage = cv2.imread(image)

        # Get script directory and define output path
        scriptDir = os.path.dirname(os.path.abspath(__file__))
        outputDir = os.path.join(scriptDir, "img")
        outputPath = os.path.join(outputDir, "output.jpg")

        # Ensure the output directory exists (explicit check)
        try:
            if not os.path.exists(outputDir):
                os.makedirs(outputDir)
                print(f"✅ Created directory: {outputDir}")
            else:
                print(f"✅ Directory already exists: {outputDir}")
        except Exception as e:
            print(f"❌ Error creating directory {outputDir}: {e}")
            exit(1)  # Exit script if directory creation fails

        for modelResult in modelResults:
            for box in modelResult.boxes:

                classID = int(box.cls[0])
                className = yolo.names[classID]
                conf = box.conf[0].item()  # Confidence score

                # Extract bounding box coordinates
                bbox = box.xyxy[0].tolist()
                x1, y1, x2, y2 = map(int, bbox)

                # Ensure coordinates are within image bounds
                h, w, _ = processedImage.shape
                x1, y1, x2, y2 = max(0, x1), max(0, y1), min(w, x2), min(h, y2)

                # Draw bounding box
                cv2.rectangle(processedImage, (x1, y1), (x2, y2), (0, 255, 0), 3)

                # Prepare label text
                labelText = f"{className} {conf:.2f}"
                fontScale = 1
                fontThickness = 3
                textSize, _ = cv2.getTextSize(labelText, cv2.FONT_HERSHEY_SIMPLEX, fontScale, fontThickness)

                # Draw background rectangle for better readability
                cv2.rectangle(processedImage, (x1, y1 - textSize[1] - 10), (x1 + textSize[0] + 10, y1), (0, 255, 0), -1)
                cv2.putText(processedImage, labelText, (x1 + 5, y1 - 5), cv2.FONT_HERSHEY_SIMPLEX, fontScale, (0, 0, 0), fontThickness)

                if classID in openImageFoodItemList:
                    #if detected known food item (mostly produce)
                    print(f"Recognized {className}...")
                    foodLabels.append(className)
                    result["Food Labels"] = foodLabels

                elif classID in openImagePackageItemList:
                    #if detected known packaging item
                    print(f"Extracting text from {className}...")

                    # Crop detected text
                    croppedText = processedImage[y1:y2, x1:x2]

                    # Recognize text using EasyOCR
                    textResults = reader.readtext(croppedText, detail=0, paragraph=True, text_threshold=0.5)

                    print("Text extraction complete!")

        success = cv2.imwrite(outputPath, processedImage)
        if not success:
            print(f"❌ Failed to save output image to {outputPath}")
        else:
            print(f"✅ Output image saved to: {outputPath}")
        if textResults is None:
            print("No known objects detected. Scanning image for text.")
            textResults = reader.readtext(processedImage, detail=0, paragraph=True, text_threshold=0.5)
            # no package or bottle detected. maybe PLU?
            print("Scan complete.")
            for text in textResults:
                classification = isPLUClass(text)
                if classification:
                    print(f"Detected PLU: {classification["value"]}.")
                    foodLabels.append(classification["value"])
                    result["Food Labels"] = foodLabels
                    return result

        # Check for food class
        for text in textResults:
            classification = isFoodClass(text)
            if classification:
                print(f"Detected food item: {classification["value"]}.")
                foodLabels.append(classification["value"])

        # Extract expiration date
        expirationDate = extractExpirationDate(textResults)

        if foodLabels:
            result["Food Labels"] = foodLabels

        if expirationDate:
            result["Expiration Date"] = expirationDate

        return result  # Only includes keys if values exist

    except Exception as e:
        return {"Error": str(e)}

import easyocr
from ultralytics import YOLO
from datetime import datetime
import re
import sys
import cv2
import numpy as np
from foodClasses import textClasses, pluMapping, openImageFoodItemList, openImagePackageItemList
import matplotlib.pyplot as plt

try:
    reader = easyocr.Reader(['en'])
except Exception as e:
    raise(f"ERROR: OCR model loading failed: {str(e)}")
try:
    yolo = YOLO('yolov8m-oiv7.pt')
except Exception as e:
    raise(f"ERROR: OCR model loading failed: {str(e)}")

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

def extract_expiration_date(text_list):
    """Extract expiration dates from text and normalize them to YYYY/MM/DD format."""
    
    # List of valid month abbreviations and full names
    valid_months = {
        "jan": 1, "feb": 2, "mar": 3, "apr": 4, "may": 5, "jun": 6,
        "jul": 7, "aug": 8, "sep": 9, "oct": 10, "nov": 11, "dec": 12,
        "january": 1, "february": 2, "march": 3, "april": 4, "may": 5, "june": 6,
        "july": 7, "august": 8, "september": 9, "october": 10, "november": 11, "december": 12
    }

    # Common expiration date patterns
    date_patterns = [
        re.compile(r'\b(\d{1,2})[./-](\d{1,2})[./-](\d{2,4})\b'),  # MM/DD/YYYY or DD/MM/YYYY or YYYY/MM/DD
        re.compile(r'\b(\d{4})[./-](\d{1,2})[./-](\d{1,2})\b'),  # YYYY-MM-DD or YYYY/MM/DD
        re.compile(r'\b(\d{1,2}) ([A-Za-z]{3,}) (\d{2,4})\b', re.IGNORECASE),  # 12 Jan 2024 or 5 July 23
        re.compile(r'\b([A-Za-z]{3,}) (\d{1,2}),? (\d{2,4})\b', re.IGNORECASE),  # Jan 12, 2024 or July 5 23
        re.compile(r'\b(\d{1,2})[./-](\d{1,2})\b'),  # MM/DD or DD/MM (short format)
    ]

    detected_dates = []

    for text in text_list:
        for pattern in date_patterns:
            match = pattern.search(text)
            if match:
                groups = match.groups()
                
                try:
                    if len(groups) == 3:
                        if groups[0].isalpha():  # Format: "Jan 12, 2024"
                            month = valid_months.get(groups[0].lower())
                            day, year = int(groups[1]), int(groups[2])
                        elif groups[1].isalpha():  # Format: "12 Jan 2024"
                            month = valid_months.get(groups[1].lower())
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

                    normalized_date = f"{year:04d}/{month:02d}/{day:02d}"
                    detected_dates.append(normalized_date)
                except (ValueError, TypeError):
                    continue  # Skip invalid dates

    return detected_dates

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
    text_results = []

    processedImage = preprocessImage(image)
    if isinstance(processedImage, str):
        return(f"ERROR: {processedImage}")


    try:
        print("Running YOLO to detect object...")
        modelResult = yolo(image)[0] #yolo(image) returns a list of 'results', we should only have one because only a single image
        print(f"RESULT SIZE: {len(modelResult)}")
        print("Detection complete. Filtering objects...")

        for box in modelResult.boxes:
            classID = int(box.cls[0])
            className = yolo.names[classID]
            print(f"CLASS NAME : {className}\n\nCLASS ID : {classID}\n\n")
            if classID in openImageFoodItemList:
                print(f"Recognized {className}...")
                foodLabels.append(className)
                result["Food Labels"] = foodLabels
                return result
            
            elif classID in openImagePackageItemList:
                print(f"Reading {className}...")
                #crop if bottle or box is detected to read specific location
                bbox = box.xyxy[0].tolist()
                x1, y1, x2, y2 = map(int, bbox)

                # Crop detected text
                cropped_text = processedImage[y1:y2, x1:x2]

                # Recognize text using EasyOCR
                text_results = reader.readtext(cropped_text, detail=0, paragraph=True, text_threshold=0.7)

                print(f"Reading {className} complete!")


        if not text_results:
            print("No known objects detected. Scanning image for text.")
            text_results = reader.readtext(processedImage, detail=0, paragraph=True, text_threshold=0.7)
            # no package or bottle detected. maybe PLU?
            print("Scan complete.")
            for text in text_results:
                classification = isPLUClass(text)
                if classification:
                    foodLabels.append(classification["value"])
                    result["Food Labels"] = foodLabels
                    return result

        # Check for food class
        for text in text_results:
            classification = isFoodClass(text)
            if classification:
                foodLabels.append(classification["value"])

        # Extract expiration date
        expiration_date = extract_expiration_date(text_results)

        if foodLabels:
            result["Food Labels"] = foodLabels

        if expiration_date:
            result["Expiration Date"] = expiration_date

        return result  # Only includes keys if values exist

    except Exception as e:
        return {"Error": str(e)}

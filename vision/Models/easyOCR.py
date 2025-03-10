import easyocr
from ultralytics import YOLO
import re
import sys
import cv2
import numpy as np
from foodClasses import textClasses, pluMapping
import matplotlib as plt

try:
    reader = easyocr.Reader(['en'])
except Exception as e:
    raise(f"ERROR: OCR model loading failed: {str(e)}")
try:
    model = YOLO('yolov8s.pt')
except Exception as e:
    raise(f"ERROR: OCR model loading failed: {str(e)}")

def cleanText(text):
    """ Normalize and clean extracted OCR text while keeping numbers. """
    return re.sub(r'[^a-zA-Z0-9\s]', '', text).strip().lower()

def isFoodClass(text):
    """ Check if extracted text belongs to a known classification. """
    words = cleanText(text).split()
    for word in words:
        if(len(word) == 4 and word.isdigit() and int(word) in pluMapping):
            return {"type": "classification", "value": pluMapping.get(int(word))}
        if len(word) > 2 and word in textClasses:
            return {"type": "classification", "value": word}  # Return food category

    return None

def extract_expiration_date(text_list):
    """Extract expiration dates from text using various formats."""

    # Common expiration date patterns
    date_patterns = [
        r'(\b\d{1,2}[./-]\d{1,2}[./-]\d{2,4}\b)',  # MM/DD/YYYY or DD/MM/YYYY or YYYY/MM/DD
        r'(\b\d{4}[./-]\d{1,2}[./-]\d{1,2}\b)',  # YYYY-MM-DD or YYYY/MM/DD
        r'(\b\d{1,2} [A-Za-z]{3,} \d{2,4}\b)',  # 12 Jan 2024 or 5 July 23
        r'(\b[A-Za-z]{3,} \d{1,2},? \d{2,4}\b)',  # Jan 12, 2024 or July 5 23
        r'(\b\d{1,2}[./-]\d{1,2}\b)',  # MM/DD or DD/MM (common short format)
        r'(\bEXP:? \d{1,2}[./-]\d{1,2}[./-]\d{2,4}\b)',  # EXP 12/01/2024
        r'(\bBest Before:? \d{1,2}[./-]\d{1,2}[./-]\d{2,4}\b)',  # Best Before 01-2025
        r'(\bUse By:? \d{1,2}[./-]\d{1,2}[./-]\d{2,4}\b)',  # Use By 12-2023
        r'(\bSell By:? \d{1,2}[./-]\d{1,2}[./-]\d{2,4}\b)',  # Sell By 03/25
        r'(\bExp:? \d{1,2}[./-]\d{1,2}\b)',  # Exp 12/23
        r'(\bEXP:? \d{1,2}[./-]\d{1,2}\b)',  # EXP 12-23
    ]

    detected_dates = []

    # Check each text line for expiration date patterns
    for text in text_list:
        for pattern in date_patterns:
            match = re.search(pattern, text, re.IGNORECASE)
            if match:
                detected_dates.append(match.group(1))

    return detected_dates if detected_dates else "No expiration date detected"

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
    processed_image = preprocessImage(image)
    if isinstance(processed_image, str):
        return(f"ERROR: {processed_image}")

    try:
        class_names = model.names  # Dictionary mapping class indices to class names
        print("Detecting text...")
        boundingBoxes = model(image)[0]
        print("Text detected!")
        print("Reading text...")
        for box in boundingBoxes.boxes.data:  # Use .boxes.data instead of .xyxy
            x1, y1, x2, y2, conf, cls = box[:6]  # Extract bounding box coordinates and class
            x1, y1, x2, y2 = map(int, [x1, y1, x2, y2])  # Convert to integers
            class_name = class_names[int(cls)]  # Get class label
            # Draw rectangle
            cv2.rectangle(image, (x1, y1), (x2, y2), (0, 255, 0), 2)
             # Add label with class name and confidence score
            label = f"{class_name} ({conf:.2f})"
            cv2.putText(image, label, (x1, y1 - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

            # Crop detected text
            cropped_text = image[y1:y2, x1:x2]

            # Recognize text using EasyOCR
            text_results = reader.readtext(cropped_text, detail=0, paragraph=True, text_threshold=0.7)

            for _, text, confidence in text_results:
                print(f"Detected Text: {text} (Confidence: {confidence:.2f})")

        # Display image using Matplotlib
        plt.figure(figsize=(10, 6))
        plt.imshow(image)
        plt.axis('off')  # Hide axes
        plt.title("YOLO Text Detection")
        plt.show()
        plt.pause(10)
        plt.close
        print("Text read!")

        # Extract expiration date
        expiration_date = extract_expiration_date(text_results)

        print(f"\n\nRESULTS: {text_results}\n\n")
        # Extract PLU codes and classify food labels
        food_labels = []
        for text in text_results:
            classification = isFoodClass(text)
            if classification:
                food_labels.append(classification["value"])

        result = {}

        if food_labels:
            result["Food Labels"] = food_labels

        if expiration_date and expiration_date != "No expiration date detected":
            result["Expiration Date"] = expiration_date

        return result  # Only includes keys if values exist

    except Exception as e:
        return {"Error": str(e)}

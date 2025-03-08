import easyocr
import re
import sys
import cv2
import numpy as np
from foodClasses import textClasses, pluMapping

try:
    reader = easyocr.Reader(['en'], gpu=True)
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

def preprocessImage(img):
    print("Preprocessing image")
    
    if img is None:
        return("Image not found")
    
    # Ensure minimum width while maintaining aspect ratio
    min_width = 600
    height, width = img.shape[:2]
    
    if width < min_width:
        scale = min_width / width
        new_width = int(width * scale)
        new_height = int(height * scale)
        img = cv2.resize(img, (new_width, new_height), interpolation=cv2.INTER_CUBIC)

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
    
def performOCR(image_path):
    print("Performing OCR")
    processed_image = preprocessImage(image_path)
    if isinstance(processed_image, str):
        return(f"ERROR: {processed_image}")

    try:
        print("Reading text");
        results = reader.readtext(processed_image, detail=0, paragraph=True, text_threshold=0.8)
        print("Text read");

        # Filter only valid classifications
        filtered_results = [isFoodClass(text) for text in results if isFoodClass(text)]

        print("OCR performed")
        if filtered_results:
            for result in filtered_results:
                return(f"CLASSIFICATION: {result['value']}")
        else:
            return("INFO: No valid classifications found")
    except Exception as e:
        return(f"ERROR: {str(e)}")

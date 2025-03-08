import easyocr
import re
import sys
import cv2
import numpy as np
from foodClasses import textClasses, pluMapping

try:
    reader = easyocr.Reader(['en'], gpu=False)
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

def preprocessImage(image_path):
    print("Preprocessing image")
    img = cv2.imread(image_path)

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
    
def extractTextRegions(image_path):
    """ First pass: Extract bounding boxes of text regions without reading the text. """
    print("Extracting text regions")
    
    processed_image = preprocessImage(image_path)
    if isinstance(processed_image, str):  # If an error message is returned
        return processed_image, None  # Return as a tuple

    try:
        # Run OCR to get bounding boxes only
        results = reader.readtext(processed_image, detail=1, paragraph=True, text_threshold=0.3)

        if not results:
            return "ERROR: No text detected by EasyOCR", None
        
        bounding_boxes = []
        for result in results:
            if not isinstance(result, (tuple, list)) or len(result) < 2:
                print(f"Skipping invalid result: {result}")  # Debug print
                continue  # Skip invalid results

            bbox, text = result[:2]  # Extract bounding box and text safely
            prob = result[2] if len(result) > 2 else 1.0  # Default confidence if missing
            
            # Print detected text for debugging
            print(f"Detected Text: '{text}' (Confidence: {prob})")

            if prob < 0.3:
                continue  # Skip low-confidence detections

            # Extract bounding box coordinates
            (top_left, top_right, bottom_right, bottom_left) = bbox
            x_min = int(min(top_left[0], bottom_left[0]))
            x_max = int(max(top_right[0], bottom_right[0]))
            y_min = int(min(top_left[1], top_right[1]))
            y_max = int(max(bottom_left[1], bottom_right[1]))

            bounding_boxes.append((x_min, y_min, x_max, y_max))

        print(f"Extracted {len(bounding_boxes)} text regions.")

        if not bounding_boxes:
            return "ERROR: No text regions found", None

        return bounding_boxes, processed_image  # Return a tuple

    except Exception as e:
        return f"ERROR: {str(e)}", None  # Return error string as tuple




def performOCRWithZooming(image_path):
    """ Second pass: Run OCR on zoomed-in text regions. """
    print("Performing second pass OCR")

    bounding_boxes, processed_image = extractTextRegions(image_path)

    if isinstance(bounding_boxes, str):  # If it's an error message
        return bounding_boxes  # Return the error

    print(f"Extracted Bounding Boxes: {bounding_boxes}")

    refined_results = []

    for bbox in bounding_boxes:
        if len(bbox) != 4:
            print(f"Skipping invalid bounding box: {bbox}")
            continue

        x_min, y_min, x_max, y_max = bbox
        print(f"Processing bounding box: x_min={x_min}, y_min={y_min}, x_max={x_max}, y_max={y_max}")

        cropped_region = processed_image[y_min:y_max, x_min:x_max]

        if cropped_region.size == 0:
            print("Skipped: Empty cropped region")
            continue

        # Resize image for better OCR accuracy
        cropped_region = cv2.resize(cropped_region, None, fx=2, fy=2, interpolation=cv2.INTER_CUBIC)

        # Apply sharpening for better OCR
        sharpen_kernel = np.array([[-1, -1, -1], [-1, 9, -1], [-1, -1, -1]])
        cropped_region = cv2.filter2D(cropped_region, -1, sharpen_kernel)

        # Run OCR on the cropped image
        zoomed_texts = reader.readtext(cropped_region, detail=0, paragraph=True, text_threshold=0.7)

        print(f"Zoomed OCR Results: {zoomed_texts}")

        for zoomed_text in zoomed_texts:
            classification = isFoodClass(zoomed_text)
            if classification:
                refined_results.append(classification)

    print("OCR performed on refined text regions")

    if refined_results:
        return "\n".join([f"CLASSIFICATION: {result['value']}" for result in refined_results])
    else:
        return "INFO: No valid classifications found"



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

import easyocr
import re
import sys
import cv2
import numpy as np

try:
    reader = easyocr.Reader(['en'], gpu=False)
except Exception as e:
    raise(f"ERROR: OCR model loading failed: {str(e)}")

classification_keywords = {
    "milk", "cheese", "bread", "eggs", "yogurt", "butter", "juice", "meat", "chicken",
    "fish", "cereal", "pasta", "rice", "flour", "sugar", "salt", "pepper", "coffee",
    "tea", "honey", "jam", "peanut butter", "chocolate", "candy", "cookies", "crackers",
    "granola", "oats", "popcorn", "chips", "pancake mix", "syrup", "beans", "corn",
    "tomatoes", "tuna", "soup", "fruit", "vegetables", "meals", "pizza", "fries",
    "ice cream", "energy bars", "protein powder", "noodles", "ketchup", "mayonnaise",
    "mustard", "soy sauce", "hot sauce", "dressing", "oil", "vinegar", "pudding",
    "whipped cream", "sour cream", "tofu", "bacon"
}

def clean_text(text):
    """ Normalize and clean extracted OCR text. """
    return re.sub(r'[^a-zA-Z\s]', '', text).strip().lower()

def is_text_class(text):
    """ Check if extracted text belongs to a known classification. """
    words = clean_text(text).split()
    
    for word in words:
        if len(word) > 2 and word in classification_keywords:
            return {"type": "classification", "value": word}  # Return food category

    return None

def preprocess_image(image_path):
    print(image_path)
    img = cv2.imread(image_path)

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

    return processed  # Return improved image as a NumPy array
    
def perform_ocr(image_path):
    processed_image = preprocess_image(image_path)
    if isinstance(processed_image, str):
        return(f"ERROR: {processed_image}")

    try:
        results = reader.readtext(processed_image, detail=0, paragraph=True, text_threshold=0.8)

        # Filter only valid classifications
        filtered_results = [is_text_class(text) for text in results if is_text_class(text)]

        if filtered_results:
            for result in filtered_results:
                return(f"CLASSIFICATION: {result['value']}")
        else:
            return("INFO: No valid classifications found")
    except Exception as e:
        return(f"ERROR: {str(e)}")

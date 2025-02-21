import easyocr
import json
import cv2
import numpy as np

def preprocess_image(image_path):
    """ Prepares the image for better OCR accuracy with text region detection. """
    img = cv2.imread(image_path)

    if img is None:
        return None

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
    try:
        reader = easyocr.Reader(['en'], gpu=True)  # Use GPU if available

        # Preprocess image before OCR
        processed_image = preprocess_image(image_path)

        if processed_image is None:
            return json.dumps({"error": "Image not found or unreadable"})

        # Perform OCR only on detected text regions
        results = reader.readtext(processed_image, 
                                  detail=0, 
                                  paragraph=True, 
                                  text_threshold=0.8)

        return json.dumps({"text": results})

    except Exception as e:
        return json.dumps({"error": str(e)})

if __name__ == "__main__":
    import sys
    if len(sys.argv) < 2:
        print(json.dumps({"error": "No image path provided"}))
        sys.exit(1)

    image_path = sys.argv[1]
    output = perform_ocr(image_path)
    print(output)

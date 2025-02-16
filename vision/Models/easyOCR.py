import easyocr
import sys
import json
import cv2
import os

def preprocess_image(image_path):
    """ Preprocesses the image to improve OCR accuracy. """
    img = cv2.imread(image_path)  # Load image

    if img is None:
        print(json.dumps({"error": "Image not found"}))
        sys.exit(1)

    # Convert to grayscale
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    # Increase contrast
    img = cv2.equalizeHist(img)

    # Resize to 832x832 (if text is too small)
    #img = cv2.resize(img, (832, 832), interpolation=cv2.INTER_CUBIC)

    # Save the processed image as a temporary file
    processed_path = "temp_processed.jpg"
    cv2.imwrite(processed_path, img)

    return processed_path


def perform_ocr(image_path):
    reader = easyocr.Reader(['en'])

    # Preprocess image before OCR
    processed_image_path = preprocess_image(image_path)

    # Perform OCR
    results = reader.readtext(processed_image_path, detail=0, paragraph=True, text_threshold=0.6)

    # Delete the temporary processed image
    os.remove(processed_image_path)

    return json.dumps(results)

if __name__ == "__main__":
    image_path = sys.argv[1]
    output = perform_ocr(image_path)
    sys.stdout.write(output)  # Use stdout.write to ensure no extra newlines

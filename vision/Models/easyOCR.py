import easyocr
import sys
import json

def perform_ocr(image_path):
    reader = easyocr.Reader(['en'])
    results = reader.readtext(image_path)
    return json.dumps([text[1] for _, text, _ in results])

if __name__ == "__main__":
    image_path = sys.argv[1]
    print(perform_ocr(image_path))

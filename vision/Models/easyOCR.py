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

plu_mapping = {
    4015: "Apples, Red Delicious",
    4017: "Apples, Granny Smith",
    4131: "Apples, Fuji",
    4135: "Apples, Gala",
    3283: "Apples, Honeycrisp",
    4128: "Apples, Pink Lady",
    4046: "Avocados, Hass",
    4225: "Avocados, Large Hass",
    4011: "Bananas",
    4236: "Bananas, Red",
    4065: "Bell Peppers, Green",
    4088: "Bell Peppers, Red",
    4680: "Bell Peppers, Yellow",
    4050: "Cantaloupe",
    4261: "Coconuts",
    4281: "Grapefruit, Pink",
    4294: "Grapefruit, White",
    4030: "Kiwi, Green",
    3279: "Kiwi, Golden",
    4053: "Lemons",
    4048: "Limes",
    4051: "Mangoes, Red",
    4312: "Mangoes, Ataulfo (Honey)",
    3621: "Mangoes, Green",
    4036: "Nectarines, Yellow",
    3035: "Nectarines, White",
    4012: "Oranges, Navel",
    3108: "Oranges, Valencia",
    4381: "Oranges, Blood",
    3110: "Oranges, Cara Cara",
    3112: "Papayas, Large",
    4394: "Papayas, Solo",
    4397: "Passion Fruit",
    4038: "Peaches, Yellow",
    4401: "Peaches, White",
    4416: "Pears, Anjou (Green)",
    4417: "Pears, Anjou (Red)",
    4408: "Pears, Asian",
    4409: "Pears, Bartlett",
    4413: "Pears, Bosc",
    4414: "Pears, Comice",
    4418: "Pears, Forelle",
    4422: "Pears, Seckel",
    4430: "Pineapple",
    4040: "Plums, Black",
    4440: "Plums, Green",
    4042: "Plums, Red",
    4445: "Plums, Pluot",
    4446: "Pomegranates",
    4072: "Potatoes, Russet",
    4727: "Potatoes, Gold (Yukon)",
    4073: "Potatoes, Red",
    4091: "Potatoes, Sweet",
    4083: "Potatoes, White",
    4256: "Starfruit (Carambola)",
    4799: "Tomatoes, Beefsteak",
    3423: "Tomatoes, Heirloom",
    4087: "Tomatoes, Roma",
    4664: "Tomatoes, Vine Ripe",
    4032: "Watermelon, Seedless",
    4031: "Watermelon, With Seeds",
    4101: "Apples, Braeburn",
    4106: "Apples, Cortland",
    3293: "Apples, Jazz",
    4145: "Apples, Jonathan",
    4019: "Apples, McIntosh",
    4020: "Apples, Golden Delicious",
    4223: "Avocados, Florida",
    4682: "Bell Peppers, Orange",
    4282: "Grapefruit, Red",
    4450: "Guava",
    4479: "Lychee",
    4959: "Persimmons, Fuyu",
    4424: "Persimmons, Hachiya",
    4431: "Pineapple, Gold",
    4665: "Tomatoes, Cluster",
    3424: "Tomatoes, Grape",
    4235: "Plantain",
    4327: "Cherimoya",
    4444: "Fig, Brown Turkey",
    3038: "Fig, Black Mission",
    4306: "Dragon Fruit, White Flesh",
    4314: "Dragon Fruit, Red Flesh",
    4279: "Sapodilla",
    4489: "Tamarillo",
    4455: "Quince",
    3093: "Prickly Pear, Red",
    3427: "Prickly Pear, Green",
    4302: "Jackfruit",
    4309: "Rambutan",
    3040: "Durian",
    3132: "Longan",
    3037: "Mulberry",
    3039: "Gooseberry",
    4478: "Mangosteen",
    4511: "Cactus Pear",
    4505: "Breadfruit",
    3051: "Chayote",
    3050: "Jicama",
    4558: "Daikon Radish",
    4629: "Kohlrabi",
    3092: "Taro Root",
    4650: "Yuca Root",
    4092: "Carrots, Loose",
    4070: "Celery",
    4061: "Lettuce, Iceberg",
    4060: "Broccoli",
    4067: "Zucchini",
    4785: "Squash, Butternut",
    4779: "Squash, Spaghetti"
}

def clean_text(text):
    """ Normalize and clean extracted OCR text while keeping numbers. """
    return re.sub(r'[^a-zA-Z0-9\s]', '', text).strip().lower()

def is_text_class(text):
    """ Check if extracted text belongs to a known classification. """
    words = clean_text(text).split()
    for word in words:
        if(len(word) == 4 and word.isdigit() and int(word) in plu_mapping):
            return {"type": "classification", "value": plu_mapping.get(int(word))}
        if len(word) > 2 and word in classification_keywords:
            return {"type": "classification", "value": word}  # Return food category

    return None

def preprocess_image(image_path):
    print("Preprocessing image")
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

    print("Image Preprocessed")
    return processed  # Return improved image as a NumPy array
    
def extract_text_regions(image_path):
    """ First pass: Extract bounding boxes of text regions without reading the text. """
    print("Extracting text regions")
    
    processed_image = preprocess_image(image_path)
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




def perform_ocr_with_zoom(image_path):
    """ Second pass: Run OCR on zoomed-in text regions. """
    print("Performing second pass OCR")

    bounding_boxes, processed_image = extract_text_regions(image_path)

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
            classification = is_text_class(zoomed_text)
            if classification:
                refined_results.append(classification)

    print("OCR performed on refined text regions")

    if refined_results:
        return "\n".join([f"CLASSIFICATION: {result['value']}" for result in refined_results])
    else:
        return "INFO: No valid classifications found"



def perform_ocr(image_path):
    print("Performing OCR")
    processed_image = preprocess_image(image_path)
    if isinstance(processed_image, str):
        return(f"ERROR: {processed_image}")

    try:
        print("Reading text");
        results = reader.readtext(processed_image, detail=0, paragraph=True, text_threshold=0.8)
        print("Text read");

        # Filter only valid classifications
        filtered_results = [is_text_class(text) for text in results if is_text_class(text)]

        print("OCR performed")
        if filtered_results:
            for result in filtered_results:
                return(f"CLASSIFICATION: {result['value']}")
        else:
            return("INFO: No valid classifications found")
    except Exception as e:
        return(f"ERROR: {str(e)}")

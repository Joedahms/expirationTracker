import os
import sys
import json
import numpy as np
import tensorflow as tf
from tensorflow.keras.applications import EfficientNetB0
from tensorflow.keras.applications.efficientnet import preprocess_input
from tensorflow.keras.preprocessing.image import load_img, img_to_array

# ✅ Suppress TensorFlow warnings
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
tf.get_logger().setLevel('ERROR')

# ✅ Force CPU if needed (for Raspberry Pi)
os.environ["CUDA_VISIBLE_DEVICES"] = "-1"

# ✅ Load EfficientNetB0 Pre-trained on ImageNet
try:
    model = EfficientNetB0(weights="imagenet")
except Exception as e:
    print(f"ERROR: Model loading failed: {str(e)}")
    sys.exit(1)

# ✅ Image Processing Parameters
IMG_SIZE = (224, 224)  # Must match EfficientNet's input size

# ✅ Valid food class indices
valid_food_classes = {
    924, 925, 926, 927, 928, 929, 930, 931, 932, 933, 934, 935, 936, 937, 938,
    939, 940, 941, 942, 943, 944, 945, 946, 947, 948, 949, 950, 951, 952, 953,
    954, 955, 956, 957, 959, 960, 961, 962, 963, 964, 965, 966, 967, 968, 969
}

# ✅ Food class label mapping
food_labels = {
    924: "guacamole", 925: "consomme", 926: "hot_pot", 927: "trifle",
    928: "ice_cream", 929: "ice_lolly", 930: "French_loaf", 931: "bagel",
    932: "pretzel", 933: "cheeseburger", 934: "hotdog", 935: "mashed_potato",
    936: "head_cabbage", 937: "broccoli", 938: "cauliflower", 939: "zucchini",
    940: "spaghetti_squash", 941: "acorn_squash", 942: "butternut_squash",
    943: "cucumber", 944: "artichoke", 945: "bell_pepper", 946: "cardoon",
    947: "mushroom", 948: "Granny_Smith", 949: "strawberry", 950: "orange",
    951: "lemon", 952: "fig", 953: "pineapple", 954: "banana", 955: "jackfruit",
    956: "custard_apple", 957: "pomegranate", 959: "carbonara", 960: "chocolate_sauce",
    961: "dough", 962: "meat_loaf", 963: "pizza", 964: "potpie", 965: "burrito",
    966: "red_wine", 967: "espresso", 968: "cup", 969: "eggnog"
}

def preprocess_image(image_path):
    """Loads and preprocesses an image for EfficientNetB0."""
    if not os.path.exists(image_path):
        return None  # Handle file not found scenario

    try:
        img = load_img(image_path, target_size=IMG_SIZE)  # Load & resize
        img_array = img_to_array(img)  # Convert to numpy array
        img_array = preprocess_input(img_array)  # Normalize using EfficientNet preprocessing
        img_array = np.expand_dims(img_array, axis=0)  # Add batch dimension
        return img_array
    except Exception as e:
        return str(e)  # Return error message

def classify_image(image_path):
    """Runs inference on an image and returns the class name if valid."""
    try:
        img_array = preprocess_image(image_path)
        
        if img_array is None:
            print("ERROR: Image not found")
            sys.exit(1)

        if isinstance(img_array, str):  # This means an error message was returned
            print(f"ERROR: {img_array}")
            sys.exit(1)

        preds = model.predict(img_array, verbose=0)  # Get predictions
        top_class_index = np.argmax(preds)  # Get index of highest probability class
        top_probability = preds[0, top_class_index]  # Get the probability of the top class

        PROBABILITY_THRESHOLD = 0.30  # Set minimum confidence level
        if top_class_index in valid_food_classes and top_probability >= PROBABILITY_THRESHOLD:
            print(F"CLASSIFICATION: {food_labels[top_class_index]}")
        else:
            print("INFO: Low confidence or not a valid food item")
            sys.exit(1)
    
    except Exception as e:
        print(f"ERROR: {str(e)}")
        sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("ERROR: No image path provided")
        sys.exit(1)

    image_path = sys.argv[1]  # Get image path from command-line argument
    classify_image(image_path)
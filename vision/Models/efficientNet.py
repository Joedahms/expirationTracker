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
    print(json.dumps({"error": f"Model loading failed: {str(e)}"}))
    sys.exit(1)

# ✅ Image Processing Parameters
IMG_SIZE = (224, 224)  # Must match EfficientNet's input size

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
        return str(e)  # Return error message for JSON output

def classify_image(image_path):
    """Runs inference on an image using ImageNet labels and returns the top predicted class index and probability."""
    try:
        img_array = preprocess_image(image_path)
        
        if img_array is None:
            return json.dumps({"error": "Image not found"})

        if isinstance(img_array, str):  # This means an error message was returned
            return json.dumps({"error": img_array})

        preds = model.predict(img_array, verbose=0)  # Get predictions
        top_class_index = np.argmax(preds)  # Get index of highest probability class
        top_probability = preds[0, top_class_index]  # Get the probability of the top class

        return json.dumps({"index": int(top_class_index), "probability": float(top_probability)})

    except Exception as e:
        return json.dumps({"error": str(e)})

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(json.dumps({"error": "No image path provided"}))
        sys.exit(1)

    image_path = sys.argv[1]  # Get image path from command-line argument
    output = classify_image(image_path)  # Ensure valid JSON output
    print(output)

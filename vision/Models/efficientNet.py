import os
import sys
import json
import numpy as np
import tensorflow as tf
from tensorflow.keras.applications import EfficientNetB0
from tensorflow.keras.applications.efficientnet import preprocess_input, decode_predictions
from tensorflow.keras.preprocessing.image import load_img, img_to_array

# ✅ Suppress TensorFlow warnings
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
tf.get_logger().setLevel('ERROR')

# ✅ Force CPU if needed (for Raspberry Pi)
os.environ["CUDA_VISIBLE_DEVICES"] = "-1"

# ✅ Load EfficientNetB0 Pre-trained on ImageNet
model = EfficientNetB0(weights="imagenet")  # No custom weights

# ✅ Image Processing Parameters
IMG_SIZE = (224, 224)  # Must match EfficientNet's input size

def preprocess_image(image_path):
    """Loads and preprocesses an image for EfficientNetB0."""
    if not os.path.exists(image_path):
        print(json.dumps({"error": "Image not found"}))  # JSON output for C++
        sys.exit(1)

    img = load_img(image_path, target_size=IMG_SIZE)  # Load & resize
    img_array = img_to_array(img)  # Convert to numpy array
    img_array = preprocess_input(img_array)  # Normalize using EfficientNet preprocessing
    img_array = np.expand_dims(img_array, axis=0)  # Add batch dimension
    return img_array

def classify_image(image_path):
    """Runs inference on an image using ImageNet labels and returns the top predicted class index and probability."""
    img_array = preprocess_image(image_path)
    
    preds = model.predict(img_array, verbose=0)  # Get predictions
    top_class_index = np.argmax(preds)  # Get index of highest probability class
    top_probability = preds[0, top_class_index]  # Get the probability of the top class
    
    return top_class_index, top_probability  # Return class index and probability

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(json.dumps({"error": "No image path provided"}))
        sys.exit(1)

    image_path = sys.argv[1]  # Get image path from command-line argument
    top_class_index, top_probability = classify_image(image_path)  # Get result instead of printing
    print(json.dumps({"index": int(top_class_index), "probability": float(top_probability)}))
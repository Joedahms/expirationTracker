import os
import zmq
import struct
import cv2
import numpy as np
from easyOCR import performOCR

PORT = "5555"
ADDRESS = f"tcp://0.0.0.0:{PORT}"  # Bind to all interfaces

def run_server():
    # Create ZeroMQ context and socket
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind(ADDRESS)
    
    print(f"Server started on {ADDRESS}")
    
    try:
        while True:
            print("Waiting for image from Raspberry Pi...")
            # Receive message (multipart: [size, image bytes])
            msg_parts = socket.recv_multipart()

            # Extract image size
            img_size = struct.unpack("Q", msg_parts[0])[0]  # First frame: uint64_t size

            # Extract image data
            img_data = msg_parts[1]  # Second frame: byte data

            print(f"Request received! {img_size} bytes.")
            # Convert byte data to OpenCV image
            image = cv2.imdecode(np.frombuffer(img_data, dtype=np.uint8), cv2.IMREAD_COLOR)
            result = performOCR(image)  # Calls OCR module
            
            # Send response back to client
            socket.send_string(result)
            print(f"Sent response: {result[:50]}...")  # Print first 50 chars
    
    except KeyboardInterrupt:
        print("Server interrupted")
    finally:
        # Clean up
        socket.close()
        context.term()
        print("Server terminated")

if __name__ == "__main__":
    run_server()

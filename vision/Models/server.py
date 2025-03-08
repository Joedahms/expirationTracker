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

            try:
                msg_parts = socket.recv_multipart(flags=zmq.NOBLOCK)  # Non-blocking receive

                img_size = struct.unpack("Q", msg_parts[0])[0]
                img_data = msg_parts[1]

                print(f"Request received! {img_size} bytes.")
                image = cv2.imdecode(np.frombuffer(img_data, dtype=np.uint8), cv2.IMREAD_COLOR)
                result = performOCR(image)

                socket.send_string(result)
                print(f"Sent response: {result[:50]}...")
                
            except zmq.Again:  # Timeout reached
                print("No incoming data, server still running...")
    
    except KeyboardInterrupt:
        print("Server interrupted")
    finally:
        # Clean up
        socket.close()
        context.term()
        print("Server terminated")

if __name__ == "__main__":
    run_server()

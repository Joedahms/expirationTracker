import os
import zmq
import struct
import cv2
import numpy as np
import socket
import json
import time
from easyOCR import performOCR

PORT = "5555" #zeroMQ port
HEARTBEAT_PORT = "5556"
DISCOVERY_PORT = 5005 # UDP discovery port

def get_local_ip():
    """Get the actual network IP of this machine (not 127.0.0.1)."""
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))  # Connect to an external server
        ip = s.getsockname()[0]
        s.close()
        return ip
    except Exception as e:
        print(f"Failed to get local IP: {e}")
        return "127.0.0.1"

def wait_for_pi_discovery():
    """Wait for a Raspberry Pi discovery request, then send the IP."""
    server_ip = get_local_ip()
    print(f"Waiting for Raspberry Pi discovery on UDP {DISCOVERY_PORT}...")

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("0.0.0.0", DISCOVERY_PORT))  # Listen on all interfaces

    while True:
        try:
            data, client_addr = sock.recvfrom(1024)  # Receive broadcast
            message = data.decode("utf-8").strip()

            if message == "DISCOVER_SERVER":
                print(f"Received discovery request from {client_addr[0]}")
                sock.sendto(server_ip.encode(), client_addr)  # Send server IP back
                sock.close()
                return
        except Exception as e:
            print(f"UDP error: {e}")

def run_server():
    wait_for_pi_discovery()  # Wait for Raspberry Pi discovery
    ADDRESS = f"tcp://0.0.0.0:{PORT}"  # Bind ZeroMQ to communicate with Pi
    HEARTBEAT_ADDRESS = f"tcp://0.0.0.0:{HEARTBEAT_PORT}"

    # Create ZeroMQ context and socket
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind(ADDRESS)

    hearbeatsocket = context.socket(zmq.REP)
    hearbeatsocket.bind(HEARTBEAT_ADDRESS)
    hearbeatsocket.RCVTIMEO = 0  # non-blocking receive
    
    print(f"Server started on {ADDRESS}")
    
    poller = zmq.Poller()
    poller.register(socket, zmq.POLLIN)

    last_heartbeat_time = time.time()

    try:
        while True:
            print("Waiting for image from Raspberry Pi...")

            socks = dict(poller.poll(1000))  # 1-second timeout

            if socket in socks and socks[socket] == zmq.POLLIN:
                # Receive image data (blocking receive)
                msg_parts = socket.recv_multipart()  
                try:
                    print(f"Request received!")
                    img_size = struct.unpack("Q", msg_parts[0])[0]
                    img_data = msg_parts[1]

                    print(f"Image is {img_size} bytes!")

                    print(f"Decoding image.")
                    # Decode image
                    image = cv2.imdecode(np.frombuffer(img_data, dtype=np.uint8), cv2.IMREAD_COLOR)
                    if image is None:
                        print("Error: Failed to decode image.")
                        socket.send_string("ERROR: Image decoding failed")
                        continue
                    
                    print(f"Image decoded. Now beginning AI processing.")

                    # Perform OCR
                    result = json.dumps(performOCR(image))

                    print(f"Processing complete. Sending result back.")
                    # Send response
                    socket.send_string(result)
                    print(f"Sent response: {result}")

                    # Reset heartbeat timer after processing completes
                    last_heartbeat_time = time.time()
                except Exception as e:
                    print(f"Processing error: {e}")
                    socket.send_string("ERROR: Exception during processing")
            else:
                try:
                    msg = hearbeatsocket.recv_string(flags=zmq.NOBLOCK)
                    if msg == "heartbeat":
                        print("Received heartbeat")
                        hearbeatsocket.send_string("alive")
                        last_heartbeat_time = time.time()
                except zmq.Again:
                    pass  # No heartbeat received

                if time.time() - last_heartbeat_time > 20:
                    print("No heartbeat received for 20 seconds while idle. Restarting server...")
                    break
    except KeyboardInterrupt:
        print("Server interrupted")
    finally:
        # Clean up
        socket.close()
        hearbeatsocket.close()
        context.term()
        print("Server terminated")

if __name__ == "__main__":
    while True:
        run_server()
        print("restarting server...")
        time.sleep(2)

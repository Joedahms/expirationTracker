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

def getLocalIP():
    """Get the actual network IP of this machine (not 127.0.0.1)."""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.connect(("8.8.8.8", 80))  # Connect to an external server
        ip = sock.getsockname()[0]
        sock.close()
        return ip
    except Exception as e:
        print(f"Failed to get local IP: {e}")
        return "127.0.0.1"

def waitForPiDiscovery():
    """Wait for a Raspberry Pi discovery request, then send the IP."""
    serverIP = getLocalIP()
    print(f"Waiting for Raspberry Pi discovery on UDP {DISCOVERY_PORT}...")

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("0.0.0.0", DISCOVERY_PORT))  # Listen on all interfaces

    while True:
        try:
            data, clientAddress = sock.recvfrom(1024)  # Receive broadcast
            message = data.decode("utf-8").strip()

            if message == "DISCOVER_SERVER":
                print(f"Received discovery request from {clientAddress[0]}")
                sock.sendto(serverIP.encode(), clientAddress)  # Send server IP back
                sock.close()
                return
        except Exception as e:
            print(f"UDP error: {e}")

def runserver():
    waitForPiDiscovery()  # Wait for Raspberry Pi discovery
    ADDRESS = f"tcp://0.0.0.0:{PORT}"  # Bind ZeroMQ to communicate with Pi
    HEARTBEAT_ADDRESS = f"tcp://0.0.0.0:{HEARTBEAT_PORT}"

    # Create ZeroMQ context and socket
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind(ADDRESS)

    heartbeatSocket = context.socket(zmq.REP)
    heartbeatSocket.bind(HEARTBEAT_ADDRESS)
    heartbeatSocket.RCVTIMEO = 0  # non-blocking receive
    
    print(f"Server started on {ADDRESS}")
    
    poller = zmq.Poller()
    poller.register(socket, zmq.POLLIN)

    lastHeartbeatTime = time.time()

    try:
        while True:
            print("Waiting for image from Raspberry Pi...")

            socks = dict(poller.poll(1000))  # 1-second timeout

            if socket in socks and socks[socket] == zmq.POLLIN:
                # Receive image data (blocking receive)
                messageParts = socket.recv_multipart()  
                try:
                    print(f"Request received!")
                    imageSize = struct.unpack("Q", messageParts[0])[0]
                    imageData = messageParts[1]

                    print(f"Image is {imageSize} bytes!")

                    print(f"Decoding image.")
                    # Decode image
                    image = cv2.imdecode(np.frombuffer(imageData, dtype=np.uint8), cv2.IMREAD_COLOR)
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
                    lastHeartbeatTime = time.time()
                except Exception as e:
                    print(f"Processing error: {e}")
                    socket.send_string("ERROR: Exception during processing")
            else:
                try:
                    heartbeatmessage = heartbeatSocket.recv_string(flags=zmq.NOBLOCK)
                    if heartbeatmessage == "heartbeat":
                        print("Received heartbeat")
                        heartbeatSocket.send_string("alive")
                        lastHeartbeatTime = time.time()
                except zmq.Again:
                    pass  # No heartbeat received

                if time.time() - lastHeartbeatTime > 6:
                    print("Pi disconnect detected. Restarting server...")
                    break
    except KeyboardInterrupt:
        print("Server interrupted")
    finally:
        # Clean up
        socket.close()
        heartbeatSocket.close()
        context.term()
        print("Server terminated")

if __name__ == "__main__":
    while True:
        runserver()
        print("restarting server...")
        time.sleep(2)

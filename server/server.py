import zmq
import struct
import cv2
import numpy as np
import socket
import time

from easyOCR import performOCR
from load_config import loadConfig
from wait_for_pi_discovery import waitForPiDiscovery
from handler import handleControlMessage, handleImage

DEFAULT_DISCOVERY_PORT = 5005
DEFAULT_PORT = 5555

def runServer():
    config = loadConfig()
    network = config.get("network", {})
    port = network.get("serverPort", DEFAULT_PORT)
    discoveryPort = network.get("discoveryPort", DEFAULT_DISCOVERY_PORT)

    waitForPiDiscovery(discoveryPort)

    ADDRESS = f"tcp://0.0.0.0:{port}"  # Bind ZeroMQ to communicate with Pi

    # Create ZeroMQ context and socket
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind(ADDRESS)

    print(f"Server started on {ADDRESS}")
    
    poller = zmq.Poller()
    poller.register(socket, zmq.POLLIN)

    try:
        startMessage = socket.recv_string() #wait for the pi to be connected
        if startMessage == "connected":
            print("Pi is connected")
            socket.send_string("awake")
        while True:
            print("Waiting for image from Raspberry Pi...")

            socks = dict(poller.poll(1000))  # 1-second timeout

            if socket in socks and socks[socket] == zmq.POLLIN:
                # Receive image data (blocking receive)
                messageParts = socket.recv_multipart()  
                try:
                    if len(messageParts) == 1:
                        handleControlMessage(socket, messageParts)
                    elif len(messageParts) == 2:
                        handleImage(socket, messageParts)

                except Exception as e:
                    print(f"Processing error: {e}")
                    socket.send_string("ERROR: Exception during processing")
    except KeyboardInterrupt:
        print("Server interrupted")
    finally:
        # Clean up
        socket.close()
        context.term()
        print("Server terminated")

if __name__ == "__main__":
    while True:
        runServer()
        print("restarting server...")
        time.sleep(2)

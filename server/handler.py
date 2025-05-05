def handleControlMessage(socket, messageParts):
    command = messageParts[0].decode()
    if command == "start":
        print("Processing started.")
        socket.send_string("yes")
    elif command == "stop":
        print("Processing stopped.")
        socket.send_string("yes")

def handleImage(socket, messageParts):
    imageSize = struct.unpack("Q", messageParts[0])[0]
    imageData = messageParts[1]

    print(f"Image is {imageSize} bytes!")

    print(f"Decoding image.")

    image = cv2.imdecode(np.frombuffer(imageData, dtype=np.uint8), cv2.IMREAD_COLOR)
    if image is None:
        print("Error: Failed to decode image.")
        socket.send_string("ERROR: Image decoding failed")

    print(f"Image decoded. Now beginning AI processing.")
    result = json.dumps(performOCR(image))

    print(f"Processing complete. Sending result back.")
    socket.send_string(result)
    print(f"Sent response: {result}")


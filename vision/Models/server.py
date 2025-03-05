import os
import zmq
from easyOCR import perform_ocr
from efficientNet import classify_image

def run_server():
    # Create ZeroMQ context and socket
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    
    # Bind socket to an endpoint
    # Using IPC transport for local interprocess communication
    endpoint = "ipc:///tmp/python_server_endpoint"
    socket.bind(endpoint)
    
    print(f"Server started on {endpoint}")
    
    try:
        while True:
            # Wait for request from client
            request = socket.recv_string()
            print(f"Received request: {request}")
            
            # Check for exit command
            if request == "exit":
                socket.send_string("Server shutting down")
                break
            
            # Process the request
            parts = request.split(" ", 1)
            if len(parts) != 2:
                result = "ERROR: Invalid request format"
            else:
                task_type, image_path = parts
                if task_type == "CLS":
                    result = classify_image(image_path)  # Calls classifier module
                elif task_type == "OCR":
                    result = perform_ocr(image_path)  # Calls OCR module
                else:
                    result = f"ERROR: Unknown task type '{task_type}'"
            
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
"""
import os
from easyOCR import perform_ocr
from efficientNet import classify_image

 Named Pipes
pipe_in = "/tmp/image_pipe"
pipe_out = "/tmp/result_pipe"


 Create pipes if they don’t exist
for pipe in [pipe_in, pipe_out]:
    if not os.path.exists(pipe):
        os.mkfifo(pipe)

while True:
    with open(pipe_in, "r") as f:
        request = f.readline().strip()
        print("request: " + request)

    #    if request == "exit":
    #        break  # Stop the server when "exit" is received

    parts = request.split(" ", 1)
    if len(parts) != 2:
        result = "ERROR: Invalid request format"
    else:
        task_type, image_path = parts
        if task_type == "CLS":
            #result = classify_image(image_path)  # Calls classifier module
            test = 1
        elif task_type == "OCR":
            result = perform_ocr(image_path)  # Calls OCR module
        else:
            result = "ERROR: Unknown task type"

    with open(pipe_out, "w") as f:
        f.write(result + "\n")

               """


import os
from easyOCR import perform_ocr
from efficientNet import classify_image

# Named Pipes
pipe_in = "/tmp/image_pipe"
pipe_out = "/tmp/result_pipe"

# Create pipes if they don’t exist
for pipe in [pipe_in, pipe_out]:
    if not os.path.exists(pipe):
        os.mkfifo(pipe)

while True:
    with open(pipe_in, "r") as f:
        request = f.readline().strip()

    if request == "exit":
        break  # Stop the server when "exit" is received

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
            result = "ERROR: Unknown task type"

    with open(pipe_out, "w") as f:
        f.write(result + "\n")

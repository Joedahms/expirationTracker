import socket

def getLocalIp():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.connect(("8.8.8.8", 80))  # Connect to an external server
    ip = sock.getsockname()[0]
    sock.close()
    return ip

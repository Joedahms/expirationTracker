import socket

from get_local_ip import getLocalIp

def waitForPiDiscovery(discoveryPort):
    """Wait for a Raspberry Pi discovery request, then send the IP."""
    serverIP = getLocalIp()
    print(f"Waiting for Raspberry Pi discovery on UDP {discoveryPort}...")

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("0.0.0.0", discoveryPort))  # Listen on all interfaces

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


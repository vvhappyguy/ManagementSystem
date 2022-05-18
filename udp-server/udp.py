import socket
import redis
from datetime import datetime

r = redis.Redis(host='localhost', port=49153, db=0, password='redispw', username='default')
localIP     = "0.0.0.0"
localPort   = 20001
bufferSize  = 1024
msgFromServer       = "Hello UDP Client"
bytesToSend         = str.encode(msgFromServer)

# Create a datagram socket
UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# Bind to address and ip
UDPServerSocket.bind((localIP, localPort))
print("UDP server up and listening")

# Listen for incoming datagrams
while(True):
    bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)
    message = bytesAddressPair[0]
    address = bytesAddressPair[1]
    clientMsg = "Message from Client:{}".format(message)
    clientIP  = "Client IP Address:{}".format(address)  
    print(clientMsg)
    print(clientIP)
    now = datetime.now()
    if (str(message).find("Hum") != -1):
        r.set('hum', message[-5:])
        r.set('hum_ts', now.strftime("%c"))
    if (str(message).find("Temp") != -1):
        r.set('temp', message[-5:])
        r.set('temp_ts', now.strftime("%c"))

    # Sending a reply to client
    #UDPServerSocket.sendto(bytesToSend, address)
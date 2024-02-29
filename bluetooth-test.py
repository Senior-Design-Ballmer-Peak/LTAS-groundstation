import bluetooth

# Define the Bluetooth service name and UUID
service_name = "GroundStation"
uuid = "c00a2d1a-0623-4cfc-980c-f11c6c6cd0dd"

# Initialize Bluetooth socket
server_socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

port = 1  # RFCOMM port number
server_socket.bind(("", port))
server_socket.listen(1)

# Advertise the service
bluetooth.advertise_service(server_socket, service_name,
                             service_id=uuid,
                             service_classes=[uuid, bluetooth.SERIAL_PORT_CLASS],
                             profiles=[bluetooth.SERIAL_PORT_PROFILE])

print("Waiting for connection on RFCOMM channel", port)

# Accept incoming connection
client_socket, client_info = server_socket.accept()
print("Accepted connection from", client_info)

# Send message to the connected device
message = "Hello from Groundstation!"
client_socket.send(message.encode())  # Encode the message as bytes before sending

print("Message sent to " + str(client_info))

client_socket.close()
server_socket.close()
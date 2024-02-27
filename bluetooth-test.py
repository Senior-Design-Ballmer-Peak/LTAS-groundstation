import bluetooth

# Define the Bluetooth service name and UUID
service_name = "GroundStation"
uuid = "128-bit random generated"

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
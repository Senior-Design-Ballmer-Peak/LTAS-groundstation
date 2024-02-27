import bluetooth

# Define the Bluetooth service name and UUID
service_name = "GroundStation"
uuid = "128-bit random generated"

# Initialize Bluetooth socket
server_socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
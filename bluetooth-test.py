import bluetooth
import time

# Define the Bluetooth service name and UUID
service_name = "GroundStation"
uuid = "c00a2d1a-0623-4cfc-980c-f11c6c6cd0dd"

# Initialize Bluetooth socket
server_socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

port = 1  # RFCOMM port number

# Accept incoming Bluetooth connection
server_socket.bind(("", port))
server_socket.listen(1)

# Make the Raspberry Pi discoverable
bluetooth.advertise_service(server_socket, service_name,
                             service_id=uuid,
                             service_classes=[uuid, bluetooth.SERIAL_PORT_CLASS],
                             profiles=[bluetooth.SERIAL_PORT_PROFILE])

print("Waiting for Bluetooth connection...")



client_socket, client_info = server_socket.accept()
print("Accepted connection from", client_info)

# Simulate receiving sensor data and forwarding it to the connected device
try:
    while True:
        # Simulate receiving sensor data
        sensor_data = "Sensor data from radio"
        
        # Forward sensor data to the connected device
        client_socket.send(sensor_data.encode())  # Encode the data as bytes before sending
        
        print("Sent sensor data to", client_info)
        
        # Sleep for a while before sending the next data (adjust as needed)
        time.sleep(1)
except KeyboardInterrupt:
    # Handle Ctrl+C gracefully
    print("Stopping...")
finally:
    # Close sockets
    client_socket.close()
    server_socket.close()
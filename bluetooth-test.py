import bluetooth
import time

# Define the Bluetooth service name and UUID
# These identifiers are used for advertising the service and allowing clients to discover and connect to it.
service_name = "groundstation"
uuid = "c00a2d1a-0623-4cfc-980c-f11c6c6cd0dd"

try:
    # Initialize Bluetooth socket for RFCOMM communication
    server_socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

    # Bind the server socket to any available local Bluetooth adapter on RFCOMM port 1
    # bluetooth.PORT_ANY allows the system to automatically select an available port
    server_socket.bind(("", bluetooth.PORT_ANY))
    server_socket.listen(1)

    # Make the Raspberry Pi discoverable by advertising the service
    bluetooth.advertise_service(server_socket, service_name,
                                service_id=uuid,
                                service_classes=[uuid, bluetooth.SERIAL_PORT_CLASS],
                                profiles=[bluetooth.SERIAL_PORT_PROFILE])

    print("Waiting for Bluetooth connection...")

    # Accept an incoming connection request
    # This call is blocking and will wait until a client connects
    client_socket, client_info = server_socket.accept()
    print(f"Accepted connection from {client_info}")

    # Main loop: Simulate receiving sensor data and forwarding it to the connected device
    try:
        while True:
            # Example sensor data to be sent
            sensor_data = "Sensor data from radio"
            
            # Send the sensor data to the connected Bluetooth device
            # Encoding the string as bytes is necessary for transmission over Bluetooth
            client_socket.send(sensor_data.encode())
            
            print(f"Sent sensor data to {client_info}")
            
            # Wait for a second before sending the next piece of data
            time.sleep(1)
    except KeyboardInterrupt:
        # Handle the user pressing CTRL+C gracefully
        print("User requested to stop the server. Stopping...")
    finally:
        # Ensure the client socket is closed to free up resources
        client_socket.close()
except Exception as e:
    # Catch any other exceptions that may occur to avoid crashing the script
    print(f"An error occurred: {e}")
finally:
    # Ensure the server socket is always closed properly to free up the port and resources
    if 'server_socket' in locals() or 'server_socket' in globals():
        server_socket.close()

print("Server stopped.")

# epic bash
# import subprocess

# def configure_bluetooth():
#     # Call the shell script to configure Bluetooth
#     subprocess.run("/home/ballmer/groundstation_code/LTAS-groundstation/bt_config.sh", shell=True, check=True)
#     print("Bluetooth configured: device is now discoverable and pairable.")

# def main():
#     configure_bluetooth()

# if __name__ == "__main__":
#     main()

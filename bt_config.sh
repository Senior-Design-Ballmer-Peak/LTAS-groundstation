#!/usr/bin/expect -f

spawn bluetoothctl
expect "#"

# Power on the Bluetooth adapter
send "power on\r"
expect "Changing power on succeeded"

# Register agent and make default
send "agent on\r"
expect "Agent registered"
send "default-agent\r"
expect "Default agent request successful"

# Make the device discoverable and pairable
send "discoverable on\r"
expect "Changing discoverable on succeeded"
send "pairable on\r"
expect "Changing pairable on succeeded"

# Handle the pairing confirmation
expect "Request confirmation"
send "yes\r"

# You can add a timeout to exit `bluetoothctl` if needed
# set timeout 30
# expect timeout

# Exit bluetoothctl
send "exit\r"
expect eof

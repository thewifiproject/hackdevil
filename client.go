package main

import (
	"bufio"
	"fmt"
	"net"
	"os"
)

func main() {
	serverAddress := "10.0.1.35:4444" // Change this to the server's IP address
	conn, err := net.Dial("tcp", serverAddress)
	if err != nil {
		fmt.Println("Error connecting to server:", err)
		return
	}
	defer conn.Close()

	// Create a scanner to read user input
	scanner := bufio.NewScanner(os.Stdin)

	// Loop to receive and execute commands
	for {
		// Read the server prompt
		prompt := make([]byte, 1024)
		n, err := conn.Read(prompt)
		if err != nil {
			fmt.Println("Error reading prompt from server:", err)
			return
		}

		// Display the prompt
		fmt.Print(string(prompt[:n]))

		// Get command from user input
		scanner.Scan()
		cmd := scanner.Text()

		// Send the command to the server
		_, err = conn.Write([]byte(cmd + "\n"))
		if err != nil {
			fmt.Println("Error sending command to server:", err)
			break
		}

		// If the user types "exit", close the connection
		if cmd == "exit" {
			break
		}

		// Read and display the response from the server
		response := make([]byte, 1024)
		n, err = conn.Read(response)
		if err != nil {
			fmt.Println("Error reading response from server:", err)
			break
		}
		fmt.Println(string(response[:n]))
	}
}

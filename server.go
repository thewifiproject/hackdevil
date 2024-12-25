package main

import (
	"fmt"
	"io"
	"net"
	"os"
	"os/exec"
	"strings"
	"sync"
)

var clientCount int
var mu sync.Mutex

// HandleClient will handle each client's request to run commands
func handleClient(conn net.Conn) {
	defer conn.Close()
	mu.Lock()
	clientCount++
	mu.Unlock()

	// Send an initial prompt to the client
	conn.Write([]byte("Shell> "))

	// Loop to receive commands from the client
	for {
		cmdBuf := make([]byte, 1024)
		n, err := conn.Read(cmdBuf)
		if err != nil {
			if err != io.EOF {
				fmt.Println("Error reading from client:", err)
			}
			break
		}

		cmdInput := string(cmdBuf[:n])
		cmdInput = strings.TrimSpace(cmdInput)

		if cmdInput == "exit" {
			conn.Write([]byte("Goodbye!\n"))
			break
		}

		// Execute the command using cmd.exe on Windows
		cmdOutput, err := exec.Command("cmd", "/C", cmdInput).CombinedOutput()
		if err != nil {
			conn.Write([]byte("Error: " + err.Error() + "\n"))
		} else {
			conn.Write(cmdOutput)
		}

		// Send the prompt again
		conn.Write([]byte("\nShell> "))
	}

	mu.Lock()
	clientCount--
	mu.Unlock()
}

// StartServer listens for incoming connections on the given IP and port
func startServer(ip string, port string) {
	listenAddress := ip + ":" + port
	listener, err := net.Listen("tcp", listenAddress)
	if err != nil {
		fmt.Println("Error starting server:", err)
		return
	}
	defer listener.Close()

	fmt.Printf("Listening on %s...\n", listenAddress)

	for {
		conn, err := listener.Accept()
		if err != nil {
			fmt.Println("Error accepting client:", err)
			continue
		}

		// Display number of connected clients
		mu.Lock()
		fmt.Printf("Number of connected clients: %d\n", clientCount)
		mu.Unlock()

		// Handle the connected client in a separate goroutine
		go handleClient(conn)
	}
}

func main() {
	var lhost, lport string

	// Prompt user for server IP and port
	fmt.Print("Enter LHOST (server IP): ")
	fmt.Scanln(&lhost)
	fmt.Print("Enter LPORT (server port): ")
	fmt.Scanln(&lport)

	// Start the server
	startServer(lhost, lport)
}

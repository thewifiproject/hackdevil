package main

import (
	"bufio"
	"fmt"
	"net"
	"os"
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

	// Read incoming data
	buf := make([]byte, 1024)
	for {
		n, err := conn.Read(buf)
		if err != nil {
			fmt.Printf("[!] Error reading from connection: %s\n", err)
			break
		}
		// Print the received data to the console
		fmt.Printf("[*] Received: %s\n", string(buf[:n]))
	}
}

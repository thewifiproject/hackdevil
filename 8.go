package main

import (
	"fmt"
	"net"
	"os"
	"os/exec"
	"bufio"
	"strings"
)

func main() {
	// Get LHOST and LPORT from user input
	reader := bufio.NewReader(os.Stdin)
	fmt.Print("ENTER LHOST: ")
	lhost, _ := reader.ReadString('\n')
	lhost = strings.TrimSpace(lhost)
	
	fmt.Print("ENTER LPORT: ")
	lport, _ := reader.ReadString('\n')
	lport = strings.TrimSpace(lport)

	// Listen on the specified IP and port
	listenAddress := lhost + ":" + lport
	ln, err := net.Listen("tcp", listenAddress)
	if err != nil {
		fmt.Println("Error starting listener:", err)
		return
	}
	defer ln.Close()

	fmt.Println("Listening on", listenAddress)

	// Accept incoming connections
	conn, err := ln.Accept()
	if err != nil {
		fmt.Println("Error accepting connection:", err)
		return
	}
	defer conn.Close()

	// Interact with the client
	fmt.Println("Client connected!")
	for {
		// Prompt for shell commands
		fmt.Print("Shell> ")
		cmdInput, _ := reader.ReadString('\n')
		cmdInput = strings.TrimSpace(cmdInput)

		// Send the command to the client
		conn.Write([]byte(cmdInput + "\n"))

		// Receive the command output from the client
		buffer := make([]byte, 1024)
		n, _ := conn.Read(buffer)
		fmt.Println("Output:\n" + string(buffer[:n]))
	}
}

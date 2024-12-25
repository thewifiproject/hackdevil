package main

import (
    "fmt"
    "log"

    "github.com/bwmarrin/discordgo"
)

const token = "YOUR_BOT_TOKEN"

func main() {
    // Create a new Discord session using the provided bot token
    dg, err := discordgo.New("Bot " + token)
    if err != nil {
        log.Fatalf("error creating Discord session: %v", err)
    }

    // Register the messageCreate function as a callback for MessageCreate events
    dg.AddHandler(messageCreate)

    // Open a websocket connection to Discord and begin listening.
    err = dg.Open()
    if err != nil {
        log.Fatalf("error opening connection: %v", err)
    }

    fmt.Println("Bot is now running. Press CTRL+C to exit.")
    select {}
}

// This function will be called when the bot receives a message
func messageCreate(s *discordgo.Session, m *discordgo.MessageCreate) {
    // Ignore messages from the bot itself
    if m.Author.ID == s.State.User.ID {
        return
    }

    if m.Content == "!hello" {
        s.ChannelMessageSend(m.ChannelID, "Hello, "+m.Author.Username+"!")
    }
}

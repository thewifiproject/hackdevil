const { Client, GatewayIntentBits, EmbedBuilder } = require('discord.js');
const fs = require('fs');
const path = require('path');
const readline = require('readline');
const difflib = require('difflib');

// Define offensive words and banned domains (initial configuration)
let OFFENSIVE_WORDS = ["negr", "debil", "hajzl", "píčo", "hajzle", "kokot", "connard"];
let BANNED_DOMAINS = ["pornhub.com", "fr.pornhub.com", "xnxx.com"];

// Load or save configuration
function loadConfig() {
    try {
        const config = JSON.parse(fs.readFileSync("config.json", "utf8"));
        OFFENSIVE_WORDS = config.offensive_words || OFFENSIVE_WORDS;
        BANNED_DOMAINS = config.banned_domains || BANNED_DOMAINS;
    } catch (err) {
        saveConfig();
    }
}

function saveConfig() {
    const config = { offensive_words: OFFENSIVE_WORDS, banned_domains: BANNED_DOMAINS };
    fs.writeFileSync("config.json", JSON.stringify(config, null, 2));
}

// Initialize bot
const client = new Client({
    intents: [
        GatewayIntentBits.Guilds,
        GatewayIntentBits.GuildMessages,
        GatewayIntentBits.MessageContent,
        GatewayIntentBits.GuildMembers
    ]
});

// Logging channel (set this to your desired channel ID)
const LOG_CHANNEL_ID = '123456789012345678'; // Replace with your log channel ID

// Offense tracking
let offenseCount = {};

// Helper functions
function checkOffensiveWords(content) {
    for (const word of OFFENSIVE_WORDS) {
        // Exact word match or fuzzy match with a threshold
        const wordRegex = new RegExp(`\\b${word}\\b`, 'i');
        if (wordRegex.test(content) || difflib.getCloseMatches(content.toLowerCase(), [word], 1, 0.8).length > 0) {
            return true;
        }
    }
    return false;
}

function checkBannedDomains(content) {
    for (const domain of BANNED_DOMAINS) {
        const domainRegex = new RegExp(`http[s]?://(?:[a-zA-Z]|[0-9]|[$-_@.&+]|[!*\\(\\),]|(?:%[0-9a-fA-F][0-9a-fA-F]))+${domain}`, 'i');
        if (domainRegex.test(content)) {
            return true;
        }
    }
    return false;
}

// Event when bot is ready
client.once('ready', () => {
    console.log(`Logged in as ${client.user.tag}`);
    loadConfig();
});

// Event when a message is received
client.on('messageCreate', async (message) => {
    if (message.author.bot) return;

    // Track offenses
    const userId = message.author.id;
    if (!offenseCount[userId]) offenseCount[userId] = 0;

    // Offensive language check
    if (checkOffensiveWords(message.content)) {
        offenseCount[userId] += 1;
        await message.delete();
        await message.channel.send(`${message.author}, your message contains offensive language. This is offense #${offenseCount[userId]}.`);

        await logOffense(message, "Used offensive language");

        if (offenseCount[userId] >= 3) {
            await timeoutUser(message.author, 15, "Repeated use of offensive language");
            offenseCount[userId] = 0;  // Reset offense count after timeout
        }
    }

    // Banned domain link check
    if (checkBannedDomains(message.content)) {
        offenseCount[userId] += 1;
        await message.delete();
        await message.channel.send(`${message.author}, sharing prohibited links is not allowed. You have been banned for this offense.`);
        await message.author.ban({ reason: "Shared prohibited links" });

        await logOffense(message, "Shared prohibited links", true);
    }
});

// Log offense to log channel
async function logOffense(message, reason, banned = false) {
    const logChannel = await client.channels.fetch(LOG_CHANNEL_ID);
    if (logChannel) {
        const embed = new EmbedBuilder()
            .setTitle('Moderation Action Taken')
            .setColor(banned ? 0xFF0000 : 0xFFA500)
            .setTimestamp(message.createdAt)
            .addFields(
                { name: 'User', value: `${message.author.tag} (${message.author.id})`, inline: false },
                { name: 'Reason', value: reason, inline: false },
                { name: 'Message Content', value: message.content, inline: false },
                { name: 'Channel', value: message.channel.name, inline: false }
            );
        await logChannel.send({ embeds: [embed] });
    }
}

// Timeout user
async function timeoutUser(user, minutes, reason) {
    try {
        const duration = minutes * 60 * 1000; // convert to milliseconds
        await user.timeout(duration, reason);
        console.log(`Timed out ${user.tag} for ${minutes} minutes.`);
    } catch (err) {
        console.error(`Failed to timeout ${user.tag}: ${err}`);
    }
}

// Command handling (Admin commands to modify lists)
client.on('messageCreate', async (message) => {
    if (message.author.bot) return;
    const args = message.content.split(' ');

    // Only allow admin commands
    if (!message.member.permissions.has('Administrator')) return;

    if (args[0] === '!add_word') {
        const word = args.slice(1).join(' ').toLowerCase();
        OFFENSIVE_WORDS.push(word);
        saveConfig();
        message.channel.send(`The word '${word}' has been added to the offensive words list.`);
    }

    if (args[0] === '!remove_word') {
        const word = args.slice(1).join(' ').toLowerCase();
        const index = OFFENSIVE_WORDS.indexOf(word);
        if (index !== -1) {
            OFFENSIVE_WORDS.splice(index, 1);
            saveConfig();
            message.channel.send(`The word '${word}' has been removed from the offensive words list.`);
        } else {
            message.channel.send(`The word '${word}' is not in the offensive words list.`);
        }
    }

    if (args[0] === '!add_domain') {
        const domain = args.slice(1).join(' ').toLowerCase();
        BANNED_DOMAINS.push(domain);
        saveConfig();
        message.channel.send(`The domain '${domain}' has been added to the banned domains list.`);
    }

    if (args[0] === '!remove_domain') {
        const domain = args.slice(1).join(' ').toLowerCase();
        const index = BANNED_DOMAINS.indexOf(domain);
        if (index !== -1) {
            BANNED_DOMAINS.splice(index, 1);
            saveConfig();
            message.channel.send(`The domain '${domain}' has been removed from the banned domains list.`);
        } else {
            message.channel.send(`The domain '${domain}' is not in the banned domains list.`);
        }
    }

    // New commands to list current offensive words and domains
    if (args[0] === '!list_words') {
        message.channel.send(`Current offensive words list: \n${OFFENSIVE_WORDS.join(', ')}`);
    }

    if (args[0] === '!list_domains') {
        message.channel.send(`Current banned domains list: \n${BANNED_DOMAINS.join(', ')}`);
    }
});

// Run the bot with the token from environment variable
client.login(process.env.DISCORD_TOKEN);

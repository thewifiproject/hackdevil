const { Client, Intents, MessageEmbed } = require('discord.js');
const { prefix, token, logChannelId, ownerId } = require('./config.json'); // Import configuration
const fs = require('fs');
const path = require('path');
const regex = require('regex-fuzzy');
const { promisify } = require('util');
const setTimeoutAsync = promisify(setTimeout);

// Define offensive words and banned domains (initial configuration)
let OFFENSIVE_WORDS = ["negr", "debil", "hajzl", "píčo", "hajzle", "kokot", "connard" "dick" ];
let BANNED_DOMAINS = ["pornhub.com", "fr.pornhub.com", "xnxx.com"];

let offenseCount = {};

// Helper functions to check for offensive words and banned domains
function checkOffensiveWords(content) {
    return OFFENSIVE_WORDS.some(word => {
        const regexPattern = new RegExp(`\\b${word}\\b`, 'i');
        return regexPattern.test(content) || regex.getCloseMatches(content.toLowerCase(), [word], 1, 0.8).length > 0;
    });
}

function checkBannedDomains(content) {
    return BANNED_DOMAINS.some(domain => {
        const regexPattern = new RegExp(`http[s]?://(?:[a-zA-Z]|[0-9]|[$-_@.&+]|[!*\\(\\),]|(?:%[0-9a-fA-F][0-9a-fA-F]))+${domain}`, 'i');
        return regexPattern.test(content);
    });
}

// Initialize bot with necessary intents
const client = new Client({
    intents: [Intents.FLAGS.GUILDS, Intents.FLAGS.GUILD_MESSAGES, Intents.FLAGS.DIRECT_MESSAGES, Intents.FLAGS.GUILD_MEMBERS]
});

// Load or save configuration
function loadConfig() {
    try {
        const config = JSON.parse(fs.readFileSync(path.join(__dirname, 'config.json')));
        OFFENSIVE_WORDS = config.offensive_words || OFFENSIVE_WORDS;
        BANNED_DOMAINS = config.banned_domains || BANNED_DOMAINS;
    } catch (err) {
        saveConfig();
    }
}

function saveConfig() {
    const config = {
        offensive_words: OFFENSIVE_WORDS,
        banned_domains: BANNED_DOMAINS
    };
    fs.writeFileSync(path.join(__dirname, 'config.json'), JSON.stringify(config, null, 4));
}

// Event when bot is ready
client.once('ready', () => {
    console.log(`Logged in as ${client.user.tag}!`);
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

        await logOffense(message, 'Used offensive language');

        if (offenseCount[userId] >= 3) {
            await timeoutUser(message.author, 15, 'Repeated use of offensive language');
            offenseCount[userId] = 0; // Reset offense count after timeout
        }
    }

    // Banned domain link check
    if (checkBannedDomains(message.content)) {
        offenseCount[userId] += 1;
        await message.delete();
        await message.channel.send(`${message.author}, sharing prohibited links is not allowed. You have been banned for this offense.`);
        await message.author.ban({ reason: 'Shared prohibited links' });

        await logOffense(message, 'Shared prohibited links', true);
    }
});

// Log offense to log channel
async function logOffense(message, reason, banned = false) {
    const logChannel = await client.channels.fetch(logChannelId);
    if (logChannel) {
        const embed = new MessageEmbed()
            .setTitle('Moderation Action Taken')
            .setColor(banned ? 'RED' : 'ORANGE')
            .setTimestamp(message.createdAt)
            .addField('User', `${message.author.tag} (${message.author.id})`, false)
            .addField('Reason', reason, false)
            .addField('Message Content', message.content, false)
            .addField('Channel', message.channel.name, false);
        
        await logChannel.send({ embeds: [embed] });
    }
}

// Timeout user
async function timeoutUser(user, minutes, reason) {
    try {
        const duration = minutes * 60 * 1000; // Convert minutes to milliseconds
        await user.timeout(duration, reason);
        console.log(`Timed out ${user.tag} for ${minutes} minutes.`);
    } catch (err) {
        console.error(`Failed to timeout ${user.tag}: ${err}`);
    }
}

// Check if the user is the owner of the bot (bypass check)
async function isOwner(user) {
    return user.id === ownerId;
}

// Ban command
client.on('messageCreate', async (message) => {
    if (message.content.startsWith(`${prefix}ban`)) {
        if (!message.member.permissions.has('BAN_MEMBERS') && !await isOwner(message.author)) {
            return message.reply("You do not have permission to ban members.");
        }

        const args = message.content.split(' ').slice(1);
        const member = message.mentions.members.first();
        const reason = args.slice(1).join(' ') || 'No reason provided';

        if (!member) return message.reply("Please mention a valid member to ban.");
        
        if (member.id === message.author.id) return message.reply("You cannot ban yourself.");

        try {
            if (await isOwner(message.author)) {
                await member.ban({ reason });
                await message.channel.send(`${member} has been banned for: ${reason}`);
            } else {
                await member.ban({ reason });
                await message.channel.send(`${member} has been banned for: ${reason}`);
            }
        } catch (err) {
            message.reply(`Failed to ban ${member.tag}.`);
        }
    }
});

// Add offensive word command (admin only)
client.on('messageCreate', async (message) => {
    if (message.content.startsWith(`${prefix}addword`) && message.member.permissions.has('ADMINISTRATOR')) {
        const args = message.content.split(' ').slice(1);
        const word = args.join(' ').toLowerCase();

        if (!word) return message.reply("Please provide a word to add.");

        OFFENSIVE_WORDS.push(word);
        saveConfig();
        message.reply(`The word '${word}' has been added to the offensive words list.`);
    }
});

// Remove offensive word command (admin only)
client.on('messageCreate', async (message) => {
    if (message.content.startsWith(`${prefix}removeword`) && message.member.permissions.has('ADMINISTRATOR')) {
        const args = message.content.split(' ').slice(1);
        const word = args.join(' ').toLowerCase();

        if (!word) return message.reply("Please provide a word to remove.");

        const index = OFFENSIVE_WORDS.indexOf(word);
        if (index !== -1) {
            OFFENSIVE_WORDS.splice(index, 1);
            saveConfig();
            message.reply(`The word '${word}' has been removed from the offensive words list.`);
        } else {
            message.reply(`The word '${word}' is not in the offensive words list.`);
        }
    }
});

// Add banned domain command (admin only)
client.on('messageCreate', async (message) => {
    if (message.content.startsWith(`${prefix}adddomain`) && message.member.permissions.has('ADMINISTRATOR')) {
        const args = message.content.split(' ').slice(1);
        const domain = args.join(' ').toLowerCase();

        if (!domain) return message.reply("Please provide a domain to add.");

        BANNED_DOMAINS.push(domain);
        saveConfig();
        message.reply(`The domain '${domain}' has been added to the banned domains list.`);
    }
});

// Remove banned domain command (admin only)
client.on('messageCreate', async (message) => {
    if (message.content.startsWith(`${prefix}removedomain`) && message.member.permissions.has('ADMINISTRATOR')) {
        const args = message.content.split(' ').slice(1);
        const domain = args.join(' ').toLowerCase();

        if (!domain) return message.reply("Please provide a domain to remove.");

        const index = BANNED_DOMAINS.indexOf(domain);
        if (index !== -1) {
            BANNED_DOMAINS.splice(index, 1);
            saveConfig();
            message.reply(`The domain '${domain}' has been removed from the banned domains list.`);
        } else {
            message.reply(`The domain '${domain}' is not in the banned domains list.`);
        }
    }
});

// Login to Discord with the app's token
client.login(token);

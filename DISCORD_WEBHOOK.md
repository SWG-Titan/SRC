# Discord Webhook Integration

## Overview

This feature provides a lightweight Discord webhook integration that sends server statistics to a Discord channel every 5 minutes using curl (no Discord SDKs or bot tokens required).

## Features

- **Lightweight**: Uses curl for HTTP requests, no external Discord SDKs
- **Periodic Updates**: Automatically sends server statistics every 5 minutes
- **Embedded Messages**: Sends data as nicely formatted Discord embeds
- **System Metrics**: Includes uptime, memory usage, CPU, and disk monitoring
- **Game Server Metrics**: Tracks players online, objects, creatures, buildings, and AI NPCs
- **Configurable**: Easy to enable/disable and configure webhook URL

## Configuration

Add the following configuration options to your game server configuration file (e.g., `gameServer.cfg`):

```ini
[ServerUtility]
# Enable or disable Discord webhook
discordWebhookEnabled=true

# Discord webhook URL (create this in your Discord server settings)
discordWebhookUrl=https://discord.com/api/webhooks/YOUR_WEBHOOK_ID/YOUR_WEBHOOK_TOKEN
```

## How to Get a Discord Webhook URL

1. Open your Discord server
2. Go to Server Settings → Integrations → Webhooks
3. Click "New Webhook"
4. Give it a name (e.g., "SWG Server Monitor")
5. Choose the channel where messages should be posted
6. Click "Copy Webhook URL"
7. Paste the URL into your configuration file

## Statistics Sent

The webhook sends the following information in a Discord embed:

### Game Server Stats
- **Server Name**: The scene/world name (e.g., "tatooine")
- **Process ID**: The process ID of the game server
- **Players Online**: Number of connected players
- **Total Objects**: Total number of objects in the game world
- **Creatures**: Number of creature objects
- **Buildings**: Number of building objects
- **AI NPCs**: Number of AI-controlled NPCs

### System Stats (Linux only)
- **Uptime**: System uptime in days, hours, and minutes
- **Memory**: Memory usage with total and percentage
- **CPU**: CPU monitoring status
- **Disk**: Disk monitoring status

### Additional Info
- **Timestamp**: ISO8601 timestamp of when the report was generated
- **Footer**: "SWG Server Monitor"

## Example Discord Message

The message will appear in Discord as a green embed with the title "Server Statistics Report" and will look like:

```
Server Statistics Report
━━━━━━━━━━━━━━━━━━━━

Server Name: tatooine
Process ID: 12345
Players Online: 42
Total Objects: 15234
Creatures: 3421
Buildings: 234
AI NPCs: 1523

Uptime: 5d 12h 34m
Memory: 8.2 GB / 16.0 GB (51.2%)
CPU: Monitored
Disk: Monitored

SWG Server Monitor | Jan 28, 2026 8:14 AM
```

## Implementation Details

### Files Added

- `engine/server/library/serverUtility/include/public/serverUtility/DiscordWebhook.h`
- `engine/server/library/serverUtility/src/shared/DiscordWebhook.cpp`

### Files Modified

- `engine/server/library/serverUtility/src/CMakeLists.txt` - Added DiscordWebhook.cpp and curl linking
- `engine/server/library/serverUtility/src/shared/ConfigServerUtility.h` - Added configuration accessors
- `engine/server/library/serverUtility/src/shared/ConfigServerUtility.cpp` - Added configuration reading
- `engine/server/library/serverGame/src/shared/core/GameServer.cpp` - Integrated webhook into main loop

### Dependencies

- libcurl (already a dependency of the project)
- Standard C++ libraries

## Technical Notes

1. **5-Minute Interval**: The webhook checks time every frame but only sends when 5 minutes have elapsed since the last send
2. **Non-blocking**: Uses curl with a 10-second timeout to avoid blocking the game server
3. **Error Handling**: Logs errors but doesn't crash the server if Discord is unreachable
4. **JSON Formatting**: Manually formats JSON to avoid external JSON library dependencies
5. **Security**: The webhook URL contains the token, so keep your config file secure

## Troubleshooting

### Webhook Not Sending

1. Verify `discordWebhookEnabled=true` in config
2. Verify the webhook URL is correct
3. Check server logs for Discord webhook errors
4. Test the webhook URL manually with curl:
   ```bash
   curl -H "Content-Type: application/json" \
        -d '{"content":"Test message"}' \
        YOUR_WEBHOOK_URL
   ```

### Invalid Webhook URL

- Make sure you copied the entire URL including the token
- The URL should start with `https://discord.com/api/webhooks/`
- Don't add quotes around the URL in the config file

### Statistics Not Updating

- The statistics are sent every 5 minutes, not every frame
- Wait at least 5 minutes after server start for the first message
- Check that the game server is fully initialized and connected

## Future Enhancements

Possible future improvements:

- Configurable send interval
- Alert on specific events (player count thresholds, errors, etc.)
- Color-coded embeds based on server health
- Additional metrics from GameServerMetricsData
- HTML page parsing for additional statistics (if available)
- Separate webhooks for different alert types

## License

Copyright 2026. Part of the SWG Server codebase.

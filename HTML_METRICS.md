# HTML Metrics Aggregation System

## Overview

The MetricsServer now includes an HTML generator that collects statistics from all game servers and aggregates them into a single HTML file. This provides a unified view of all server statistics across the entire cluster.

## Features

- **Centralized Metrics**: Collects data from all connected game servers
- **Real-time Updates**: Automatically updates HTML file at configurable intervals
- **Web-Friendly Format**: Generates styled HTML that can be served by any web server
- **Auto-Refresh**: HTML page automatically refreshes every 60 seconds
- **Server Status**: Shows connection status for each game server
- **Aggregate Statistics**: Displays cluster-wide totals (players, objects, etc.)

## Architecture

The system leverages the existing MetricsServer infrastructure:

1. **Game Servers** → Send metrics to MetricsServer via `MetricsDataMessage`
2. **MetricsServer** → `MetricsGatheringConnection` collects data from each server
3. **HtmlMetricsGenerator** → Aggregates data and generates HTML file
4. **HTML Output** → Can be served by Apache/Nginx/any web server

## Configuration

Add the following to your `metricsServer.cfg`:

```ini
[MetricsServer]
# Enable HTML metrics generation
htmlGeneratorEnabled=true

# Path to output HTML file (can be in web server directory)
htmlOutputPath=/var/www/html/metrics.html

# Update interval in seconds (default: 60)
htmlUpdateInterval=60
```

### Configuration Options

- **htmlGeneratorEnabled** (boolean, default: false)
  - Enables or disables HTML generation
  - Set to `true` to activate

- **htmlOutputPath** (string, default: "metrics.html")
  - Full path where HTML file will be written
  - Can be absolute or relative path
  - Examples:
    - `/var/www/html/metrics.html` - For Apache/Nginx
    - `./metrics.html` - Relative to MetricsServer directory
    - `/home/swg/public_html/status.html` - User directory

- **htmlUpdateInterval** (integer, default: 60)
  - How often to regenerate HTML file (in seconds)
  - Minimum recommended: 30 seconds
  - Higher values reduce disk I/O but data is less current

## HTML Output

The generated HTML includes:

### Summary Section
- Connected Servers count
- Total Players across all servers
- Total Objects across all servers
- Total Servers (including disconnected)

### Server Details Table
For each game server:
- Server Name
- Scene ID (planet/zone)
- Process ID
- Connection Status (Connected/Disconnected)
- Players Online
- Total Objects
- Creatures
- Buildings
- AI NPCs

### Styling
- Dark theme for easy viewing
- Responsive design works on desktop and mobile
- Color-coded status indicators
- Auto-refresh every 60 seconds

## Web Server Integration

### Apache Example

```apache
# In your Apache config or .htaccess
<Directory /var/www/html>
    Options -Indexes
    Order allow,deny
    Allow from all
</Directory>

# Optional: Restrict access
<Files "metrics.html">
    AuthType Basic
    AuthName "Server Metrics"
    AuthUserFile /etc/apache2/.htpasswd
    Require valid-user
</Files>
```

### Nginx Example

```nginx
server {
    listen 80;
    server_name metrics.example.com;
    
    location /metrics.html {
        root /var/www/html;
        
        # Optional: Basic auth
        auth_basic "Server Metrics";
        auth_basic_user_file /etc/nginx/.htpasswd;
        
        # Disable caching
        add_header Cache-Control "no-cache, no-store, must-revalidate";
    }
}
```

### Simple Python HTTP Server

For quick testing:

```bash
cd /path/to/html/output
python3 -m http.server 8000
# Access at http://localhost:8000/metrics.html
```

## Metrics Collected

The system collects all metrics sent by game servers, including:

- **numClients** - Number of connected players
- **numObjects** - Total objects in world
- **numCreatures** - Creature objects
- **numBuildings** - Building objects
- **numAI** - AI-controlled NPCs
- And more (depends on GameServerMetricsData)

## Troubleshooting

### HTML file not being created

1. Check that `htmlGeneratorEnabled=true` in config
2. Verify MetricsServer has write permissions to output path
3. Check MetricsServer logs for errors
4. Ensure path directory exists

### Data not updating

1. Verify game servers are connected to MetricsServer
2. Check `htmlUpdateInterval` setting
3. Wait at least one update interval after server start
4. Check game server logs for metrics sending errors

### Some servers not appearing

1. Ensure game servers are configured to send metrics
2. Check network connectivity between game servers and MetricsServer
3. Verify MetricsServer port is accessible
4. Check if servers are sending MetricsInitiationMessage

### HTML shows old data

1. Check web browser cache (Ctrl+F5 to hard refresh)
2. Verify MetricsServer is running
3. Check file modification timestamp on HTML file
4. Increase `htmlUpdateInterval` if too aggressive

## Security Considerations

The HTML file contains server statistics that may be sensitive:

1. **Access Control**: Use web server authentication to restrict access
2. **Firewall**: Limit access to trusted networks/IPs
3. **HTTPS**: Use SSL/TLS for encrypted transmission
4. **File Permissions**: Set restrictive permissions on HTML file
   ```bash
   chmod 640 /var/www/html/metrics.html
   chown metricsserver:www-data /var/www/html/metrics.html
   ```

## Example HTML Output

The generated HTML will look like:

```
==================================================
Star Wars Galaxies - Server Metrics
==================================================

[Summary Cards]
Connected Servers: 5
Total Players: 247
Total Objects: 45,234
Total Servers: 5

[Server Table]
Server Name | Scene      | PID   | Status     | Players | Objects | ...
------------|------------|-------|------------|---------|---------|-----
GameServer  | tatooine   | 12345 | Connected  | 42      | 15,234  | ...
GameServer  | naboo      | 12346 | Connected  | 38      | 12,456  | ...
GameServer  | corellia   | 12347 | Connected  | 67      | 17,544  | ...
...

Last Updated: 2026-01-28 08:30:15
```

## Integration with Discord Webhook

This system complements the Discord webhook feature:

- **Discord**: Real-time notifications every 5 minutes
- **HTML**: Persistent web-accessible dashboard
- Both can run simultaneously
- HTML provides more detailed view
- Discord provides push notifications

## Performance Impact

- **CPU**: Minimal - only generates HTML when interval expires
- **Memory**: Low - stores metrics in memory, no database
- **Disk**: One write per update interval (configurable)
- **Network**: No additional network traffic (uses existing metrics)

## Future Enhancements

Possible improvements:

- Historical graphs (requires database)
- Custom metric selection
- Multiple HTML templates
- JSON API endpoint
- WebSocket real-time updates
- Per-server detail pages
- Alert thresholds and notifications

## License

Copyright 2026. Part of the SWG Server codebase.

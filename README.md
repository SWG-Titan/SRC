# Star Wars Galaxies Source Code (C++) Repository

This is the main server code for SWGSource 1.2 as originally forked from the https://bitbucket.org/stellabellumswg/ repository.  Please see that repository for original publication and alteration credit.

# Features

## Discord Webhook Integration

The server now includes a lightweight Discord webhook integration for monitoring server statistics. This feature sends formatted embed messages to Discord every 5 minutes with server metrics including:

- System metrics (uptime, memory usage)
- Player counts and online status
- Object counts (creatures, buildings, AI NPCs)
- Process information

For setup instructions, see [DISCORD_WEBHOOK.md](DISCORD_WEBHOOK.md).

## HTML Metrics Aggregation

The MetricsServer now includes an HTML generator that collects statistics from **all game servers** and aggregates them into a single HTML file. This provides a unified, web-accessible dashboard for monitoring the entire cluster:

- Centralized metrics from all connected game servers
- Real-time aggregate statistics (total players, objects, etc.)
- Per-server status and metrics
- Auto-refreshing HTML page
- Dark theme with responsive design
- Can be served by any web server (Apache, Nginx, etc.)

For setup instructions, see [HTML_METRICS.md](HTML_METRICS.md).

# Works in progress
* 64-bit-types - fully 64 bit version that builds and runs completely.

# Building

For local testing, and non-live builds set MODE=Release or MODE=Debug in the build.properties file in swg-main.

For production, user facing builds, set MODE=MINSIZEREL for profile built, heavily optimized versions of the binaries.

## Profiling and Using Profiles (IN-WORK)

To generate new profiles, build SWG with MODE=RELWITHDEBINFO. 

Add export LLVM_PROFILE_FILE="output-%p.profraw" to your startServer.sh file. 

WHILE THE SERVER IS RUNNING do a ps -a to get the pid's of each SWG executable. And take note of which ones are which.

After you cleanly exit (shutdown) the server, and ctrl+c the LoginServer, move each output-pid.profraw to a folder named for it's process.

Then, proceed to combine them into usable profiles for the compiler:

llvm-profdata merge -output=code.profdata output-*.profraw

Finally, then replace the profdata files with the updated versions, within the src/ tree.

See http://clang.llvm.org/docs/UsersManual.html#profiling-with-instrumentation for more information.

# More Information

See https://swg-source.github.io/ for more information on the SWG Source project.

Join the SWGSource Discord if you would like to contribute:  https://discord.gg/j53cMj9

// DiscordWebhook.h
// Copyright 2026

#ifndef _DISCORD_WEBHOOK_H
#define _DISCORD_WEBHOOK_H

//-----------------------------------------------------------------------

#include <string>
#include <map>

//-----------------------------------------------------------------------

/**
 * Lightweight Discord webhook library using curl
 * Sends messages to Discord as embeds without requiring Discord SDK or bot tokens
 */
class DiscordWebhook
{
public:
	struct EmbedField
	{
		std::string name;
		std::string value;
		bool isInline;

		EmbedField(const std::string& n, const std::string& v, bool inline_val = false)
			: name(n), value(v), isInline(inline_val) {}
	};

	struct Embed
	{
		std::string title;
		std::string description;
		int color; // RGB color as integer
		std::vector<EmbedField> fields;
		std::string footer;
		std::string timestamp; // ISO8601 timestamp

		Embed() : color(0x0099ff) {}
	};

	static void install();
	static void remove();
	static DiscordWebhook& getInstance();

	// Send a Discord embed to the configured webhook URL
	bool sendEmbed(const Embed& embed);

	// Build and send server statistics as an embed
	void sendServerStatistics();
	
	// Build and send server statistics with additional game stats
	void sendServerStatistics(const std::map<std::string, std::string>& additionalStats);

	// Enable/disable the webhook
	void setEnabled(bool enabled);
	bool isEnabled() const;

	// Set the webhook URL
	void setWebhookUrl(const std::string& url);
	const std::string& getWebhookUrl() const;

private:
	DiscordWebhook();
	~DiscordWebhook();

	// Disabled
	DiscordWebhook(const DiscordWebhook&);
	DiscordWebhook& operator=(const DiscordWebhook&);

	// Format embed as JSON
	std::string formatEmbedJson(const Embed& embed) const;

	// Escape JSON strings
	std::string escapeJson(const std::string& input) const;

	// Collect system statistics
	void collectSystemStats(std::map<std::string, std::string>& stats);

	// Collect game server statistics
	void collectGameServerStats(std::map<std::string, std::string>& stats);

	// Get current timestamp in ISO8601 format
	std::string getCurrentTimestamp() const;

	// Send HTTP POST request using curl
	bool sendPostRequest(const std::string& url, const std::string& jsonData);

private:
	static DiscordWebhook* ms_instance;
	bool m_enabled;
	std::string m_webhookUrl;
	unsigned long m_lastSendTime;
	static const unsigned long SEND_INTERVAL_SECONDS = 300; // 5 minutes
};

//-----------------------------------------------------------------------

#endif // _DISCORD_WEBHOOK_H

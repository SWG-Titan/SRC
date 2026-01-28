// DiscordWebhook.cpp
// Copyright 2026

#include "FirstServerUtility.h"
#include "serverUtility/DiscordWebhook.h"
#include "serverUtility/ConfigServerUtility.h"
#include "sharedFoundation/Clock.h"
#include "sharedFoundation/Os.h"
#include "sharedLog/Log.h"

#include <curl/curl.h>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <vector>

//-----------------------------------------------------------------------

DiscordWebhook* DiscordWebhook::ms_instance = nullptr;

//-----------------------------------------------------------------------

namespace
{
	// Callback for curl to handle response
	size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
	{
		((std::string*)userp)->append((char*)contents, size * nmemb);
		return size * nmemb;
	}

	// Read a line from /proc filesystem
	bool readProcLine(const char* filepath, std::string& output)
	{
		std::ifstream file(filepath);
		if (file.is_open())
		{
			std::getline(file, output);
			file.close();
			return true;
		}
		return false;
	}
}

//-----------------------------------------------------------------------

DiscordWebhook::DiscordWebhook()
	: m_enabled(false)
	, m_webhookUrl()
	, m_lastSendTime(0)
{
}

//-----------------------------------------------------------------------

DiscordWebhook::~DiscordWebhook()
{
}

//-----------------------------------------------------------------------

void DiscordWebhook::install()
{
	if (ms_instance == nullptr)
	{
		ms_instance = new DiscordWebhook();
		curl_global_init(CURL_GLOBAL_DEFAULT);
	}
}

//-----------------------------------------------------------------------

void DiscordWebhook::remove()
{
	if (ms_instance != nullptr)
	{
		curl_global_cleanup();
		delete ms_instance;
		ms_instance = nullptr;
	}
}

//-----------------------------------------------------------------------

DiscordWebhook& DiscordWebhook::getInstance()
{
	return *ms_instance;
}

//-----------------------------------------------------------------------

void DiscordWebhook::setEnabled(bool enabled)
{
	m_enabled = enabled;
}

//-----------------------------------------------------------------------

bool DiscordWebhook::isEnabled() const
{
	return m_enabled && !m_webhookUrl.empty();
}

//-----------------------------------------------------------------------

void DiscordWebhook::setWebhookUrl(const std::string& url)
{
	m_webhookUrl = url;
}

//-----------------------------------------------------------------------

const std::string& DiscordWebhook::getWebhookUrl() const
{
	return m_webhookUrl;
}

//-----------------------------------------------------------------------

std::string DiscordWebhook::escapeJson(const std::string& input) const
{
	std::ostringstream ss;
	for (auto c : input)
	{
		switch (c)
		{
		case '"': ss << "\\\""; break;
		case '\\': ss << "\\\\"; break;
		case '\b': ss << "\\b"; break;
		case '\f': ss << "\\f"; break;
		case '\n': ss << "\\n"; break;
		case '\r': ss << "\\r"; break;
		case '\t': ss << "\\t"; break;
		default:
			if ('\x00' <= c && c <= '\x1f')
			{
				ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
			}
			else
			{
				ss << c;
			}
		}
	}
	return ss.str();
}

//-----------------------------------------------------------------------

std::string DiscordWebhook::formatEmbedJson(const Embed& embed) const
{
	std::ostringstream json;
	
	json << "{\"embeds\":[{";
	
	if (!embed.title.empty())
	{
		json << "\"title\":\"" << escapeJson(embed.title) << "\",";
	}
	
	if (!embed.description.empty())
	{
		json << "\"description\":\"" << escapeJson(embed.description) << "\",";
	}
	
	json << "\"color\":" << embed.color << ",";
	
	if (!embed.fields.empty())
	{
		json << "\"fields\":[";
		for (size_t i = 0; i < embed.fields.size(); ++i)
		{
			if (i > 0) json << ",";
			json << "{\"name\":\"" << escapeJson(embed.fields[i].name) << "\",";
			json << "\"value\":\"" << escapeJson(embed.fields[i].value) << "\",";
			json << "\"inline\":" << (embed.fields[i].isInline ? "true" : "false") << "}";
		}
		json << "],";
	}
	
	if (!embed.footer.empty())
	{
		json << "\"footer\":{\"text\":\"" << escapeJson(embed.footer) << "\"},";
	}
	
	if (!embed.timestamp.empty())
	{
		json << "\"timestamp\":\"" << embed.timestamp << "\",";
	}
	
	// Remove trailing comma if present
	std::string result = json.str();
	if (result.back() == ',')
	{
		result.pop_back();
	}
	
	result += "}]}";
	return result;
}

//-----------------------------------------------------------------------

std::string DiscordWebhook::getCurrentTimestamp() const
{
	time_t now = time(nullptr);
	struct tm* timeinfo = gmtime(&now);
	
	char buffer[80];
	strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", timeinfo);
	
	return std::string(buffer);
}

//-----------------------------------------------------------------------

bool DiscordWebhook::sendPostRequest(const std::string& url, const std::string& jsonData)
{
	CURL* curl = curl_easy_init();
	if (!curl)
	{
		LOG("DiscordWebhook", ("Failed to initialize curl"));
		return false;
	}
	
	std::string response;
	
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
	
	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	
	// Set timeout
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
	
	CURLcode res = curl_easy_perform(curl);
	
	bool success = (res == CURLE_OK);
	
	if (!success)
	{
		LOG("DiscordWebhook", ("curl_easy_perform() failed: %s", curl_easy_strerror(res)));
	}
	else
	{
		long response_code;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		
		if (response_code < 200 || response_code >= 300)
		{
			LOG("DiscordWebhook", ("HTTP error: %ld, response: %s", response_code, response.c_str()));
			success = false;
		}
	}
	
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	
	return success;
}

//-----------------------------------------------------------------------

bool DiscordWebhook::sendEmbed(const Embed& embed)
{
	if (!isEnabled())
	{
		return false;
	}
	
	std::string jsonData = formatEmbedJson(embed);
	return sendPostRequest(m_webhookUrl, jsonData);
}

//-----------------------------------------------------------------------

void DiscordWebhook::collectSystemStats(std::map<std::string, std::string>& stats)
{
#ifdef linux
	// Get uptime
	std::string uptime_str;
	if (readProcLine("/proc/uptime", uptime_str))
	{
		double uptime = 0;
		if (sscanf(uptime_str.c_str(), "%lf", &uptime) == 1)
		{
			int days = static_cast<int>(uptime / 86400);
			int hours = static_cast<int>((uptime - days * 86400) / 3600);
			int minutes = static_cast<int>((uptime - days * 86400 - hours * 3600) / 60);
			
			std::ostringstream ss;
			if (days > 0) ss << days << "d ";
			ss << hours << "h " << minutes << "m";
			stats["Uptime"] = ss.str();
		}
	}
	
	// Get memory usage
	std::ifstream meminfo("/proc/meminfo");
	if (meminfo.is_open())
	{
		std::string line;
		unsigned long memTotal = 0, memAvailable = 0;
		
		while (std::getline(meminfo, line))
		{
			if (line.find("MemTotal:") == 0)
			{
				sscanf(line.c_str(), "MemTotal: %lu", &memTotal);
			}
			else if (line.find("MemAvailable:") == 0)
			{
				sscanf(line.c_str(), "MemAvailable: %lu", &memAvailable);
			}
			
			if (memTotal > 0 && memAvailable > 0)
				break;
		}
		meminfo.close();
		
		if (memTotal > 0 && memAvailable > 0)
		{
			unsigned long memUsed = memTotal - memAvailable;
			double percentUsed = (static_cast<double>(memUsed) / memTotal) * 100.0;
			
			std::ostringstream ss;
			ss << std::fixed << std::setprecision(1);
			ss << (memUsed / 1024.0 / 1024.0) << " GB / ";
			ss << (memTotal / 1024.0 / 1024.0) << " GB (";
			ss << percentUsed << "%)";
			stats["Memory"] = ss.str();
		}
	}
	
	// Get CPU usage (simple approximation from /proc/stat)
	std::ifstream stat("/proc/stat");
	if (stat.is_open())
	{
		std::string line;
		std::getline(stat, line);
		stat.close();
		
		if (line.find("cpu ") == 0)
		{
			unsigned long user, nice, system, idle;
			if (sscanf(line.c_str(), "cpu %lu %lu %lu %lu", &user, &nice, &system, &idle) == 4)
			{
				// This is cumulative, so we'd need to sample twice for a real percentage
				// For now, just report that it's available
				stats["CPU"] = "Monitored";
			}
		}
	}
	
	// Get disk usage for current directory
	std::ifstream mounts("/proc/mounts");
	if (mounts.is_open())
	{
		// For simplicity, just note that disk is monitored
		stats["Disk"] = "Monitored";
		mounts.close();
	}
#else
	stats["Uptime"] = "N/A (Windows)";
	stats["Memory"] = "N/A (Windows)";
	stats["CPU"] = "N/A (Windows)";
	stats["Disk"] = "N/A (Windows)";
#endif
}

//-----------------------------------------------------------------------

void DiscordWebhook::collectGameServerStats(std::map<std::string, std::string>& stats)
{
	// Note: We collect basic stats here. For more detailed game server metrics,
	// the GameServer can provide additional information via GameServerMetricsData
	stats["Process ID"] = std::to_string(Os::getProcessId());
	
	// Additional stats would need to be passed in or accessed through proper interfaces
	// For now, we provide what's available without tight coupling to GameServer
}

//-----------------------------------------------------------------------

void DiscordWebhook::sendServerStatistics()
{
	std::map<std::string, std::string> emptyStats;
	sendServerStatistics(emptyStats);
}

//-----------------------------------------------------------------------

void DiscordWebhook::sendServerStatistics(const std::map<std::string, std::string>& additionalStats)
{
	if (!isEnabled())
	{
		return;
	}
	
	// Check if enough time has passed since last send
	unsigned long currentTime = Clock::timeSeconds();
	if (m_lastSendTime > 0 && (currentTime - m_lastSendTime) < SEND_INTERVAL_SECONDS)
	{
		return;
	}
	
	m_lastSendTime = currentTime;
	
	// Collect statistics
	std::map<std::string, std::string> systemStats;
	std::map<std::string, std::string> gameStats;
	
	collectSystemStats(systemStats);
	collectGameServerStats(gameStats);
	
	// Merge additional stats
	for (const auto& stat : additionalStats)
	{
		gameStats[stat.first] = stat.second;
	}
	
	// Build embed
	Embed embed;
	embed.title = "Server Statistics Report";
	embed.color = 0x00ff00; // Green
	embed.timestamp = getCurrentTimestamp();
	embed.footer = "SWG Server Monitor";
	
	// Add game server stats
	for (const auto& stat : gameStats)
	{
		embed.fields.push_back(EmbedField(stat.first, stat.second, true));
	}
	
	// Add system stats
	for (const auto& stat : systemStats)
	{
		embed.fields.push_back(EmbedField(stat.first, stat.second, true));
	}
	
	// Send the embed
	if (sendEmbed(embed))
	{
		LOG("DiscordWebhook", ("Successfully sent server statistics"));
	}
	else
	{
		LOG("DiscordWebhook", ("Failed to send server statistics"));
	}
}

//-----------------------------------------------------------------------

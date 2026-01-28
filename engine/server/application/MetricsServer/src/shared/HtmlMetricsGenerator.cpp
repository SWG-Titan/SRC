// HtmlMetricsGenerator.cpp
// Copyright 2026

#include "FirstMetricsServer.h"
#include "HtmlMetricsGenerator.h"
#include "sharedFoundation/Clock.h"
#include <sstream>
#include <fstream>
#include <iomanip>
#include <ctime>

//-----------------------------------------------------------------------

HtmlMetricsGenerator* HtmlMetricsGenerator::ms_instance = nullptr;

//-----------------------------------------------------------------------

HtmlMetricsGenerator::HtmlMetricsGenerator()
	: m_enabled(false)
	, m_outputPath("metrics.html")
	, m_updateIntervalSeconds(60) // Default: 1 minute
	, m_lastUpdateTime(0)
	, m_serverMetrics()
{
}

//-----------------------------------------------------------------------

HtmlMetricsGenerator::~HtmlMetricsGenerator()
{
}

//-----------------------------------------------------------------------

void HtmlMetricsGenerator::install()
{
	if (ms_instance == nullptr)
	{
		ms_instance = new HtmlMetricsGenerator();
	}
}

//-----------------------------------------------------------------------

void HtmlMetricsGenerator::remove()
{
	if (ms_instance != nullptr)
	{
		delete ms_instance;
		ms_instance = nullptr;
	}
}

//-----------------------------------------------------------------------

HtmlMetricsGenerator& HtmlMetricsGenerator::getInstance()
{
	return *ms_instance;
}

//-----------------------------------------------------------------------

void HtmlMetricsGenerator::setEnabled(bool enabled)
{
	m_enabled = enabled;
}

//-----------------------------------------------------------------------

bool HtmlMetricsGenerator::isEnabled() const
{
	return m_enabled;
}

//-----------------------------------------------------------------------

void HtmlMetricsGenerator::setOutputPath(const std::string& path)
{
	m_outputPath = path;
}

//-----------------------------------------------------------------------

const std::string& HtmlMetricsGenerator::getOutputPath() const
{
	return m_outputPath;
}

//-----------------------------------------------------------------------

void HtmlMetricsGenerator::setUpdateInterval(unsigned long seconds)
{
	m_updateIntervalSeconds = seconds;
}

//-----------------------------------------------------------------------

unsigned long HtmlMetricsGenerator::getUpdateInterval() const
{
	return m_updateIntervalSeconds;
}

//-----------------------------------------------------------------------

void HtmlMetricsGenerator::updateServerMetrics(const std::string& serverId, const std::string& metricName, const std::string& value)
{
	ServerMetrics& server = m_serverMetrics[serverId];
	server.metrics[metricName] = value;
	server.lastUpdateTime = Clock::timeSeconds();
	server.isConnected = true;
}

//-----------------------------------------------------------------------

void HtmlMetricsGenerator::setServerInfo(const std::string& serverId, const std::string& serverName, const std::string& sceneId, int processId)
{
	ServerMetrics& server = m_serverMetrics[serverId];
	server.serverName = serverName;
	server.sceneId = sceneId;
	server.processId = processId;
}

//-----------------------------------------------------------------------

void HtmlMetricsGenerator::setServerConnected(const std::string& serverId, bool connected)
{
	auto it = m_serverMetrics.find(serverId);
	if (it != m_serverMetrics.end())
	{
		it->second.isConnected = connected;
	}
}

//-----------------------------------------------------------------------

std::string HtmlMetricsGenerator::getCurrentTimestamp() const
{
	time_t now = time(nullptr);
	struct tm* timeinfo = localtime(&now);
	
	char buffer[80];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
	
	return std::string(buffer);
}

//-----------------------------------------------------------------------

int HtmlMetricsGenerator::getTotalPlayers() const
{
	int total = 0;
	for (const auto& server : m_serverMetrics)
	{
		if (!server.second.isConnected)
			continue;
			
		auto it = server.second.metrics.find("numClients");
		if (it != server.second.metrics.end())
		{
			total += atoi(it->second.c_str());
		}
	}
	return total;
}

//-----------------------------------------------------------------------

int HtmlMetricsGenerator::getTotalObjects() const
{
	int total = 0;
	for (const auto& server : m_serverMetrics)
	{
		if (!server.second.isConnected)
			continue;
			
		auto it = server.second.metrics.find("numObjects");
		if (it != server.second.metrics.end())
		{
			total += atoi(it->second.c_str());
		}
	}
	return total;
}

//-----------------------------------------------------------------------

int HtmlMetricsGenerator::getConnectedServerCount() const
{
	int count = 0;
	for (const auto& server : m_serverMetrics)
	{
		if (server.second.isConnected)
			++count;
	}
	return count;
}

//-----------------------------------------------------------------------

std::string HtmlMetricsGenerator::generateHtmlHeader() const
{
	std::ostringstream html;
	
	html << "<!DOCTYPE html>\n";
	html << "<html lang=\"en\">\n";
	html << "<head>\n";
	html << "  <meta charset=\"UTF-8\">\n";
	html << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
	html << "  <meta http-equiv=\"refresh\" content=\"60\">\n"; // Auto-refresh every 60 seconds
	html << "  <title>SWG Server Metrics</title>\n";
	html << "  <style>\n";
	html << "    body {\n";
	html << "      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n";
	html << "      background-color: #1a1a1a;\n";
	html << "      color: #e0e0e0;\n";
	html << "      margin: 0;\n";
	html << "      padding: 20px;\n";
	html << "    }\n";
	html << "    .container {\n";
	html << "      max-width: 1400px;\n";
	html << "      margin: 0 auto;\n";
	html << "    }\n";
	html << "    h1 {\n";
	html << "      color: #4CAF50;\n";
	html << "      border-bottom: 2px solid #4CAF50;\n";
	html << "      padding-bottom: 10px;\n";
	html << "    }\n";
	html << "    .summary {\n";
	html << "      background-color: #2d2d2d;\n";
	html << "      border-radius: 8px;\n";
	html << "      padding: 20px;\n";
	html << "      margin: 20px 0;\n";
	html << "      display: grid;\n";
	html << "      grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));\n";
	html << "      gap: 15px;\n";
	html << "    }\n";
	html << "    .summary-item {\n";
	html << "      background-color: #3a3a3a;\n";
	html << "      padding: 15px;\n";
	html << "      border-radius: 5px;\n";
	html << "      text-align: center;\n";
	html << "    }\n";
	html << "    .summary-item .label {\n";
	html << "      font-size: 0.9em;\n";
	html << "      color: #aaa;\n";
	html << "      margin-bottom: 5px;\n";
	html << "    }\n";
	html << "    .summary-item .value {\n";
	html << "      font-size: 2em;\n";
	html << "      font-weight: bold;\n";
	html << "      color: #4CAF50;\n";
	html << "    }\n";
	html << "    table {\n";
	html << "      width: 100%;\n";
	html << "      border-collapse: collapse;\n";
	html << "      background-color: #2d2d2d;\n";
	html << "      border-radius: 8px;\n";
	html << "      overflow: hidden;\n";
	html << "      margin-top: 20px;\n";
	html << "    }\n";
	html << "    th {\n";
	html << "      background-color: #4CAF50;\n";
	html << "      color: white;\n";
	html << "      padding: 12px;\n";
	html << "      text-align: left;\n";
	html << "      font-weight: 600;\n";
	html << "    }\n";
	html << "    td {\n";
	html << "      padding: 12px;\n";
	html << "      border-bottom: 1px solid #444;\n";
	html << "    }\n";
	html << "    tr:hover {\n";
	html << "      background-color: #3a3a3a;\n";
	html << "    }\n";
	html << "    .status-connected {\n";
	html << "      color: #4CAF50;\n";
	html << "      font-weight: bold;\n";
	html << "    }\n";
	html << "    .status-disconnected {\n";
	html << "      color: #f44336;\n";
	html << "      font-weight: bold;\n";
	html << "    }\n";
	html << "    .timestamp {\n";
	html << "      text-align: right;\n";
	html << "      color: #aaa;\n";
	html << "      font-size: 0.9em;\n";
	html << "      margin-top: 20px;\n";
	html << "    }\n";
	html << "  </style>\n";
	html << "</head>\n";
	html << "<body>\n";
	html << "  <div class=\"container\">\n";
	html << "    <h1>Star Wars Galaxies - Server Metrics</h1>\n";
	
	return html.str();
}

//-----------------------------------------------------------------------

std::string HtmlMetricsGenerator::generateHtmlFooter() const
{
	std::ostringstream html;
	
	html << "    <div class=\"timestamp\">\n";
	html << "      Last Updated: " << getCurrentTimestamp() << "\n";
	html << "    </div>\n";
	html << "  </div>\n";
	html << "</body>\n";
	html << "</html>\n";
	
	return html.str();
}

//-----------------------------------------------------------------------

std::string HtmlMetricsGenerator::generateSummarySection() const
{
	std::ostringstream html;
	
	html << "    <div class=\"summary\">\n";
	
	html << "      <div class=\"summary-item\">\n";
	html << "        <div class=\"label\">Connected Servers</div>\n";
	html << "        <div class=\"value\">" << getConnectedServerCount() << "</div>\n";
	html << "      </div>\n";
	
	html << "      <div class=\"summary-item\">\n";
	html << "        <div class=\"label\">Total Players</div>\n";
	html << "        <div class=\"value\">" << getTotalPlayers() << "</div>\n";
	html << "      </div>\n";
	
	html << "      <div class=\"summary-item\">\n";
	html << "        <div class=\"label\">Total Objects</div>\n";
	html << "        <div class=\"value\">" << getTotalObjects() << "</div>\n";
	html << "      </div>\n";
	
	html << "      <div class=\"summary-item\">\n";
	html << "        <div class=\"label\">Total Servers</div>\n";
	html << "        <div class=\"value\">" << m_serverMetrics.size() << "</div>\n";
	html << "      </div>\n";
	
	html << "    </div>\n";
	
	return html.str();
}

//-----------------------------------------------------------------------

std::string HtmlMetricsGenerator::generateServerTable() const
{
	std::ostringstream html;
	
	html << "    <table>\n";
	html << "      <thead>\n";
	html << "        <tr>\n";
	html << "          <th>Server Name</th>\n";
	html << "          <th>Scene</th>\n";
	html << "          <th>Process ID</th>\n";
	html << "          <th>Status</th>\n";
	html << "          <th>Players</th>\n";
	html << "          <th>Objects</th>\n";
	html << "          <th>Creatures</th>\n";
	html << "          <th>Buildings</th>\n";
	html << "          <th>AI NPCs</th>\n";
	html << "        </tr>\n";
	html << "      </thead>\n";
	html << "      <tbody>\n";
	
	for (const auto& serverEntry : m_serverMetrics)
	{
		const ServerMetrics& server = serverEntry.second;
		
		html << "        <tr>\n";
		html << "          <td>" << (server.serverName.empty() ? serverEntry.first : server.serverName) << "</td>\n";
		html << "          <td>" << server.sceneId << "</td>\n";
		html << "          <td>" << server.processId << "</td>\n";
		html << "          <td class=\"" << (server.isConnected ? "status-connected" : "status-disconnected") << "\">";
		html << (server.isConnected ? "Connected" : "Disconnected") << "</td>\n";
		
		// Players (numClients)
		auto it = server.metrics.find("numClients");
		html << "          <td>" << (it != server.metrics.end() ? it->second : "0") << "</td>\n";
		
		// Total Objects (numObjects)
		it = server.metrics.find("numObjects");
		html << "          <td>" << (it != server.metrics.end() ? it->second : "0") << "</td>\n";
		
		// Creatures (numCreatures)
		it = server.metrics.find("numCreatures");
		html << "          <td>" << (it != server.metrics.end() ? it->second : "0") << "</td>\n";
		
		// Buildings (numBuildings)
		it = server.metrics.find("numBuildings");
		html << "          <td>" << (it != server.metrics.end() ? it->second : "0") << "</td>\n";
		
		// AI NPCs (numAI)
		it = server.metrics.find("numAI");
		html << "          <td>" << (it != server.metrics.end() ? it->second : "0") << "</td>\n";
		
		html << "        </tr>\n";
	}
	
	html << "      </tbody>\n";
	html << "    </table>\n";
	
	return html.str();
}

//-----------------------------------------------------------------------

std::string HtmlMetricsGenerator::generateHtml() const
{
	std::string html;
	html += generateHtmlHeader();
	html += generateSummarySection();
	html += generateServerTable();
	html += generateHtmlFooter();
	return html;
}

//-----------------------------------------------------------------------

bool HtmlMetricsGenerator::writeHtmlToFile(const std::string& filePath) const
{
	std::ofstream file(filePath.c_str());
	if (!file.is_open())
	{
		return false;
	}
	
	file << generateHtml();
	file.close();
	
	return true;
}

//-----------------------------------------------------------------------

void HtmlMetricsGenerator::update()
{
	if (!m_enabled)
	{
		return;
	}
	
	unsigned long currentTime = Clock::timeSeconds();
	
	// Check if enough time has passed since last update
	if (m_lastUpdateTime > 0 && (currentTime - m_lastUpdateTime) < m_updateIntervalSeconds)
	{
		return;
	}
	
	m_lastUpdateTime = currentTime;
	
	// Write HTML to file
	if (!writeHtmlToFile(m_outputPath))
	{
		// Log error but don't crash
		DEBUG_REPORT_LOG(true, ("HtmlMetricsGenerator: Failed to write to %s\n", m_outputPath.c_str()));
	}
	else
	{
		DEBUG_REPORT_LOG(true, ("HtmlMetricsGenerator: Updated %s with metrics from %d servers\n", 
			m_outputPath.c_str(), static_cast<int>(m_serverMetrics.size())));
	}
}

//-----------------------------------------------------------------------

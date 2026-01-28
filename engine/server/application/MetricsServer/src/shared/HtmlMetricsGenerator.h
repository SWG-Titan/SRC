// HtmlMetricsGenerator.h
// Copyright 2026

#ifndef _HTML_METRICS_GENERATOR_H
#define _HTML_METRICS_GENERATOR_H

//-----------------------------------------------------------------------

#include <string>
#include <map>
#include <vector>

//-----------------------------------------------------------------------

/**
 * Generates HTML output for aggregated game server metrics
 * Collects data from all connected game servers and formats as HTML table
 */
class HtmlMetricsGenerator
{
public:
	struct ServerMetrics
	{
		std::string serverName;
		std::string sceneId;
		int processId;
		std::map<std::string, std::string> metrics; // metric name -> value
		unsigned long lastUpdateTime;
		bool isConnected;

		ServerMetrics() : processId(0), lastUpdateTime(0), isConnected(false) {}
	};

	static void install();
	static void remove();
	static HtmlMetricsGenerator& getInstance();

	// Update metrics for a specific server
	void updateServerMetrics(const std::string& serverId, const std::string& metricName, const std::string& value);
	void setServerInfo(const std::string& serverId, const std::string& serverName, const std::string& sceneId, int processId);
	void setServerConnected(const std::string& serverId, bool connected);

	// Generate HTML output
	std::string generateHtml() const;
	bool writeHtmlToFile(const std::string& filePath) const;

	// Configuration
	void setEnabled(bool enabled);
	bool isEnabled() const;
	void setOutputPath(const std::string& path);
	const std::string& getOutputPath() const;
	void setUpdateInterval(unsigned long seconds);
	unsigned long getUpdateInterval() const;

	// Periodic update
	void update();

private:
	HtmlMetricsGenerator();
	~HtmlMetricsGenerator();

	// Disabled
	HtmlMetricsGenerator(const HtmlMetricsGenerator&);
	HtmlMetricsGenerator& operator=(const HtmlMetricsGenerator&);

	// Generate HTML components
	std::string generateHtmlHeader() const;
	std::string generateHtmlFooter() const;
	std::string generateServerTable() const;
	std::string generateSummarySection() const;
	std::string getCurrentTimestamp() const;

	// Calculate aggregate statistics
	int getTotalPlayers() const;
	int getTotalObjects() const;
	int getConnectedServerCount() const;

private:
	static HtmlMetricsGenerator* ms_instance;
	
	bool m_enabled;
	std::string m_outputPath;
	unsigned long m_updateIntervalSeconds;
	unsigned long m_lastUpdateTime;
	
	std::map<std::string, ServerMetrics> m_serverMetrics;
};

//-----------------------------------------------------------------------

#endif // _HTML_METRICS_GENERATOR_H

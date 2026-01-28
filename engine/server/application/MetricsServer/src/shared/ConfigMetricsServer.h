// ConfigMetricsServer.h
// copyright 2000 Verant Interactive
// Author: Justin Randall

#ifndef	_ConfigMetricsServer_H
#define	_ConfigMetricsServer_H

//-----------------------------------------------------------------------

class ConfigMetricsServer
{
public:
	struct Data
	{
		const char * authenticationFileName;
		const char * clusterName;
		uint16       metricsListenerPort;
		uint16       metricsServicePort;
		bool         runTestStats;
		uint16       taskManagerPort;
		const char * metricsServiceBindInterface;
		bool         htmlGeneratorEnabled;
		const char * htmlOutputPath;
		int          htmlUpdateInterval;
	};
	
	static void			         install                     ();
	static void			         remove                      ();


	static const char* getAuthenticationFileName();
	static const char* getClusterName();
	static uint16      getMetricsListenerPort();
	static uint16      getMetricsServicePort();
	static bool        getRunTestStats();
	static uint16      getTaskManagerPort();
	static const char* getMetricsServiceBindInterface();
	static bool        getHtmlGeneratorEnabled();
	static const char* getHtmlOutputPath();
	static int         getHtmlUpdateInterval();
		
private:
	static Data *	data;
};

//-----------------------------------------------------------------------

inline const char* ConfigMetricsServer::getAuthenticationFileName()
{
	return data->authenticationFileName;
}
//-----------------------------------------------------------------------

inline const char* ConfigMetricsServer::getClusterName()
{
	return data->clusterName;
}
//-----------------------------------------------------------------------

inline uint16 ConfigMetricsServer::getMetricsListenerPort()
{
	return data->metricsListenerPort;
}
//-----------------------------------------------------------------------

inline uint16 ConfigMetricsServer::getMetricsServicePort()
{
	return data->metricsServicePort;
}
//-----------------------------------------------------------------------

inline bool ConfigMetricsServer::getRunTestStats()
{
	return data->runTestStats;
}
//-----------------------------------------------------------------------

inline uint16 ConfigMetricsServer::getTaskManagerPort()
{
	return data->taskManagerPort;
}

//-----------------------------------------------------------------------

inline const char * ConfigMetricsServer::getMetricsServiceBindInterface()
{
	return data->metricsServiceBindInterface;
}

//-----------------------------------------------------------------------

inline bool ConfigMetricsServer::getHtmlGeneratorEnabled()
{
	return data->htmlGeneratorEnabled;
}

//-----------------------------------------------------------------------

inline const char * ConfigMetricsServer::getHtmlOutputPath()
{
	return data->htmlOutputPath;
}

//-----------------------------------------------------------------------

inline int ConfigMetricsServer::getHtmlUpdateInterval()
{
	return data->htmlUpdateInterval;
}

//-----------------------------------------------------------------------


#endif	// _ConfigMetricsServer_H



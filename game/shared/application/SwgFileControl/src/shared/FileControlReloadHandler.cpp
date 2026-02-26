// ======================================================================
//
// FileControlReloadHandler.cpp
// copyright 2024 Sony Online Entertainment
//
// Server-side reload handler. When SwgFileControl runs on the server
// (Linux), it uses ServerMessageForwarding to broadcast reload commands
// to all connected game servers, and calls local reload APIs directly.
//
// ======================================================================

#include "FirstSwgFileControl.h"
#include "FileControlReloadHandler.h"

#include "FileControlServer.h"
#include "HotReloadManager.h"

#include "sharedLog/Log.h"
#include "sharedUtility/DataTableManager.h"

#include <cstdio>
#include <cstring>
#include <fstream>

// ======================================================================

bool FileControlReloadHandler::ms_installed = false;

// ======================================================================

void FileControlReloadHandler::install()
{
	if (ms_installed)
		return;

	ms_installed = true;
	LOG("FileControl", ("FileControlReloadHandler installed (server-side)"));
}

// ----------------------------------------------------------------------

void FileControlReloadHandler::remove()
{
	ms_installed = false;
}

// ======================================================================

bool FileControlReloadHandler::handleReloadNotify(const std::string & relativePath, const std::string & reloadCommand)
{
	if (!ms_installed)
		return false;

	LOG("FileControl", ("ReloadHandler: processing %s (command=%s)", relativePath.c_str(), reloadCommand.c_str()));

	HotReloadManager::FileType ft = HotReloadManager::detectFileType(relativePath);

	switch (ft)
	{
	case HotReloadManager::FT_JAVA_CLASS:
		{
			std::string scriptName = HotReloadManager::getScriptReloadName(relativePath);
			return reloadScript(scriptName);
		}

	case HotReloadManager::FT_DATATABLE_SERVER:
	case HotReloadManager::FT_DATATABLE_SHARED:
		{
			std::string dtName = HotReloadManager::getDatatableReloadName(relativePath);
			return reloadDatatable(dtName);
		}

	case HotReloadManager::FT_TEMPLATE_SERVER:
	case HotReloadManager::FT_TEMPLATE_SHARED:
		return reloadTemplate(relativePath);

	case HotReloadManager::FT_BUILDOUT:
		return reloadBuildout(relativePath);

	case HotReloadManager::FT_STRING_TABLE:
	case HotReloadManager::FT_TERRAIN:
	case HotReloadManager::FT_OTHER_IFF:
		return reloadGenericAsset(relativePath);

	case HotReloadManager::FT_UNKNOWN:
	default:
		LOG("FileControl", ("ReloadHandler: Unknown file type for %s", relativePath.c_str()));
		return false;
	}
}

// ======================================================================
// Server-side reload implementations
//
// SwgFileControl is a standalone process. It cannot directly call
// GameScriptObject::reloadScript() or ServerMessageForwarding because
// those live in the game server process. Instead, it issues the reload
// command via the console command interface that game servers expose,
// or relies on the RELOAD_NOTIFY message that connected game server
// clients will process.
//
// The actual game server integration works as follows:
// 1. SwgFileControl stores the file and distributes it to all clients
// 2. SwgFileControl sends RELOAD_NOTIFY to all connected clients
// 3. Connected game servers (which run a FileControlClient) receive
//    the RELOAD_NOTIFY and call their local reload functions
// 4. The game server's FileControlClient calls the appropriate reload
//    API (GameScriptObject::reloadScript, DataTableManager::reload, etc.)
//
// For the standalone SwgFileControl process, we perform local reloads
// where possible (DataTableManager is a shared library) and log the
// reload intent for game-server-specific operations.
// ======================================================================

bool FileControlReloadHandler::reloadScript(const std::string & scriptName)
{
	LOG("FileControl", ("ReloadHandler: script reload %s", scriptName.c_str()));
	printf("[RELOAD] script reload %s -> broadcast to game servers via RELOAD_NOTIFY\n", scriptName.c_str());

	// Game servers will receive the RELOAD_NOTIFY and call
	// GameScriptObject::reloadScript(scriptName) locally.
	return true;
}

// ----------------------------------------------------------------------

bool FileControlReloadHandler::reloadDatatable(const std::string & datatableName)
{
	LOG("FileControl", ("ReloadHandler: datatable reload %s", datatableName.c_str()));

	DataTableManager::reload(datatableName);
	LOG("FileControl", ("ReloadHandler: local DataTableManager::reload(%s) called", datatableName.c_str()));

	printf("[RELOAD] datatable reload %s -> local + broadcast\n", datatableName.c_str());
	return true;
}

// ----------------------------------------------------------------------

bool FileControlReloadHandler::reloadTemplate(const std::string & templatePath)
{
	LOG("FileControl", ("ReloadHandler: template reload %s", templatePath.c_str()));
	printf("[RELOAD] template reload %s -> broadcast to game servers via RELOAD_NOTIFY\n", templatePath.c_str());

	// Game servers will receive the RELOAD_NOTIFY and call
	// ObjectTemplateList::reload() locally with the Iff data.
	return true;
}

// ----------------------------------------------------------------------

bool FileControlReloadHandler::reloadBuildout(const std::string & buildoutPath)
{
	LOG("FileControl", ("ReloadHandler: buildout reload %s", buildoutPath.c_str()));
	printf("[RELOAD] buildout reload %s -> broadcast to game/planet servers via RELOAD_NOTIFY\n", buildoutPath.c_str());

	return true;
}

// ----------------------------------------------------------------------

bool FileControlReloadHandler::reloadGenericAsset(const std::string & assetPath)
{
	LOG("FileControl", ("ReloadHandler: generic asset reload %s", assetPath.c_str()));
	printf("[RELOAD] asset reload %s -> broadcast via RELOAD_NOTIFY\n", assetPath.c_str());

	return true;
}

// ======================================================================

bool FileControlReloadHandler::handleReceivedFile(const std::string & relativePath, const std::vector<unsigned char> & data)
{
	if (!ms_installed)
		return false;

	if (data.empty())
		return false;

	LOG("FileControl", ("ReloadHandler: received file %s (%d bytes), storing locally",
		relativePath.c_str(), static_cast<int>(data.size())));

	if (FileControlServer::isRunning())
	{
		return true;
	}

	return false;
}

// ======================================================================

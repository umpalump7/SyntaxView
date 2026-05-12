//****************************************************************************
// 
// Syntax Highlighting Plugin for Open Salamander
//
// Author: Michal Mores
//
// Email: xmoresm00@stud.fit.vut.cz
//
//****************************************************************************

#include "precomp.h"

#include "helper.h"
#include "reader.h"

#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

std::string getCurrentDllPath()
{
    HMODULE hModule = nullptr;

    // Get handle to the module where this function is located
    GetModuleHandleEx(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCSTR>(&getCurrentDllPath),
        &hModule);

    char path[MAX_PATH];
    GetModuleFileNameA(hModule, path, MAX_PATH);

    // Remove filename, keep only directory
    std::string fullPath(path);
    size_t pos = fullPath.find_last_of("\\/");
    if (pos != std::string::npos)
        fullPath = fullPath.substr(0, pos);

    return fullPath;
}


std::vector<std::string> getFilesWithExtension(const std::string& directory, const std::string& extension)
{
    std::vector<std::string> result;

    WIN32_FIND_DATA findData;
    HANDLE hFind;

    std::string searchPath = directory + "\\*";

    hFind = FindFirstFile(searchPath.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE)
        return result;

    do
    {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        std::string name = findData.cFileName;

        if (name.size() >= extension.size() &&
            name.substr(name.size() - extension.size()) == extension)
        {
            result.push_back(name);
        }

    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);

    return result;
}

std::vector<std::string> removeFileExtension(const std::vector<std::string>& fileNames)
{
	std::vector<std::string> result;
	for (const std::string& name : fileNames)
	{
		size_t pos = name.find_last_of(".");
		if (pos != std::string::npos)
		{
			result.push_back(name.substr(0, pos));
		}
		else
		{
			result.push_back(name);
		}
	}
	return result;
}

std::vector<std::string> getThemeList(std::vector<std::string>& paths)
{
	std::string module_path = getCurrentDllPath();
    std::vector<std::string> result;
    for (int i = 0; i < paths.size(); i++)
    {
        Json::Value root = parse::loadJson(module_path + "\\themes\\" + paths[i]);
        std::string name;

        
        if (!root.isArray() && root.isMember("displayName")) {
            name = root["displayName"].asString();
        }
        else if (!root.isArray() && root.isMember("name")) {
            name = root["name"].asString();
        }
        else
        {
			paths.erase(paths.begin() + i);
            --i;
            continue;
        }
		result.push_back(name);
    }
    return result;
}

void loadStringMap(const std::string& file_path, std::unordered_map<std::string, std::string>* target)
{
    if (!target)
        return;

    std::ifstream file(file_path);
    if (!file)
    {
        // failed to open
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        if (!line.empty() && line.back() == ',')
            line.pop_back();

        std::istringstream iss(line);
        std::string key, eq, value;

        if (iss >> key >> eq >> value)
        {
            if (eq == "=")
            {
                (*target)[key] = value;
            }
            else
            {
                // bad format
            }
        }
    }
}

std::string getGrammarChoice(const std::string& path)
{
    std::string module_path = getCurrentDllPath();

    std::unordered_map<std::string, std::string> extension_bindings;
    loadStringMap(module_path + "\\extension_bindings.conf", &extension_bindings);
    size_t pos = path.find_last_of(".");
    std::string file_extension = "";
    if (pos != std::string::npos)
    {
        file_extension = path.substr(pos + 1, path.size() - pos - 1);
    }

    auto it = extension_bindings.find(file_extension);
    std::string grammar_name = "";
    if (it != extension_bindings.end())
    {
        grammar_name = it->second;
    }
    return grammar_name;
}
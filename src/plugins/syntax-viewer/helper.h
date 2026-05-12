//****************************************************************************
// 
// Syntax Highlighting Plugin for Open Salamander
//
// Author: Michal Mores
//
// Email: xmoresm00@stud.fit.vut.cz
//
//****************************************************************************

#pragma once

//#include "textmodel.h"

std::vector<std::string> getFilesWithExtension(const std::string& directory, const std::string& extension);
std::vector<std::string> removeFileExtension(const std::vector<std::string>& fileNames);
std::vector<std::string> getThemeList(std::vector<std::string>& paths);
std::string getCurrentDllPath();
void loadStringMap(const std::string& file_path, std::unordered_map<std::string, std::string>* target);
std::string getGrammarChoice(const std::string& path);
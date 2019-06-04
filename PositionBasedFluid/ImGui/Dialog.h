#pragma once

#include <string>

class Dialog
{
public:
	static std::string OpenSelectedFileDialog(const char* filter);

	static std::string OpenSelectedDirDialog(const std::string& title = "");
};
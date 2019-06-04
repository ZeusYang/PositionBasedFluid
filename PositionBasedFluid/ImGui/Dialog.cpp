#include "Dialog.h"
#include <Windows.h>
#include <ShlObj.h>

std::string Dialog::OpenSelectedFileDialog(const char* filter)
{
	char file[MAX_PATH] = "";

	OPENFILENAMEA ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner	= nullptr;
	ofn.nMaxFile	= MAX_PATH;
	ofn.lpstrFile	= file;
	ofn.Flags		= OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER;
	ofn.lpstrTitle	= "Ñ¡ÔñÎÄ¼þ";
	ofn.lpstrFilter = filter;

	if ( GetOpenFileNameA(&ofn) ) {
		return file;
	}
	return "";
}

std::string Dialog::OpenSelectedDirDialog(const std::string& title)
{
	char file[MAX_PATH] = "";

	BROWSEINFOA bif = { 0 };
	bif.lpszTitle		= title.c_str();
	bif.pszDisplayName	= file;
	bif.ulFlags			= BIF_BROWSEINCLUDEFILES;

	if ( LPITEMIDLIST pil = SHBrowseForFolderA(&bif) ) {
		SHGetPathFromIDListA(pil, file);
		return file;
	}
	return "";
}
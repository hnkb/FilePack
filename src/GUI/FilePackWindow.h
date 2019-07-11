
#pragma once

#include "Crib/WindowsBase.h"
#include "../FilePack.h"
#include "DataFormatter.h"
#include "EmbeddedBrowser.h"


class FilePackWindow : public Crib::Window
{
public:
	FilePackWindow();
	~FilePackWindow();

protected:
	LRESULT proc(const UINT message, const WPARAM wParam, const LPARAM lParam) override;

private:
	void fillTree();
	void fillList();
	void selectBlock(std::string name);


	std::wstring m_filename;
	std::unique_ptr<FilePack::Reader> m_reader;

	std::unique_ptr<FilePack::Reader::Block<uint8_t>> m_blockData;
	std::unique_ptr<DataFormatterBase> m_format;

	std::unique_ptr<CWebBrowser> m_browser;


	// Controls

	HWND hwTreeView;
	HWND hwListView;

	const int widthTreeView = 150;
	const int controlMargin = 8;
};

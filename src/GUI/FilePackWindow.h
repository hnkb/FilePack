
#pragma once

#include "Crib/WindowsBase.h"
#include "../FilePack.h"


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


	std::wstring m_filename;
	std::unique_ptr<FilePack::Reader> m_reader;


	// Controls

	HWND hwTreeView;
	HWND hwTextView;
	HWND hwListView;

	const int idTreeView = 1000;
	const int idTextView = 1001;
	const int idListView = 1002;

	const int widthTreeView = 150;
	const int controlMargin = 8;
};

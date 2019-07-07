
#pragma once

#include "Crib/WindowsBase.h"


class FilePackWindow : public Crib::Window
{
public:
	FilePackWindow();
	~FilePackWindow();

protected:
	LRESULT proc(const UINT message, const WPARAM wParam, const LPARAM lParam) override;

private:
	void fillTree();

	// Controls

	HWND hwTreeView;

	const int idTreeView = 1000;

	const int widthTreeView = 150;
};

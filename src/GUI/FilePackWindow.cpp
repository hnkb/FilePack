
#include "FilePackWindow.h"
#include <CommCtrl.h>

#include <strsafe.h>

FilePackWindow::FilePackWindow() :
	Crib::Window(),
	hwTreeView(nullptr)
{
	setTitle(L"FilePack Explorer");

	HINSTANCE hInst = GetModuleHandleW(nullptr);
	RECT rc;
	GetClientRect(m_handle, &rc);

	hwTreeView = CreateWindowExW(0, WC_TREEVIEWW, L"Tree View", WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES,
		0, 0, widthTreeView, rc.bottom, m_handle, (HMENU)idTreeView, hInst, nullptr);


	fillTree();
}

FilePackWindow::~FilePackWindow()
{
	DestroyWindow(hwTreeView);
}


void FilePackWindow::fillTree()
{
	WCHAR title[] = L"root item!";

	TVITEMW tvi;
	tvi.mask = TVIF_TEXT | TVIF_STATE;// | TVIF_PARAM;
	tvi.pszText = title;
	tvi.cchTextMax = sizeof(title) / sizeof(title[0]);
	tvi.state = TVIS_EXPANDED;
	tvi.stateMask = TVIS_EXPANDED;
	//tvi.lParam = (LPARAM)nLevel;

	TVINSERTSTRUCTW tvins;
	tvins.item = tvi;
	tvins.hParent = TVI_ROOT;
	tvins.hInsertAfter = TVI_FIRST;

	tvins.hParent = (HTREEITEM)SendMessageW(hwTreeView, TVM_INSERTITEMW, 0, (LPARAM)& tvins);
	tvins.hInsertAfter = TVI_LAST;

	for (int i = 0; i < 10; i++)
	{
		StringCchPrintfW(title, 10, L"Item %d", i);
		SendMessageW(hwTreeView, TVM_INSERTITEMW, 0, (LPARAM)& tvins);
	}
}


LRESULT FilePackWindow::proc(const UINT message, const WPARAM wParam, const LPARAM lParam)
{
	if (message == WM_SIZE)
	{
		RECT rc;
		GetClientRect(m_handle, &rc);
		MoveWindow(hwTreeView, 0, 0, widthTreeView, rc.bottom, TRUE);
	}

	if (message == WM_KEYDOWN && wParam == VK_ESCAPE)
	{
		DestroyWindow(m_handle);
		return 0;
	}

	return Window::proc(message, wParam, lParam);
}

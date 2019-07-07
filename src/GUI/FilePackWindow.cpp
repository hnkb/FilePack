
#include "FilePackWindow.h"

#include <CommCtrl.h>
#include <strsafe.h>


FilePackWindow::FilePackWindow() :
	Crib::Window(),
	hwTreeView(nullptr)
{
	setTitle(L"FilePack Explorer");

	auto hInst = (HINSTANCE)GetWindowLongPtrW(m_handle, GWLP_HINSTANCE);

	// Using some invalid size here, because soon a WM_SIZE will be sent to controls and they are resized in proc()

	hwTreeView = CreateWindowExW(0, WC_TREEVIEWW, L"Tree View",
		WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES,
		0, 0, 0, 0, m_handle, (HMENU)idTreeView, hInst, nullptr);

	hwTextView = CreateWindowExW(0, L"EDIT", nullptr,
		WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL | ES_LEFT | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
		0, 0, 0, 0, m_handle, (HMENU)idTextView, hInst, nullptr);

	hwListView = CreateWindowExW(0, WC_LISTVIEWW, L"List View",
		WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_OWNERDATA,
		0, 0, 0, 0, m_handle, (HMENU)idListView, hInst, nullptr);


	fillTree();
	fillList();
}

FilePackWindow::~FilePackWindow()
{
	DestroyWindow(hwTreeView);
	DestroyWindow(hwTextView);
	DestroyWindow(hwListView);
}


void FilePackWindow::fillTree()
{
	TreeView_DeleteAllItems(hwTreeView);


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

	tvins.hParent = TreeView_InsertItem(hwTreeView, &tvins);
	tvins.hInsertAfter = TVI_LAST;

	for (int i = 0; i < 10; i++)
	{
		StringCchPrintfW(title, 10, L"Item %d", i);
		TreeView_InsertItem(hwTreeView, &tvins);
	}
}

void FilePackWindow::fillList()
{
	ListView_DeleteAllItems(hwListView);
	while (ListView_DeleteColumn(hwListView, 0));


	WCHAR text[256];

	LVCOLUMNW lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.pszText = text;

	for (int i = 0; i < 5; i++)
	{
		StringCchPrintfW(text, 256, L"Column %d", i);
		lvc.iSubItem = i;
		lvc.cx = 100;
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hwListView, i, &lvc);
	}


	LVITEMW lvi;
	lvi.mask = LVIF_TEXT | LVIF_STATE;
	lvi.pszText = LPSTR_TEXTCALLBACKW;
	lvi.state = lvi.stateMask = 0;
	lvi.iSubItem = 0;

	for (int i = 0; i < 20; i++)
	{
		lvi.iItem = i;
		ListView_InsertItem(hwListView, &lvi);
	}
}


LRESULT FilePackWindow::proc(const UINT message, const WPARAM wParam, const LPARAM lParam)
{
	if (message == WM_SIZE)
	{
		const int width = LOWORD(lParam);
		const int height = HIWORD(lParam);
		MoveWindow(hwTreeView, 0, 0, widthTreeView, width, TRUE);
		MoveWindow(hwTextView, widthTreeView + controlMargin, 0, width - widthTreeView - controlMargin, height / 2 - controlMargin, TRUE);
		MoveWindow(hwListView, widthTreeView + controlMargin, height / 2, width - widthTreeView - controlMargin, height / 2, TRUE);
	}

	if (message == WM_NOTIFY)
	{
		auto hdr = (NMHDR*)lParam;
		
		if (hdr->hwndFrom == hwListView && hdr->code == LVN_GETDISPINFO)
		{
			auto dispInfo = (NMLVDISPINFO*)lParam;
			if (dispInfo->item.mask & LVIF_TEXT)
			{
				StringCchPrintf(dispInfo->item.pszText, dispInfo->item.cchTextMax, L"It %d Col %d", dispInfo->item.iItem, dispInfo->item.iSubItem);
			}
		}
	}

	if (message == WM_KEYDOWN && wParam == VK_ESCAPE)
	{
		DestroyWindow(m_handle);
		return 0;
	}

	return Window::proc(message, wParam, lParam);
}

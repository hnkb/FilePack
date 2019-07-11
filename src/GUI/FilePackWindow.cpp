
#include "FilePackWindow.h"

#include <atlcomcli.h>
#include <CommCtrl.h>
#include <strsafe.h>
#include <Shlwapi.h>

#pragma comment(lib, "shlwapi.lib")


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

	hwListView = CreateWindowExW(0, WC_LISTVIEWW, L"List View",
		/*WS_VISIBLE |*/ WS_CHILD | LVS_REPORT | LVS_OWNERDATA,
		0, 0, 0, 0, m_handle, (HMENU)idListView, hInst, nullptr);
	ListView_SetExtendedListViewStyleEx(hwListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_browser.reset(new CWebBrowser(m_handle, nullptr));
	if (m_browser)
		m_browser->DisplayString(CComBSTR(L""));


	m_filename = L"C:\\Users\\hani\\OneDrive\\Desktop\\test-2.pcn";
	m_reader.reset(new FilePack::Reader(m_filename));
	fillTree();
	fillList();
}

FilePackWindow::~FilePackWindow()
{
	DestroyWindow(hwTreeView);
	DestroyWindow(hwListView);
}


void FilePackWindow::fillTree()
{
	TreeView_DeleteAllItems(hwTreeView);
	if (!m_reader)
		return;


	const int textSize = 512;
	WCHAR text[textSize];

	TVITEMW tvi;
	tvi.mask = TVIF_TEXT | TVIF_STATE;// | TVIF_PARAM;
	tvi.pszText = text;
	tvi.cchTextMax = textSize;
	tvi.state = TVIS_EXPANDED;
	tvi.stateMask = TVIS_EXPANDED;
	//tvi.lParam = (LPARAM)nLevel;

	TVINSERTSTRUCTW tvins;
	tvins.item = tvi;
	tvins.hParent = TVI_ROOT;
	tvins.hInsertAfter = TVI_FIRST;

	StringCchPrintfW(text, textSize, L"%s (%s)",
		PathFindFileNameW(m_filename.c_str()),
		Crib::fromLatin1(m_reader->signature()).c_str());
	tvins.hParent = TreeView_InsertItem(hwTreeView, &tvins);
	tvins.hInsertAfter = TVI_LAST;

	for (auto& block : m_reader->blocks())
	{
		StringCchCopyW(text, textSize, Crib::fromLatin1(block.first).c_str());
		TreeView_InsertItem(hwTreeView, &tvins);
	}
}

void FilePackWindow::fillList()
{
	while (ListView_DeleteColumn(hwListView, 0));

	if (!m_format || m_format->columnCount() == 0)
		return;


	WCHAR text[256];

	LVCOLUMNW lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.pszText = text;

	StringCchCopy(text, 256, L"Index");
	lvc.iSubItem = 0;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
	ListView_InsertColumn(hwListView, lvc.iSubItem, &lvc);

	for (int i = 0; i < m_format->columnCount(); i++)
	{
		StringCchCopy(text, 256, m_format->columnLabel(i).c_str());
		lvc.iSubItem = i + 1;
		lvc.cx = 100;
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hwListView, lvc.iSubItem, &lvc);
	}


	ListView_SetItemCountEx(hwListView, m_format->rowCount, LVSICF_NOSCROLL);
}

void FilePackWindow::selectBlock(std::string name)
{
	if (m_reader && m_reader->blocks().find(name) != m_reader->blocks().end())
	{
		std::map<std::string, std::string> formatDesc = {
			{ "pt2d.flags", "bFlag" },
			{ "pt2d.sf", "fFactorStdDev" },
			{ "pt2d.sxy", "fStdDevX,fStdDevY" },
			{ "pt2d.x", "fx" },
			{ "pt2d.y", "fy" },
			{ "pt3d.layer", "iLayer" },
			{ "pt3d.mix2", "dX;dY;dZ;dStdDevX;dStdDevY;dStdDevZ" },
		};

		if (name == "js")
		{
			ShowWindow(hwListView, SW_HIDE);

			if (m_browser)
			{
				auto data = m_reader->get<char>(name);
				std::wstring txt =
					L"<!DOCTYPE html>\n<html>\n<head><meta charset=\"utf-8\" /><title>Code Viewer</title>"
					"<style type=\"text/css\">#editor { position: absolute; top: 0; right: 0; bottom: 0; left: 0; }</style>"
					"</head><body><div id=\"editor\">"
					+
					Crib::fromUtf8(std::string(data.begin(), data.end()))
					+
					L"</div><script src=\"C:\\Users\\hani\\OneDrive\\Desktop\\ace\\ace.js\" type=\"text/javascript\" charset=\"utf-8\"></script>"
					"<script>"
					"var editor = ace.edit('editor');"
					"editor.setTheme('ace/theme/dawn');"
					"editor.session.setMode('ace/mode/json');"
					"</script></body></html>";

				CComBSTR bstr(txt.size(), txt.c_str());
				m_browser->DisplayString(bstr);
			}

			return;
		}

		if (formatDesc.find(name) != formatDesc.end())
		{
			m_blockData.reset(new FilePack::Reader::Block<uint8_t>(std::move(m_reader->get<uint8_t>(name))));
			m_format.reset(new DataFormatter(formatDesc.at(name), m_blockData->size()));
			fillList();
			ShowWindow(hwListView, SW_SHOW);
			return;
		}
	}

	m_format.reset(nullptr);
	if (m_browser)
		m_browser->DisplayString(CComBSTR(L""));
	ShowWindow(hwListView, SW_HIDE);
}


LRESULT FilePackWindow::proc(const UINT message, const WPARAM wParam, const LPARAM lParam)
{
	if (message == WM_SIZE)
	{
		const int width = LOWORD(lParam);
		const int height = HIWORD(lParam);
		MoveWindow(hwTreeView, 0, 0, widthTreeView, width, TRUE);
		MoveWindow(hwListView, widthTreeView + controlMargin, 0, width - widthTreeView - controlMargin, height, TRUE);
		
		RECT rc{ widthTreeView + controlMargin, 0, width, height };
		m_browser->Resize(&rc);
	}

	if (message == WM_NOTIFY)
	{
		auto hdr = (NMHDR*)lParam;
		
		if (hdr->hwndFrom == hwListView && hdr->code == LVN_GETDISPINFO)
		{
			auto dispInfo = (NMLVDISPINFO*)lParam;
			if (dispInfo->item.mask & LVIF_TEXT)
			{
				if (m_format && m_blockData)
					m_format->get(dispInfo->item.pszText, dispInfo->item.cchTextMax, m_blockData->data(), dispInfo->item.iItem, dispInfo->item.iSubItem);
				else if (dispInfo->item.iSubItem == 0)
					StringCchPrintf(dispInfo->item.pszText, dispInfo->item.cchTextMax, L"%d", dispInfo->item.iItem);
				else
					dispInfo->item.pszText[0] = 0;
			}
			return TRUE;
		}

		if (hdr->hwndFrom == hwTreeView && hdr->code == TVN_SELCHANGED)
		{
			auto tv = (NMTREEVIEW*)lParam;

			WCHAR text[512];
			TVITEM tvi;
			tvi.hItem = tv->itemNew.hItem;
			tvi.mask = TVIF_TEXT;
			tvi.pszText = text;
			tvi.cchTextMax = 512;
			TreeView_GetItem(hwTreeView, &tvi);
			selectBlock(Crib::toLatin1(text));
			return 0;
		}
	}

	if (message == WM_KEYDOWN && wParam == VK_ESCAPE)
	{
		DestroyWindow(m_handle);
		return 0;
	}

	if (message == WM_DESTROY)
	{
		// For some reason, when browser window works, some child windows linger
		// and we never quit. This only works if I am main window.
		PostQuitMessage(0);
	}

	return Window::proc(message, wParam, lParam);
}

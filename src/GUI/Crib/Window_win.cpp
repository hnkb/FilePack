
#include "WindowsBase.h"

#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


Crib::Window::Window(const Options& options) :
	m_handle(nullptr)
{
	const std::wstring title = L"Crib";
	const std::wstring className = L"CribGraphicsTestWindowCls";
	const DWORD style = WS_OVERLAPPEDWINDOW;
	const WORD menu = 0;
	const WORD icon = 0;
	const int x = 350;// CW_USEDEFAULT;
	const int y = 500;// 0;
	const int width = 800;// CW_USEDEFAULT;
	const int height = 600;// 0;




	InitCommonControls();

	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.hInstance = GetModuleHandleW(nullptr);
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.lpszClassName = className.c_str();
	wcex.lpfnWndProc = proc;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	//wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(menu);
	wcex.hIcon = LoadIconW(GetModuleHandleW(nullptr), MAKEINTRESOURCEW(icon));
	RegisterClassExW(&wcex);

	RECT rect{ 0, 0, width, height };
	if (rect.right != CW_USEDEFAULT) AdjustWindowRect(&rect, style, menu != 0);

	m_handle = CreateWindowExW(WS_EX_DLGMODALFRAME, wcex.lpszClassName, title.c_str(), style,
		x, y, rect.right - rect.left, rect.bottom - rect.top,
		nullptr, nullptr, wcex.hInstance, nullptr);
	if (!m_handle) throw Error("CreateWindow");

	SetWindowLongPtrW(m_handle, GWLP_USERDATA, LONG_PTR(this));

	ShowWindow(m_handle, SW_SHOWDEFAULT);

	GetClientRect(m_handle, &rect);
	PostMessageW(m_handle, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rect.right - rect.left, rect.bottom - rect.top));
}

Crib::Window::~Window()
{
	if (m_handle)
		DestroyWindow(m_handle);
}


LRESULT Crib::Window::proc(const UINT message, const WPARAM wParam, const LPARAM lParam)
{
	switch (message)
	{
	//case WM_ERASEBKGND:
	//	return TRUE;
	//case WM_PAINT:
	//	if (m_gfxContext)
	//	{
	//		m_gfxContext->draw();
	//		//const auto title = L"Crib  [" + m_gfxContext->description + L"]";
	//		//	;// +L" " + std::to_wstring((int)std::round(1. / m_gfxContext->lastFrameTime)) + L" fps";
	//		//SetWindowTextW(m_handle, title.c_str());
	//	}
	//	return 0;
	//case WM_SIZE:
	//	if (m_gfxContext)
	//		m_gfxContext->resize();
	//	return 0;
	case WM_DESTROY:
		m_handle = nullptr;
		PostMessageW(nullptr, Application::Message::WindowClosed, 0, 0);
		return 0;
	}

	return DefWindowProcW(m_handle, message, wParam, lParam);
}

LRESULT CALLBACK Crib::Window::proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	try
	{
		auto wnd = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
		return (wnd && wnd->m_handle == hWnd) ?
			wnd->proc(message, wParam, lParam) :
			DefWindowProcW(hWnd, message, wParam, lParam);
	}
	catch (std::exception& ex)
	{
		MessageBoxA(0, ex.what(), 0, MB_ICONERROR);
	}
}

Crib::Window& Crib::Window::setTitle(const std::wstring& title)
{
	SetWindowTextW(m_handle, title.c_str());
	return *this;
}

std::wstring Crib::Window::getTitle() const
{
	std::wstring title(GetWindowTextLengthW(m_handle), L'\0');
	GetWindowTextW(m_handle, &title[0], static_cast<int>(title.size() + 1));
	return title;
}

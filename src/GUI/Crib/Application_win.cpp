
#include "WindowsBase.h"


int Crib::Application::messageLoop()
{
	MSG msg;

	if (EnumThreadWindows(GetCurrentThreadId(), [](HWND, LPARAM) { return FALSE; }, 0))
		throw std::logic_error("[Crib::Application::messageLoop] At least one window is required.");

	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);

		if ((msg.message == Message::Quit) ||
			(msg.message == Message::WindowClosed &&
				EnumThreadWindows(GetCurrentThreadId(), [](HWND, LPARAM) { return FALSE; }, 0)))
			PostQuitMessage(0);
	}

	return (int)msg.wParam;
}

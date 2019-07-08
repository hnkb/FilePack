
#pragma once

#include <stdexcept>
#include <string>
#include <memory>

#ifndef UNICODE
#define UNICODE
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

//#include <WinSDKVer.h>
//#define _WIN32_WINNT 0x0501 //WINXP
//#define _WIN32_WINNT_WIN7 0x0601
//#define _WIN32_WINNT_WIN8 0x0602
//#define _WIN32_WINNT _WIN32_WINNT_WINXP
//#include <SDKDDKVer.h>

#include <Windows.h>


namespace Crib
{
	class Error : public std::runtime_error
	{
	public:
		Error();
		Error(const char* operationName);
		Error(const HRESULT hr)
			: Error(hr, "Operation") {}
		Error(const HRESULT hr, const char* operationName)
			: runtime_error(getMessageText(hr, operationName)) {}

	private:
		std::string getMessageText(const HRESULT hr, const char* operationName);
	};

	// there is a DX::ThrowIfFailed
	void ThrowOnFail(const HRESULT hr, const char* operationName);
	void ThrowOnFail(const HRESULT hr);


	class InitializeCOM
	{
	public:
		InitializeCOM();
		~InitializeCOM();

	private:
		bool needUninitialize;
	};


	namespace Application
	{
		namespace Message
		{
			constexpr UINT WindowClosed = (WM_APP + 1001);
			constexpr UINT Quit =         (WM_APP + 1002);
		}

		int messageLoop();
	}


	class Window
	{
	public:
		class Options
		{
		public:
			std::wstring typeId;
			std::wstring title;
			// settings
			// point top
			// rect size
			// Platform-specifi: menu, icon, style
		};

		Window() : Window(Options()) {};
		Window(const Options&);

		virtual ~Window();

		Window(const Window& other) = delete;
		Window(Window&& other) = delete;
		Window& operator=(const Window& other) = delete;
		Window& operator=(Window&& other) = delete;

		Window& setTitle(const std::wstring& title);
		std::wstring getTitle() const;

	protected:
		virtual LRESULT proc(const UINT message, const WPARAM wParam, const LPARAM lParam);
		static LRESULT CALLBACK proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		HWND m_handle;
	};


	std::string toUtf8(const std::wstring& src);
	std::string toLatin1(const std::wstring& src);
	std::string toUserCodePage(const std::wstring& src);

	std::wstring fromUtf8(const std::string& src);
	std::wstring fromLatin1(const std::string& src);
	std::wstring fromUserCodePage(const std::string& src);
}


#include "WindowsBase.h"


namespace
{
	std::string toANSI(const std::wstring& src, UINT codePage)
	{
		int bufferSize = WideCharToMultiByte(codePage, 0, src.c_str(), static_cast<int>(src.size()), nullptr, 0, nullptr, nullptr);
		if (bufferSize == 0 && src.size() != 0)
			throw Crib::Error("WideCharToMultiByte");
		std::string output(bufferSize, 0);
		WideCharToMultiByte(codePage, 0, src.c_str(), static_cast<int>(src.size()), &output[0], static_cast<int>(output.size()), nullptr, nullptr);
		return output;
	}

	std::wstring fromANSI(const std::string& src, UINT codePage)
	{
		int bufferSize = MultiByteToWideChar(codePage, 0, src.c_str(), static_cast<int>(src.size()), nullptr, 0);
		if (bufferSize == 0 && src.size() != 0)
			throw Crib::Error("MultiByteToWideChar");
		std::wstring output(bufferSize, 0);
		MultiByteToWideChar(codePage, 0, src.c_str(), static_cast<int>(src.size()), &output[0], static_cast<int>(output.size()));
		return output;
	}
}

std::string Crib::toUtf8(const std::wstring& src) { return toANSI(src, CP_UTF8); }
std::string Crib::toLatin1(const std::wstring& src) { return toANSI(src, 1252); }
std::string Crib::toUserCodePage(const std::wstring& src) { return toANSI(src, CP_ACP); }

std::wstring Crib::fromUtf8(const std::string& src) { return fromANSI(src, CP_UTF8); }
std::wstring Crib::fromLatin1(const std::string& src) { return fromANSI(src, 1252); }
std::wstring Crib::fromUserCodePage(const std::string& src) { return fromANSI(src, CP_ACP); }

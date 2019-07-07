
#include "WindowsBase.h"


Crib::Error::Error()
	: Error(GetLastError())
{

}

Crib::Error::Error(const char* operationName)
	: Error(GetLastError(), operationName)
{

}

std::string Crib::Error::getMessageText(const HRESULT hr, const char* operationName)
{
	std::string retval = std::string(operationName) + " failed.";
	char* msg;

	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)& msg, 0, nullptr);

	if (msg)
	{
		retval = retval + "\n" + msg;
		LocalFree(msg);
	}
	else
	{
		retval += " Unknown error " + std::to_string(hr) + ".";
	}

	return retval;
}


void Crib::ThrowOnFail(const HRESULT hr, const char* operationName)
{
	if (FAILED(hr))
		throw Error(hr, operationName);
}

void Crib::ThrowOnFail(const HRESULT hr)
{
	if (FAILED(hr))
		throw Error(hr);
}


#include "DataFormatter.h"
#include "Crib/WindowsBase.h"
#include <regex>
#include <map>
#include <strsafe.h>


namespace
{
	std::map<char, int> typeSize = {
		{ 'b', 1 },
		{ 'd', 8 },
		{ 'i', 4 },
		{ 'f', 4 },
	};
}


DataFormatter::DataFormatter(const std::string formatDesc) :
	rowSize(0)
{
	std::regex rgx(",");

	for (std::sregex_token_iterator i(formatDesc.begin(), formatDesc.end(), rgx, { -1 });
		i != std::sregex_token_iterator(); i++)
	{
		columns.emplace_back();
		auto& c = columns.back();

		c.type = i->str().at(0);
		c.name = Crib::fromLatin1(i->str().substr(1));
		c.offset = rowSize;

		rowSize += typeSize.at(c.type);
	}
}

void DataFormatter::get(wchar_t* output, const size_t outputSize, const uint8_t* data, const size_t row, const int column)
{
	auto ptr = data + row * rowSize + columns[column].offset;

	switch (columns[column].type)
	{
	case 'b':
		StringCchPrintfW(output, outputSize, L"%d", (int)(*ptr));
		break;

	case 'i':
		StringCchPrintfW(output, outputSize, L"%d", *(int*)ptr);
		break;

	case 'f':
		StringCchPrintfW(output, outputSize, L"%g", *(float*)ptr);
		break;

	case 'd':
		StringCchPrintfW(output, outputSize, L"%g", *(float*)ptr);
		break;

	default:
		StringCchPrintfW(output, outputSize, L"-");
		break;
	}
}

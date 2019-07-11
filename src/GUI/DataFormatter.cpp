
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


DataFormatter::DataFormatter(std::string formatDesc, const size_t numberOfBytesInDataset)
{
	std::regex rx("\\s*(\\d*)\\s*(\\S)\\s*(?:\\\"([^\\\"]+)\\\"|([^,;]+))\\s*([,;])");

	formatDesc += ","; // to be able to match and capture last column because I could not write an elegant regex

	int groupId = 0;
	size_t fullRowSize = 0;
	std::map<int, size_t> groupSize;
	std::map<int, size_t> groupOffset = { {0,0} };

	for (std::sregex_token_iterator i(formatDesc.begin(), formatDesc.end(), rx, { 1, 2, 3, 4, 5 });
		i != std::sregex_token_iterator();
		i++)
	{
		columns.emplace_back();
		auto& c = columns.back();

		c.count = i->length() ? std::stoi(i->str()) : 1;
		c.type = (++i)->str().front();
		c.label = Crib::fromLatin1((++i)->str() + (++i)->str());
		c.groupId = groupId;
		c.offset = groupSize[c.groupId];

		const auto colSize = typeSize.at(c.type) * c.count;
		fullRowSize += colSize;
		groupSize[groupId] += colSize;

		if ((++i)->str() == ";")
		{
			groupId++;
			groupOffset[groupId] = fullRowSize;
		}
	}


	if (numberOfBytesInDataset % fullRowSize)
		MessageBox(0, L"Warning: data size is not a multiple of row size!\n", 0, MB_ICONWARNING);

	rowCount = numberOfBytesInDataset / fullRowSize;

	for (auto& c : columns)
	{
		c.pitch = groupSize.at(c.groupId);
		c.offset += groupOffset.at(c.groupId) * rowCount;
	}
}

void DataFormatter::get(wchar_t* output, const size_t outputSize, const uint8_t* data, const size_t row, const int column)
{
	if (column == 0)
	{
		StringCchPrintfW(output, outputSize, L"%lld", row);
	}
	else
	{

		const auto& c = columns.at(column - 1);
		auto ptr = data + row * c.pitch + c.offset;

		switch (c.type)
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
			StringCchPrintfW(output, outputSize, L"%g", *(double*)ptr);
			break;

		default:
			StringCchPrintfW(output, outputSize, L"-");
			break;
		}
	}
}

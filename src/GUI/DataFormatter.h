
#pragma once

#include <string>
#include <vector>


class DataFormatter
{
public:
	struct Column
	{
		std::wstring name;
		char type;
		int offset;
	};


	DataFormatter(const std::string formatDesc);
	void get(wchar_t* output, const size_t outputSize, const uint8_t* data, const size_t row, const int column);

	std::vector<Column> columns;
	int rowSize;
};

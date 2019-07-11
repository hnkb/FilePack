
#pragma once

#include <string>
#include <vector>


class DataFormatter
{
public:
	struct Column
	{
		std::wstring label;
		int groupId;
		char type;
		size_t count;
		size_t offset;
		size_t pitch;
	};


	DataFormatter(std::string formatDesc, const size_t numberOfBytesInDataset);
	void get(wchar_t* output, const size_t outputSize, const uint8_t* data, const size_t row, const int column);

	std::vector<Column> columns;
	int rowCount;
};

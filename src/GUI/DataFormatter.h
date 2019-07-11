
#pragma once

#include <string>
#include <vector>


class DataFormatterBase
{
public:
	virtual ~DataFormatterBase() {}

	virtual void get(wchar_t* output, const size_t outputSize, const uint8_t* data, const size_t row, const int column) = 0;

	virtual size_t columnCount() = 0;
	virtual std::wstring columnLabel(int i) = 0;

	size_t rowCount = 0;
};


class DataFormatter : public DataFormatterBase
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
	void get(wchar_t* output, const size_t outputSize, const uint8_t* data, const size_t row, const int column) override;


	size_t columnCount() override { return columns.size(); }
	std::wstring columnLabel(int i) override { return columns[i].label; }

	std::vector<Column> columns;
};


class StringListFormatter :public DataFormatterBase
{
public:
	StringListFormatter(const uint8_t* data, const size_t numberOfBytesInDataset);
	void get(wchar_t* output, const size_t outputSize, const uint8_t* data, const size_t row, const int column) override;


	size_t columnCount() override { return 2; }
	std::wstring columnLabel(int i) override { return i == 0 ? L"Offset" : L"Text"; }

	std::vector<wchar_t*> offsets;
};

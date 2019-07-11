
#pragma once

#include <string>
#include <vector>


class DataFormatterBase
{
public:
	virtual ~DataFormatterBase() {}

	virtual void get(wchar_t* output, size_t outputSize, const uint8_t* data, const size_t row, const int column) = 0;

	virtual size_t columnCount() = 0;
	virtual std::wstring columnLabel(int i) = 0;
	virtual int columnWidth(int i) = 0;

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
	void get(wchar_t* output, size_t outputSize, const uint8_t* data, const size_t row, const int column) override;


	size_t columnCount() override { return columns.size(); }
	std::wstring columnLabel(int i) override { return columns[i].label; }
	int columnWidth(int i) override { return 60 + (int)columns[i].count * 40; }

	std::vector<Column> columns;
};


class StringListFormatter :public DataFormatterBase
{
public:
	StringListFormatter(const uint8_t* data, const size_t numberOfBytesInDataset);
	void get(wchar_t* output, size_t outputSize, const uint8_t* data, const size_t row, const int column) override;


	size_t columnCount() override { return 2; }
	std::wstring columnLabel(int i) override { return i == 0 ? L"Offset" : L"Text"; }
	int columnWidth(int i) override { return i == 1 ? 200 : 100; }

	std::vector<wchar_t*> offsets;
};

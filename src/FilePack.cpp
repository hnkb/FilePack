
#include "FilePack.h"

#include <stdexcept>
#include <vector>
#define NOMINMAX
#include <Windows.h>

#include <regex>
#include "Compression.h"

#define CompressHeader 0


namespace FilePack
{
	namespace Internal
	{
		std::string toANSI(const std::wstring& src, const UINT codePage)
		{
			int bufferSize = WideCharToMultiByte(codePage, 0, src.c_str(), static_cast<int>(src.size()), nullptr, 0, nullptr, nullptr);
			if (bufferSize == 0 && src.size() != 0)
				throw std::runtime_error("WideCharToMultiByte failed");
			std::string output(bufferSize, 0);
			WideCharToMultiByte(codePage, 0, src.c_str(), static_cast<int>(src.size()), &output[0], static_cast<int>(output.size()), nullptr, nullptr);
			return output;
		}
		std::string toUTF8(const std::wstring & src) { return toANSI(src, CP_UTF8); }
		std::string toLatin1(const std::wstring & src) { return toANSI(src, 1252); }
		std::string toUserCodePage(const std::wstring & src) { return toANSI(src, CP_ACP); }
		std::wstring fromANSI(const std::string & src, UINT codePage)
		{
			int bufferSize = MultiByteToWideChar(codePage, 0, src.c_str(), static_cast<int>(src.size()), nullptr, 0);
			if (bufferSize == 0 && src.size() != 0)
				throw std::runtime_error("WideCharToMultiByte failed");
			std::wstring output(bufferSize, 0);
			MultiByteToWideChar(codePage, 0, src.c_str(), static_cast<int>(src.size()), &output[0], static_cast<int>(output.size()));
			return output;
		}
		std::wstring fromUTF8(const std::string & src) { return fromANSI(src, CP_UTF8); }
		std::wstring fromLatin1(const std::string & src) { return fromANSI(src, 1252); }
		std::wstring fromUserCodePage(const std::string & src) { return fromANSI(src, CP_ACP); }


		class WriterImpl
		{
		public:
			WriterImpl(const std::wstring& name, const char signature[8]);
			~WriterImpl();
			void add(const std::string& blockName, const uint8_t* buffer, const size_t size, const int compression);

		private:
			HANDLE m_file;
			std::vector<std::pair<std::string, BlockInfo>> m_blocks;
		};

		class ReaderImpl
		{
		public:
			ReaderImpl(const std::wstring& name, std::map<std::string, BlockInfo>& blocks);
			~ReaderImpl();

			const std::string signatrue() const;

		private:
			HANDLE m_file, m_fileMap;
			uint8_t* m_view;
			size_t m_size;
		};
	}
}



FilePack::Writer::Writer(const std::wstring& name, const char signature[8])
{
	pimpl = (void*)new Internal::WriterImpl(name, signature);
}

FilePack::Writer::Writer(const std::string& name, const char signature[8])
	: Writer(Internal::fromUTF8(name), signature) {}

FilePack::Writer::~Writer()
{
	delete static_cast<Internal::WriterImpl*>(pimpl);
}

void FilePack::Writer::addRaw(const std::string& blockName, const uint8_t* buffer, const size_t size, const int compression)
{
	static_cast<Internal::WriterImpl*>(pimpl)->add(blockName, buffer, size, compression);
}


FilePack::Internal::WriterImpl::WriterImpl(const std::wstring& name, const char signature[8])
{
	//if (existingFileBehavior == Existing::Fail && File::exists(filename))
	//	throw std::runtime_error("File::MemoryMapped - file already exists");
	//// make sure path exists so we can actually create file
	//Directory::create(Path::getDirectoryName(filename));

	m_file = CreateFileW(name.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, 0, nullptr);
	if (m_file == INVALID_HANDLE_VALUE)
		throw std::runtime_error("CreateFileW failed.");

	DWORD bytes;
	WriteFile(m_file, signature, 8, &bytes, nullptr);
	if (bytes != 8)
		throw std::runtime_error("WriteFile failed.");

	uint64_t address = -1;
	WriteFile(m_file, &address, 8, &bytes, nullptr);
	if (bytes != 8)
		throw std::runtime_error("WriteFile failed.");

	SetEndOfFile(m_file);
}

FilePack::Internal::WriterImpl::~WriterImpl()
{
	try
	{
		std::vector<char> header;
		size_t headerOffset = 16; // 8 bytes signature, 1 byte version, 7 bytes header offset
		header.reserve(m_blocks.size() * 32);

		for (auto& block : m_blocks)
		{
			if (header.size())
				header.push_back(0);
			const auto blockHeader = std::to_string(block.second.compression) + ";"
				+ std::to_string(block.second.size) + ";" + block.first;
			std::copy(blockHeader.begin(), blockHeader.end(), std::back_inserter(header));
			headerOffset += block.second.size;
		}


		DWORD bytes;
#if CompressHeader
		const auto compressedHeader = compressZSTD((uint8_t*)header.data(), header.size());
		WriteFile(m_file, compressedHeader.data(), compressedHeader.size(), &bytes, nullptr);
		if (bytes != compressedHeader.size())
#else
		WriteFile(m_file, header.data(), header.size(), &bytes, nullptr);
		if (bytes != header.size())
#endif
			throw std::runtime_error("WriteFile failed.");
		printf("%d bytes written in header.\n", bytes);

		SetFilePointer(m_file, 8, 0, FILE_BEGIN);
		const uint64_t headerData = (headerOffset << 8) | (0x01ull); // 1 byte version, 7 bytes header offset
		WriteFile(m_file, &headerData, 8, &bytes, nullptr);
		if (bytes != 8)
			throw std::runtime_error("WriteFile failed.");

		CloseHandle(m_file);



		//std::string blockNames;
		//std::vector<BlockInfo> blocks;
		//for (auto& block : m_blocks) {
		//	blockNames += block.first + '\0';
		//	blocks.push_back(block.second);
		//	printf("- block \"%s\" %lld %lld\n", block.first.c_str(), block.second.ptr, block.second.size);
		//}
		//add("FilePack::Internal::WriterImpl::BlockNames", (uint8_t*)blockNames.data(), blockNames.size());
		//blocks.push_back(m_blocks["FilePack::Internal::WriterImpl::BlockNames"]);

		////for (auto& block : m_blocks)
		////for (const auto& block : m_blocks)

		//DWORD bytes;
		//WriteFile(m_file, blocks.data(), blocks.size() * sizeof(BlockInfo), &bytes, nullptr);
		//printf("%d bytes written in header.\n", bytes);

		//SetFilePointer(m_file, 8, 0, FILE_BEGIN);
		//const uint64_t headerData = (m_offset << 8) | (0x01ull); // 1 byte version, 7 bytes header offset
		//WriteFile(m_file, &headerData, 8, &bytes, nullptr);
		//if (bytes != 8)
		//	throw std::runtime_error("WriteFile failed.");

		//CloseHandle(m_file);
	}
	catch (...)
	{
		// can't do anything!
	}
}

void FilePack::Internal::WriterImpl::add(const std::string& blockName, const uint8_t* buffer, const size_t size, const int compression)
{
	if (compression > 0)
	{
		//const auto compressed = compressZSTD(compressBlosc(buffer, size, compression));
		const auto compressed = compressZSTD(buffer, size, compression);
		printf("Block \"%s\" compressed from %lld KB to %lld KB (%d%%)\n", blockName.c_str(), size / 1024,
			compressed.size() / 1024, (int)std::round(100. * compressed.size() / (double)size));
		return add(blockName, compressed.data(), compressed.size(), -1 * compression);
	}

	//for (auto& block : m_blocks)
	//	if (block.first == blockName)
	//		throw std::invalid_argument("Block already exists");

	if ((size_t)((DWORD)size) != size)
		throw std::runtime_error("Large blocks not supported!");

	DWORD bytes;
	WriteFile(m_file, buffer, size, &bytes, nullptr);
	if (bytes != size)
		throw std::runtime_error("WriteFile failed.");

	//m_blocks[blockName] = BlockInfo(size, -1 * compression);
	m_blocks.emplace_back(blockName, BlockInfo(size, -1 * compression));
}



FilePack::Reader::Reader(const std::wstring& name)
{
	pimpl = (void*)new Internal::ReaderImpl(name, m_blocks);
}

FilePack::Reader::Reader(const std::string& name)
	: Reader(Internal::fromUTF8(name)) {}

FilePack::Reader::~Reader()
{
	delete static_cast<Internal::ReaderImpl*>(pimpl);
}

const std::string FilePack::Reader::signature() const
{
	return static_cast<Internal::ReaderImpl*>(pimpl)->signatrue();
}

std::vector<uint8_t> FilePack::Reader::processRaw(const BlockInfo& block)
{
	std::vector<uint8_t> ret;

	if (block.compression)
	{
		//ret = std::move(decompressBlosc(decompressZSTD(block.ptr, block.size)));
		ret = std::move(decompressZSTD(block.ptr, block.size));
	}

	return std::move(ret);
}


FilePack::Internal::ReaderImpl::ReaderImpl(const std::wstring& name, std::map<std::string, BlockInfo>& blocks)
	: m_file(nullptr), m_fileMap(nullptr), m_view(nullptr)
{
	m_file = CreateFileW(name.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if (m_file == INVALID_HANDLE_VALUE)
		throw std::runtime_error("CreateFileW failed.");

	LARGE_INTEGER fileSize;
	if (!GetFileSizeEx(m_file, &fileSize))
		throw std::runtime_error("GetFileSizeEx failed.");
	m_size = static_cast<size_t>(fileSize.QuadPart);

	if (m_size < 16)
		throw std::invalid_argument("Invalid FilePack header.");


	m_fileMap = CreateFileMappingW(m_file, nullptr, PAGE_READONLY, 0, 0, nullptr);
	if (m_fileMap == nullptr)
		throw std::runtime_error("CreateFileMappingW failed.");

	m_view = static_cast<uint8_t*>(MapViewOfFile(m_fileMap, FILE_MAP_READ, 0, 0, 0));
	if (m_view == nullptr)
		throw std::runtime_error("MapViewOfFile failed.");


	const auto headerVersion = *(m_view + 8);

	if (headerVersion == 1)
	{
		const size_t headerOffset = *(reinterpret_cast<uint64_t*>(m_view + 8)) >> 8;
		const auto headerSize = m_size - headerOffset;
		if (headerOffset >= m_size)
			throw std::invalid_argument("Invalid FilePack header.");

		auto ptr = m_view + 16;
		std::regex rgx("(\\d+);(\\d+);([^\\0]+)");
#if CompressHeader
		const auto uncompressed = decompressZSTD(m_view + headerOffset, headerSize);
		for (std::cregex_token_iterator i((char*)uncompressed.data(), (char*)uncompressed.data() + uncompressed.size(), rgx, { 1, 2, 3 });
#else
		for (std::cregex_token_iterator i((char*)m_view + headerOffset, (char*)m_view + headerOffset + headerSize, rgx, { 1, 2, 3 });
#endif
			i != std::cregex_token_iterator(); i++)
		{
			BlockInfo block(std::stoull((++i)->str()), std::stoi(i->str()), ptr);
			ptr += block.size;
			blocks[(++i)->str()] = std::move(block);
		}

		//std::copy(std::cregex_token_iterator(ch, ch + (size_t)namesBlockInfo.size, rgx, -1),
		//	std::cregex_token_iterator(), std::back_inserter(blockNames));

		return;

		//std::copy_n(reinterpret_cast<Block*>(m_view + headerOffset), headerSize / sizeof(Block), std::back_inserter(m_blocks));
		//for (auto block : m_blocks)
		//	printf("- block \"%s\" %lld %lld\n", block.name.c_str(), block.offset, block.size);

		//for (auto block = reinterpret_cast<Block*>(m_view + headerOffset); block < reinterpret_cast<Block*>(m_view + m_size); block++)
		const auto blockCount = headerSize / sizeof(BlockInfo);
		
		// last block contains names
		std::vector<std::string> blockNames;
		//{
		//	const auto& namesBlockInfo = reinterpret_cast<BlockInfo*>(m_view + headerOffset)[blockCount - 1];
		//	const auto ch = (char*)m_view + (size_t)namesBlockInfo.offset;
		//	std::regex rgx("\\0");
		//	std::copy(std::cregex_token_iterator(ch, ch + (size_t)namesBlockInfo.size, rgx, -1),
		//		std::cregex_token_iterator(), std::back_inserter(blockNames));
		//}

		// other blocks contain actual data
		uint64_t runningSize = 16;
		for (size_t i = 0; i < blockCount - 1; i++)
		{
			const auto& block = reinterpret_cast<BlockInfo*>(m_view + headerOffset)[i];
			//blocks[blockNames[i]] = block;

			BlockInfo b;
			b.size = block.size;
			b.ptr = m_view + runningSize;
			//b.offset = reinterpret_cast<uint64_t>(m_view) + runningSize;
			runningSize += block.size;
			blocks[blockNames[i]] = std::move(b);
		}

		// add base mapped file address so block offsets are actual valid pointers
		//for (auto& block : blocks)
		//	block.second.offset += reinterpret_cast<uint64_t>(m_view);
	}
	else
	{
		throw std::invalid_argument("FilePack not supported.");
	}
}

FilePack::Internal::ReaderImpl::~ReaderImpl()
{
	if (m_view != nullptr)
		UnmapViewOfFile(m_view);

	if (m_fileMap != nullptr)
		CloseHandle(m_fileMap);

	if (m_file != INVALID_HANDLE_VALUE)
		CloseHandle(m_file);
}

const std::string FilePack::Internal::ReaderImpl::signatrue() const
{
	return std::string((char*)m_view, 8);
}

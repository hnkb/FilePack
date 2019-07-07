
#include "Compression.h"
#include <stdexcept>
#include <zstd.h>
#include <lz4.h>
#include <blosc.h>


std::vector<uint8_t> compressZSTD(const uint8_t* data, const size_t size, const int level)
{
	std::vector<uint8_t> compressed(ZSTD_compressBound(size));

	const auto ret = ZSTD_compress(compressed.data(), compressed.size(), data, size, level);
	if (ZSTD_isError(ret))
		throw std::runtime_error("ZSTD_compress failed.");
	compressed.resize(ret);

	return compressed;
}

std::vector<uint8_t> decompressZSTD(const uint8_t * data, const size_t size)
{
	std::vector<uint8_t> uncompressed;

	const auto contentSize = ZSTD_getFrameContentSize(data, size);
	if (contentSize == ZSTD_CONTENTSIZE_ERROR || contentSize == ZSTD_CONTENTSIZE_UNKNOWN)
	{
		throw std::runtime_error("ZSTD_getFrameContentSize failed.");
	}
	else
	{
		uncompressed.resize(contentSize);
		const auto actualSize = ZSTD_decompress(uncompressed.data(), uncompressed.size(), data, size);
		if (ZSTD_isError(actualSize))
			throw std::runtime_error("ZSTD_compress failed.");
		uncompressed.resize(actualSize);
	}

	return uncompressed;
}

std::vector<uint8_t> compressLZ4(const uint8_t* data, const size_t size)
{
	std::vector<uint8_t> compressed(LZ4_compressBound((int)size));

	const auto ret = LZ4_compress_fast((char*)data, (char*)compressed.data(), (int)size, (int)compressed.size(), 1);
	if (ret == 0)
		throw std::runtime_error("LZ4_compress_fast failed.");
	compressed.resize(ret);

	return compressed;
}

std::vector<uint8_t> decompressLZ4(const uint8_t* data, const size_t size, const size_t contentSize)
{
	std::vector<uint8_t> uncompressed;

	uncompressed.resize(contentSize);
	const auto actualSize = LZ4_decompress_safe((char*)data, (char*)uncompressed.data(), (int)size, (int)uncompressed.size());
	if (actualSize < 0)
		throw std::runtime_error("LZ4_decompress_safe failed.");
	uncompressed.resize(actualSize);

	return uncompressed;
}

std::vector<uint8_t> compressBlosc(const uint8_t* data, const size_t size, const int level)
{
	std::vector<uint8_t> compressed(size + BLOSC_MAX_OVERHEAD);

	blosc_init();

	const auto csize = blosc_compress(level, BLOSC_SHUFFLE, 4, size, data, compressed.data(), compressed.size());
	if (csize == 0) {
		printf("Buffer is uncompressible.  Giving up.\n");
		compressed.resize(size);
		std::copy_n(data, size, compressed.begin());
	}
	else if (csize < 0) {
		printf("Compression error.  Error code: %d\n", csize);
		compressed.resize(size);
		std::copy_n(data, size, compressed.begin());
	}
	else {
		compressed.resize(csize);
	}

	blosc_destroy();

	return compressed;
}

std::vector<uint8_t> decompressBlosc(const uint8_t* data, const size_t size)
{
	blosc_init();

	size_t contentSize;
	if (blosc_cbuffer_validate(data, size, &contentSize))
		throw std::invalid_argument("blosc_cbuffer_validate failed.");

	std::vector<uint8_t> decompressed(contentSize);

	const auto csize = blosc_decompress(data, decompressed.data(), decompressed.size());
	if (csize <= 0)
		throw std::invalid_argument("Buffer is corrupt.");
	else {
		decompressed.resize(csize);
	}

	blosc_destroy();

	return decompressed;
}

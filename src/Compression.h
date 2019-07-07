#pragma once

#include <vector>
#include <cstdint>

std::vector<uint8_t> compressZSTD(const uint8_t* data, const size_t size, const int level = 2);
std::vector<uint8_t> decompressZSTD(const uint8_t* data, const size_t size);

std::vector<uint8_t> compressLZ4(const uint8_t* data, const size_t size);
std::vector<uint8_t> decompressLZ4(const uint8_t* data, const size_t size, const size_t contentSize);

std::vector<uint8_t> compressBlosc(const uint8_t* data, const size_t size, const int level = 5);
std::vector<uint8_t> decompressBlosc(const uint8_t* data, const size_t size);





template <class Type, template <typename> class Container>
std::vector<uint8_t> compressZSTD(const Container<Type>& data, const int level = 5) {
	return compressZSTD((uint8_t*)data.data(), data.size() * sizeof(Type), level);
}
template <class Type, template <typename, typename> class Container, class A>
std::vector<uint8_t> compressZSTD(const Container<Type, A>& data, const int level = 5) {
	return compressZSTD((uint8_t*)data.data(), data.size() * sizeof(Type), level);
}
template <class Type, template <typename, typename, typename> class Container, class A, class B>
std::vector<uint8_t> compressZSTD(const Container<Type, A, B>& data, const int level = 5) {
	return compressZSTD((uint8_t*)data.data(), data.size() * sizeof(Type), level);
}
template <class Type, template <typename> class Container>
std::vector<uint8_t> compressBlosc(const Container<Type>& data, const int level = 5) {
	return compressBlosc((uint8_t*)data.data(), data.size() * sizeof(Type), level);
}
template <class Type, template <typename, typename> class Container, class A>
std::vector<uint8_t> compressBlosc(const Container<Type, A>& data, const int level = 5) {
	return compressBlosc((uint8_t*)data.data(), data.size() * sizeof(Type), level);
}
template <class Type, template <typename, typename, typename> class Container, class A, class B>
std::vector<uint8_t> compressBlosc(const Container<Type, A, B>& data, const int level = 5) {
	return compressBlosc((uint8_t*)data.data(), data.size() * sizeof(Type), level);
}

inline std::vector<uint8_t> decompressZSTD(const std::vector<uint8_t>& data) {
	return decompressZSTD(data.data(), data.size());
}
inline std::vector<uint8_t> decompressBlosc(const std::vector<uint8_t>& data) {
	return decompressBlosc(data.data(), data.size());
}

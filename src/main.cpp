
#include "FilePack.h"
#include "Compression.h"
#include <vector>
#include <iostream>

using namespace std;


void testZSTD()
{
	const wstring data = L"Hello. My name is Hani. I am compressing myself. Behave the same as ZSTD_compressCCtx(), but compression parameters are set using the advanced API.";
	//vector<uint8_t> uncompressed(data.size() * 2);
	//std::copy_n((uint8_t*)data.data(), uncompressed.size(), uncompressed.begin());

	const auto compressed1 = compressBlosc((uint8_t*)data.data(), data.size() * 2);
	const auto compressed = compressZSTD(compressed1.data(), compressed1.size());
	//const auto compressed = compressZSTD((uint8_t*)data.data(), data.size() * 2);
	cout << "\nCompressed size = " << compressed.size() << "\n";

	const auto uncompressed = decompressZSTD(compressed.data(), compressed.size());
	cout << "\nUncompressed size = " << uncompressed.size() << "\n";

	wcout << "\nUncompressed data:\n" << std::wstring((wchar_t*)uncompressed.data(), uncompressed.size() / 2) << "\n";
}

void benchmark(uint8_t* data, const size_t size)
{
	cout << "\nUncompressed size = " << size << " bytes\n";
	for (int i = 1; i < 10; i++)
	{
		const auto cp = compressZSTD(data, size, i);
		cout << "Level " << i << " compressed = " << cp.size() << " bytes\t(" << std::round(100. * cp.size() / size) << "%)\n";

		const auto cp2 = compressBlosc(data, size, i);
		cout << "Level " << i << " Blosc      = " << cp2.size() << " bytes\t(" << std::round(100. * cp2.size() / size) << "%)\n";

		const auto cp3 = compressZSTD(cp2.data(), cp2.size(), i);
		cout << "Level " << i << " Blosc/ZSTD = " << cp3.size() << " bytes\t(" << std::round(100. * cp3.size() / size) << "%)\n";
	}
}
template <class Type, template <typename, typename> class Container, class A>
void benchmark(const Container<Type, A>& data) { benchmark((uint8_t*)data.data(), data.size() * sizeof(Type)); }
template <class Type, template <typename, typename, typename> class Container, class A, class B>
void benchmark(const Container<Type, A, B>& data) { benchmark((uint8_t*)data.data(), data.size() * sizeof(Type)); }


int main()
{
	//testZSTD();
	//return 0;



	if (1)
	{
		std::vector<int> numbers;
		std::string id1;
		std::wstring id2;
		for (int i = 1001; i < 3000; i++)
		{
			numbers.push_back(i);
			id1 += ";" + std::to_string(i) + '\0';
			id2 += L";" + std::to_wstring(i) + L'\0';
			//id1 += ";" + std::to_string(i) + "\\";
			//id2 += L";" + std::to_wstring(i) + L"\\";
		}

		//benchmark(id1);
		//benchmark(id2);
		//benchmark(numbers);

		//const auto in = compressBlosc((uint8_t*)id2.data(), 2 * id2.size());
		//cout << in.size() << "\n";
		//const auto out = decompressBlosc(in.data(), in.size());
		//cout << out.size() << "\n";
		//std::wcout << std::wstring((wchar_t*)out.data(), out.size() / 2);

		//return 0;

		FilePack::Writer fp(L"D:\\Temp\\test.fp", "HANIFP01");
		fp.add("TEXT0", (uint8_t*)"Hello, my dear", 14);
		fp.add("TEXT1", (uint8_t*)"Ich bin Hani", 12, 5);
		//fp.add("TEXT2", std::string("How about this?"));
		//fp.add("numbers", numbers);
		//fp.add("ASCR", id1);
		//fp.add("ASCC", id1, 4);
		//fp.add("UNCR", id2);
		//fp.add("UNCC", id2, 4);
		//fp.add("NUMR", numbers);
		//fp.add("NUMC", numbers, 4);

		fp.add("ASCC", id1, 5);
		fp.add("UNCC", id2, 5);
		fp.add("NUMC", numbers, 5);
	}

	if (1)
	{
		FilePack::Reader fp(L"D:\\Temp\\test.fp");

		std::cout << "File signature is \"" << fp.signature() << "\"\n";
		for (auto block : fp.blocks())
			printf("- block \"%s\" %lld bytes, compression %d\n", block.first.c_str(), block.second.size, block.second.compression);

		{
			auto b = fp.get<char>("TEXT0");
			cout << b.size() << "\n";
			cout << std::string(b.begin(), b.end()) << "\n";
		}
		{
			auto a = fp.get<char>("TEXT1");
			auto b = a;
			cout << b.size() << "\n";
			cout << std::string(b.begin(), b.end()) << "\n";
		}
		{
			auto b = fp.get<int>("NUMC");
			cout << b.size() << "\n";
			for (auto& n : b)
				cout << n << " ";
			cout << "\n";
		}
	}

	std::cout << "Done.\n";
}


#pragma once

#include <string>
#include <cstdint>
#include <map>
#include <vector>


namespace FilePack
{

	struct BlockInfo
	{
		uint8_t* ptr = nullptr;
		uint64_t size = 0;
		int compression = 0;

		BlockInfo() = default;
		BlockInfo(const size_t size, const int compression) : size(size), compression(compression) {}
		BlockInfo(const size_t size, const int compression, uint8_t* ptr) : size(size), ptr(ptr), compression(compression) {}
	};

	class Writer
	{
	public:
		Writer(const std::wstring& name, const char signature[8]);
		Writer(const std::string& name, const char signature[8]);
		~Writer();

		template <class Type>
		void add(const std::string& blockName, const Type* buffer, const size_t size, const int compression = 0) {
			addRaw(blockName, reinterpret_cast<const uint8_t*>(buffer), size * sizeof(Type), compression);
		}
		template <class Type, template <typename> class Container>
		void add(const std::string& blockName, const Container<Type>& data, const int compression = 0) {
			add(blockName, (const Type*)data.data(), (size_t)data.size(), compression);
		}
		template <class Type, template <typename, typename> class Container, class A>
		void add(const std::string& blockName, const Container<Type, A>& data, const int compression = 0) {
			add(blockName, (const Type*)data.data(), (size_t)data.size(), compression);
		}
		template <class Type, template <typename, typename, typename> class Container, class A, class B>
		void add(const std::string& blockName, const Container<Type, A, B>& data, const int compression = 0) {
			add(blockName, (const Type*)data.data(), (size_t)data.size(), compression);
		}

	private:
		void addRaw(const std::string& blockName, const uint8_t* buffer, const size_t size, const int compression);
		void* pimpl;
	};

	class Reader
	{
	public:
		Reader(const std::wstring& name);
		Reader(const std::string& name);
		~Reader();

		const std::string signature() const;
		const std::map<std::string, BlockInfo>& blocks() const { return m_blocks; }
		bool exists(const std::string& blockName) { return (m_blocks.find(blockName) != m_blocks.end()); }

		template <class Type>
		class Block
		{
		public:
			size_t size() const { return m_size; }
			const Type* data() const { return m_ptr; }
			const Type* begin() const { return m_ptr; }
			const Type* end() const { return m_ptr + m_size; }
			const Type& operator [](const size_t index) const { return m_ptr[index]; }

		private:
			Block(BlockInfo& b) : m_size(b.size / sizeof(Type)), m_ptr(reinterpret_cast<Type*>(b.ptr)) {}
			Block(std::vector<uint8_t>&& alloc) : m_allocatedData(std::move(alloc)),
				m_size(m_allocatedData.size() / sizeof(Type)), m_ptr(reinterpret_cast<Type*>(m_allocatedData.data())) {}

			std::vector<uint8_t> m_allocatedData;
			Type* m_ptr;
			size_t m_size;

			friend Reader;
		};

		template <class Type>
		Block<Type> get(const std::string& blockName) {
			auto block = m_blocks.find(blockName);
			if (block == m_blocks.end())
				throw std::invalid_argument("Block does not exist in pack");
			//if (block->second.size % sizeof(Type))
			//	throw std::invalid_argument("Block size incorrect");
			auto alloc = processRaw(block->second);
			return alloc.size() ?
				Block<Type>(std::move(alloc)) :
				Block<Type>(block->second);
		}

	private:
		std::vector<uint8_t> processRaw(const BlockInfo& block);
		std::map<std::string, BlockInfo> m_blocks;
		void* pimpl;
	};

}

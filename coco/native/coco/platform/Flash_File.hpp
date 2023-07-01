#pragma once

#include <coco/BufferImpl.hpp>
#include <coco/platform/File.hpp>


namespace coco {

/**
	Blocking implementation of flash interface based on files, mainly for testing purposes.
*/
class Flash_File {
public:
	/**
		Constructor
		@param name file name
		@param size size of emulated flash
		@param pageSize size of one page that can be erased at once
		@param blockSize size of one block that can be written at once and is the read alignment
	*/
	Flash_File(String name, int size, int pageSize, int blockSize);

	class Buffer : public BufferImpl {
	public:
		/**
			Constructor
		*/
		Buffer(Flash_File &file, int size);
		~Buffer() override;

		bool setHeader(const uint8_t *data, int size) override;
		using BufferImpl::setHeader;
		bool startInternal(int size, Op op) override;
		void cancel() override;

	protected:
		Flash_File &file;
		uint32_t address = 0xf0000000; // invalid
	};

protected:
	File file;
	uint32_t size;
	int pageSize;
	int blockSize;
};

} // namespace coco

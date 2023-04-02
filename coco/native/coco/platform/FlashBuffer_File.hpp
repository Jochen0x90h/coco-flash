#pragma once

#include <coco/Buffer.hpp>
#include <coco/platform/File.hpp>


namespace coco {

/**
	Flash emulation based on file, mainly for testing purposes
*/
class FlashBuffer_File : public Buffer {
public:
	/**
		Constructor
	*/
	FlashBuffer_File(int capacity, const fs::path &filename, int pageSize, int blockSize);
	~FlashBuffer_File() override;

	bool start(Op op) override;
	void cancel() override;

protected:
	File file;
	int pageSize;
	int blockSize;
};

} // namespace coco

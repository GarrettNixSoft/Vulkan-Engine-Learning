#include "fve_memory.hpp"

namespace fve {

	VmaAllocator fveAllocator;

	void FveMemory::init(FveDevice& device) {

		VmaAllocatorCreateInfo allocatorInfo{};
		allocatorInfo.physicalDevice = device.physicalDevice();
		allocatorInfo.device = device.device();
		allocatorInfo.instance = device.instance();
		vmaCreateAllocator(&allocatorInfo, &fveAllocator);

	}

}
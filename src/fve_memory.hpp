#pragma once

#include "fve_types.hpp"
#include "fve_model.hpp"

#ifndef VMA_DEBUG_LOG
#define VMA_DEBUG_LOG(format, ...) do { \
       if (strncmp(format, "vmaFlushAllocation", 18)) { \
       printf(format, __VA_ARGS__); \
       printf("\n"); }\
   } while(false)
#endif

#include <vma/vk_mem_alloc.h>

#include <iostream>

namespace fve {

	extern VmaAllocator fveAllocator;

	class FveMemory {

	public:
		static void init(FveDevice& device);

	private:

	};

}
#pragma once

#include "fve_types.hpp"
#include "fve_model.hpp"


#ifndef NDEBUG
#define VMA_DEBUG_LOG
#endif
#include <vma/vk_mem_alloc.h>

namespace fve {

	extern VmaAllocator fveAllocator;

	class FveMemory {

	public:
		static void init(FveDevice& device);

	private:

	};

}
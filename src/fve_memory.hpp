#pragma once

#include "fve_types.hpp"
#include "fve_model.hpp"

#include <vma/vk_mem_alloc.h>

namespace fve {

	extern VmaAllocator fveAllocator;

	class FveMemory {

	public:
		static void init(FveDevice& device);

	private:

	};

}
#pragma once

#include "fve_types.hpp"
#include "fve_device.hpp"

namespace fve {

	bool loadImageFromFile(FveDevice& device, const char* filePath, AllocatedImage& outImage);

}
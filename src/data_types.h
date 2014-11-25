
#pragma once
#ifndef FBMP_DATA_TYPES_H
#define FBMP_DATA_TYPES_H

#include <cstdint>
#include <string>

namespace fbmp
{

	#pragma pack(2)
	struct main_header
	{
		char magic[2];
		int32_t file_size = 0;
		int16_t reserved1 = 0;
		int16_t reserved2 = 0;
		int32_t offset = 0;

		std::string details()
		{
			return std::string("size: ") + std::to_string(file_size) +
				" reserved1: " + std::to_string(reserved1) +
				" reserved2: " + std::to_string(reserved2) +
				" offset: " + std::to_string(offset);
		}
	};
	#pragma pack()

	struct dib_header
	{
		int32_t width = 0;
		int32_t height = 0;
		int16_t planes = 0;
		int16_t bitCount = 0;
		int32_t compression = 0;
		int32_t imageSize = 0;
		int32_t XPeelsPerMeter = 0;
		int32_t YPeelsPerMeter = 0;
		int32_t paletteColors = 0;
		int32_t importantColors = 0;

		std::string details()
		{
			return std::string("width: ") + std::to_string(width) +
				" height: " + std::to_string(height) +
				" planes: " + std::to_string(planes) +
				" bitCount: " + std::to_string(bitCount) +
				" compression: " + std::to_string(compression) +
				" imageSize: " + std::to_string(imageSize) +
				" XPeelsPerMeter: " + std::to_string(XPeelsPerMeter) +
				" YPeelsPerMeter: " + std::to_string(YPeelsPerMeter) +
				" paletteColors: " + std::to_string(paletteColors) +
				" importantColors: " + std::to_string(importantColors);
		}
	};

}

#endif //FBMP_DATA_TYPES_H

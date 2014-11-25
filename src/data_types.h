
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
			return std::string("Main header:")
				+ "\n" "Size: " + std::to_string(file_size)
				+ "\n" "Reserved1: " + std::to_string(reserved1)
				+ "\n" "Reserved2: " + std::to_string(reserved2)
				+ "\n" "Offset: " + std::to_string(offset)
				+ "\n\n";
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
			return std::string("DIB header:")
				+ "\n" "Width: " + std::to_string(width)
				+ "\n" "Height: " + std::to_string(height)
				+ "\n" "Planes: " + std::to_string(planes)
				+ "\n" "Bit Count: " + std::to_string(bitCount)
				+ "\n" "Compression: " + std::to_string(compression)
				+ "\n" "Image Size: " + std::to_string(imageSize)
				+ "\n" "X Peels Per Meter: " + std::to_string(XPeelsPerMeter)
				+ "\n" "Y Peels Per Meter: " + std::to_string(YPeelsPerMeter)
				+ "\n" "Palette Colors: " + std::to_string(paletteColors)
				+ "\n" "Important Colors: " + std::to_string(importantColors)
				+ "\n\n";
		}
	};

}

#endif //FBMP_DATA_TYPES_H

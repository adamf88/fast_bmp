
#pragma once
#ifndef FBMP_DATA_TYPES_H
#define FBMP_DATA_TYPES_H

#include <cstdint>
#include <string>
#include <memory>
#include "exception.h"

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
				+ "\n" "Magic: " + std::string(magic, magic + 2)
				+ "\n" "Size: " + std::to_string(file_size)
				+ "\n" "Reserved1: " + std::to_string(reserved1)
				+ "\n" "Reserved2: " + std::to_string(reserved2)
				+ "\n" "Offset: " + std::to_string(offset)
				+ "\n\n";
		}
	};
	#pragma pack()

	enum class dib_header_type
	{
		bitmap_unknown_header	= 0,
		bitmap_core_header		= 12,
		bitmap_core_header2		= 64,
		bitmap_info_header		= 40,
		bitmap_v2_info_header	= 52,
		bitmap_v3_info_header	= 56,
		bitmap_v4_header		= 108,
		bitmap_v5_header		= 124
	};

	class dib_header
	{
	public:
		inline static std::unique_ptr<dib_header> create_header(int32_t header_size);

	public:
		virtual ~dib_header(){}

		virtual void* data() = 0;
		virtual const void* data() const = 0;
		virtual dib_header_type header_type() const = 0;

		virtual int32_t size() const = 0;
		virtual int32_t width() const = 0;
		virtual int32_t height() const = 0;
		virtual int16_t planes() const = 0;
		virtual int16_t bit_count() const = 0;
		virtual int32_t compression() const = 0;
		virtual int32_t image_size() const = 0;
		virtual int32_t x_peels_per_meter() const = 0;
		virtual int32_t y_peels_per_meter() const = 0;
		virtual int32_t palette_colors() const = 0;
		virtual int32_t important_colors() const = 0;

		std::string details()
		{
			return std::string("DIB header: ")
				+ "\n" "Size: " + std::to_string(size())
				+ "\n" "Width: " + std::to_string(width())
				+ "\n" "Height: " + std::to_string(height())
				+ "\n" "Planes: " + std::to_string(planes())
				+ "\n" "Bit Count: " + std::to_string(bit_count())
				+ "\n" "Compression: " + std::to_string(compression())
				+ "\n" "Image Size: " + std::to_string(image_size())
				+ "\n" "X Peels Per Meter: " + std::to_string(x_peels_per_meter())
				+ "\n" "Y Peels Per Meter: " + std::to_string(y_peels_per_meter())
				+ "\n" "Palette Colors: " + std::to_string(palette_colors())
				+ "\n" "Important Colors: " + std::to_string(important_colors())
				+ "\n\n";
		}

		
	};

	struct bitmap_info_header_data
	{
		int32_t width = 0;
		int32_t height = 0;
		int16_t planes = 0;
		int16_t bit_count = 0;
		int32_t compression = 0;
		int32_t image_size = 0;
		int32_t x_peels_per_meter = 0;
		int32_t y_peels_per_meter = 0;
		int32_t palette_colors = 0;
		int32_t important_colors = 0;
	};

	class dib_bitmap_info_header : public dib_header
	{
	public:
		bitmap_info_header_data header;

		void* data() override { return &header; }
		const void* data() const { return &header; }
		dib_header_type header_type() const override { return dib_header_type::bitmap_info_header; }

		int32_t size() const override { return 40; }
		int32_t width() const override { return header.width; }
		int32_t height() const override { return header.height; }
		int16_t planes() const override { return header.planes; }
		int16_t bit_count() const override { return header.bit_count; }
		int32_t compression() const override { return header.compression; }
		int32_t image_size() const override { return header.image_size; }
		int32_t x_peels_per_meter() const override { return header.x_peels_per_meter; }
		int32_t y_peels_per_meter() const override { return header.y_peels_per_meter; }
		int32_t palette_colors() const override { return header.palette_colors; }
		int32_t important_colors() const  override { return header.important_colors; }
	};

	struct bitmap_core_header_data
	{
		int16_t width = 0;
		int16_t height = 0;
		int16_t planes = 0;
		int16_t bit_count = 0;
	};

	class dib_bitmap_core_header : public dib_header
	{
	public:
		bitmap_core_header_data header;

		void* data() override { return &header; }
		const void* data() const { return &header; }
		dib_header_type header_type() const override { return dib_header_type::bitmap_core_header; }

		int32_t size() const override { return 12; }
		int32_t width() const override { return header.width; }
		int32_t height() const override { return header.height; }
		int16_t planes() const override { return header.planes; }
		int16_t bit_count() const override { return header.bit_count; }
		int32_t compression() const override { return 0; }
		int32_t image_size() const override { return 0; }
		int32_t x_peels_per_meter() const override { return 0; }
		int32_t y_peels_per_meter() const override { return 0; }
		int32_t palette_colors() const override { return 0; }
		int32_t important_colors() const  override { return 0; }
	};

	struct bitmap_core_header2_data
	{
		int32_t width = 0;
		int32_t height = 0;
		int16_t planes = 0;
		int16_t bit_count = 0;
		int32_t compression = 0;
		int32_t image_size = 0;
		int32_t x_peels_per_meter = 0;
		int32_t y_peels_per_meter = 0;
		int32_t palette_colors = 0;
		int32_t important_colors = 0;
		int16_t resUnit = 0;
		int16_t reserved = 0;
		int16_t orientation = 0;
		int16_t halftoning = 0;
		int32_t halftone_size1 = 0;
		int32_t halftone_size2 = 0;
		int32_t color_space = 0;
		int32_t app_data = 0;
	};

	class dib_bitmap_core_header2 : public dib_header
	{
	public:
		bitmap_core_header2_data header;

		void* data() override { return &header; }
		const void* data() const { return &header; }
		dib_header_type header_type() const override { return dib_header_type::bitmap_core_header2; }

		int32_t size() const override { return 64; }
		int32_t width() const override { return header.width; }
		int32_t height() const override { return header.height; }
		int16_t planes() const override { return header.planes; }
		int16_t bit_count() const override { return header.bit_count; }
		int32_t compression() const override { return 0; }
		int32_t image_size() const override { return 0; }
		int32_t x_peels_per_meter() const override { return 0; }
		int32_t y_peels_per_meter() const override { return 0; }
		int32_t palette_colors() const override { return 0; }
		int32_t important_colors() const  override { return 0; }
	};

	//this header may have various size
	class dib_bitmap_Os22xBitmapHeader : public dib_header
	{
	public:
		bitmap_core_header2_data header;

		void* data() override { return &header; }
		const void* data() const { return &header; }
		dib_header_type header_type() const override { return dib_header_type::bitmap_core_header2; }

		int32_t size() const override { return -1; } //max 64 bytes
		int32_t width() const override { return header.width; }
		int32_t height() const override { return header.height; }
		int16_t planes() const override { return header.planes; }
		int16_t bit_count() const override { return header.bit_count; }
		int32_t compression() const override { return header.compression; }
		int32_t image_size() const override { return header.image_size; }
		int32_t x_peels_per_meter() const override { return header.x_peels_per_meter; }
		int32_t y_peels_per_meter() const override { return header.y_peels_per_meter; }
		int32_t palette_colors() const override { return header.palette_colors; }
		int32_t important_colors() const  override { return header.important_colors; }
	};

	inline std::unique_ptr<dib_header> dib_header::create_header(int32_t header_size)
	{
		const dib_header_type header_type = static_cast<dib_header_type>(header_size);
		switch (header_type)
		{
		case dib_header_type::bitmap_core_header:
			return std::unique_ptr<dib_header>(new dib_bitmap_core_header());
		case dib_header_type::bitmap_core_header2:
			return std::unique_ptr<dib_header>(new dib_bitmap_core_header2());
		case dib_header_type::bitmap_info_header:
			return std::unique_ptr<dib_header>(new dib_bitmap_info_header());
		case dib_header_type::bitmap_v2_info_header:
			break;
		case dib_header_type::bitmap_v3_info_header:
			break;
		case dib_header_type::bitmap_v4_header:
			break;
		case dib_header_type::bitmap_v5_header:
			break;
		default:
			break;
			//throw exception(std::string("Unsuppoerted bitmap dib header size - ") + std::to_string(static_cast<int>(header_type)));
		}
		
		if ( header_size <= 64 )
			return std::unique_ptr<dib_header>(new dib_bitmap_Os22xBitmapHeader());
		else
			throw exception(std::string("Unsuppoerted bitmap dib header size - ") + std::to_string(static_cast<int>(header_type)));
	}
}

#endif //FBMP_DATA_TYPES_H

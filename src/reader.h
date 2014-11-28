
#pragma once
#ifndef FBMP_READER_H
#define FBMP_READER_H

#include <cstdint>

#include "data_types.h"
#include "stream.h"
#include "image.h"

namespace fbmp
{

	class reader
	{
	public:
		reader();
		void read(input_stream& stream);

		const main_header& get_main_header() const { return _header; }
		main_header& get_main_header() { return _header; }

		const dib_header& get_dib_header() const { return *_dib_header; }
		dib_header& get_dib_header() { return *_dib_header; }

		const image& get_image() const { return _image; }

	public:
		void read_header(input_stream& stream);
		void read_dib_header(input_stream& stream);
		void read_image(input_stream& steram);
		void read_palette(input_stream& stream, size_t colors);

		image _image;
		int32_t _dib_header_size;
		main_header _header;
		uint32_t palette[256];
		std::unique_ptr<dib_header> _dib_header;
	};

}

#endif //FBMP_READER_H

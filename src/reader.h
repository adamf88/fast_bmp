
#pragma once
#ifndef FBMP_READER_H
#define FBMP_READER_H

#include <cstdint>

#include "data_types.h"
#include "stream.h"
#include "image.h"

namespace fbmp
{
	static_assert(sizeof(main_header) == 14, "wrong size of BmpHeader");

	class reader
	{
	public:
		reader(input_stream& stream);
		reader(input_stream* stream);
		~reader();

		void read();

		const main_header& get_main_header() const { return _header; }
		main_header& get_main_header() { return _header; }

		const dib_header& get_dib_header() const { return *_dib_header; }
		dib_header& get_dib_header() { return *_dib_header; }

		const image& get_image() const { return _image; }

		input_stream& Stream() { return _stream; }

	public:
		void read_header();
		void read_dib_header();
		void read_palette();
		void read_image();
	private:
		bool is_palette_black_white();

		void read_1bpp(int width, int height, int row_size, bool flipped);
		void read_4bpp(int width, int height, int row_size, bool flipped);
		void read_8bpp(int width, int height, int row_size, bool flipped);
		void read_24bpp(int width, int height, int row_size, bool flipped);
		void read_32bpp(int width, int height, int row_size, bool flipped);

	private:
		input_stream& _stream;

		main_header _header;
		std::unique_ptr<dib_header> _dib_header;

		image _image;
		uint32_t _palette[256];
	};

}

#endif //FBMP_READER_H

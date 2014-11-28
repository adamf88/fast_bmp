
#include <memory>
#include <cassert>
#include "reader.h"


namespace fbmp
{
	class input_stream_handle
	{
	public:
		input_stream_handle(input_stream& stream)
			: m_stream(stream)
		{
			m_stream.open_for_reading();
		}

		input_stream_handle(const input_stream_handle&) = delete;
		input_stream_handle& operator=(const input_stream_handle&) = delete;

		~input_stream_handle()
		{
			m_stream.close();
		}

	private:
		input_stream& m_stream;
	};

	reader::reader()
	{
		static_assert(sizeof(main_header) == 14, "wrong size of BmpHeader");
	}

	void reader::read(input_stream& stream)
	{
		input_stream_handle streamHandle(stream);

		read_header(stream);
		read_dib_header(stream);

		read_image(stream);
	}


	void reader::read_header(input_stream& stream)
	{
		stream.read(&_header, sizeof(main_header), 1);
		if (_header.magic[0] != 'B' && _header.magic[1] != 'M')
		{
			throw exception("Bad magic number. The file should begin from: BM");
		}
	}

	void reader::read_dib_header(input_stream& stream)
	{
		stream.read(&_dib_header_size, sizeof(int32_t), 1);
		_dib_header = dib_header::create_header(_dib_header_size);
		stream.read(_dib_header->data(), _dib_header->size() - sizeof(int32_t), 1);
	}

	void reader::read_image(input_stream& stream)
	{
		const dib_header& info_header = *_dib_header;
		const int32_t width = info_header.width();
		const int32_t height = abs(info_header.height());
		const int32_t bit_count = info_header.bit_count();
		const bool flipped = info_header.height() > 0;
		const int row_size = ((bit_count * width + 31) / 32) * 4; //padding to 4 bytes

		if (bit_count == 1)
		{
			_image.reset(width, height, 1);

			stream.seek(_header.offset);

			std::unique_ptr<uint8_t[]> line_buffer(new uint8_t[row_size]);

			for (int y = height - 1; y >= 0; --y)
			{
				int streamPos = 0;
				uint8_t* data = _image.get_row_begin(y);
				const uint8_t* const data_end = data + width;

				stream.read(line_buffer.get(), sizeof(uint8_t), row_size);

				while (data < data_end - 8)
				{
					uint8_t value = line_buffer[streamPos++];
					data[0] = ((value >> 7) & 1) * 255;
					data[1] = ((value >> 6) & 1) * 255;
					data[2] = ((value >> 5) & 1) * 255;
					data[3] = ((value >> 4) & 1) * 255;
					data[4] = ((value >> 3) & 1) * 255;
					data[5] = ((value >> 2) & 1) * 255;
					data[6] = ((value >> 1) & 1) * 255;
					data[7] = ((value >> 0) & 1) * 255;
					data += 8;
				}

				if (data < data_end)
				{
					uint8_t value = line_buffer[streamPos];
					for (int k = 7; data < data_end; --k)
					{
						*data++ = ((value >> k) & 1) * 255;
					}
				}
			}
		}
		else if ( bit_count <= 8)
		{
			uint32_t colors_in_color_table = info_header.palette_colors();
			if (colors_in_color_table == 0)
				colors_in_color_table = 1 << bit_count;
			read_palette(stream, colors_in_color_table);

			_image.reset(width, height, 3);
			stream.seek(_header.offset);

			std::unique_ptr<uint8_t[]> line_buffer(new uint8_t[row_size]);
			int s = 0;
			int e = height - 1;
			int i = 1;
			if (flipped)
			{
				s = height - 1;
				e = -1;
				i = -1;
			}

			if (bit_count == 4)
			{
				for (int y = s; y != e; y += i)
				{
					stream.read(line_buffer.get(), sizeof(uint8_t), row_size);
					uint8_t* begin = _image.get_row_begin(y);
					const uint8_t* const end = _image.get_row_end(y);
					uint8_t* data = line_buffer.get();
					while (begin != end)
					{
						uint8_t value = *data;
						uint32_t color = palette[value >> 4];
						begin[0] = (uint8_t)(color >> 16);
						begin[1] = (uint8_t)(color >> 8);
						begin[2] = (uint8_t)color;
						begin += 3;

						if (begin < end)
						{
							uint32_t color = palette[value & 0xF];
							begin[0] = (uint8_t)(color >> 16);
							begin[1] = (uint8_t)(color >> 8);
							begin[2] = (uint8_t)color;
							begin += 3;
						}
						++data;
					}
				}
			}
			else if (bit_count == 8)
			{
				for (int y = s; y != e; y += i)
				{
					stream.read(line_buffer.get(), sizeof(uint8_t), row_size);
					uint8_t* begin = _image.get_row_begin(y);
					const uint8_t* const end = _image.get_row_end(y);
					uint8_t* data = line_buffer.get();
					while (begin != end)
					{
						uint32_t color = palette[*data];// palette[*data];
						++data;
						begin[0] = (uint8_t)(color >> 16);
						begin[1] = (uint8_t)(color >> 8);
						begin[2] = (uint8_t)color;
						begin += 3;
					}
				}
			}
			else
				throw exception(std::string("not supported bpp ") + std::to_string(bit_count));
		}
		else if (bit_count == 24)
		{		
			_image.reset(width, height, 3, row_size);

			stream.seek(_header.offset);
			stream.read(_image.data(), sizeof(uint8_t), row_size * height);

			if (!flipped)
			{
				for (int i = 0; i < height; ++i)
				{
					uint8_t* a = _image.get_row_begin(i);
					const uint8_t* const end = _image.get_row_end(i);
					while (a < end)
					{
						uint8_t tmp = a[0];
						a[0] = a[2];
						a[2] = tmp;
						a += 3;
					}
				}
			}
			else
			{
				for (int i = 0, j = height - 1; i < (height / 2); ++i, --j)
				{
					const uint8_t* const end = _image.get_row_end(i);
					uint8_t* a = _image.get_row_begin(i);
					uint8_t* b = _image.get_row_begin(j);
					while (a < end)
					{
						uint32_t ac = *(uint32_t*)a;
						uint32_t bc = *(uint32_t*)b;
						b[0] = (uint8_t)(ac >> 16);
						b[1] = (uint8_t)(ac >> 8);
						b[2] = (uint8_t)ac;
						b += 3;
						a[0] = (uint8_t)(bc >> 16);
						a[1] = (uint8_t)(bc >> 8);
						a[2] = (uint8_t)bc;
						a += 3;
					}
				}

				if (height % 2 == 1)
				{
					size_t y = height / 2;
					uint8_t* a = _image.get_row_begin(y);
					const uint8_t* const end = _image.get_row_end(y);
					while (a < end)
					{
						uint8_t _r = a[0];
						a[0] = a[2];
						a[2] = _r;
						a += 3;
					}
				}

			}
		}
		else
		{
			throw exception(std::string("not supported bpp ") + std::to_string(bit_count));
		}
	}

	void reader::read_palette(input_stream& stream, size_t colors_in_color_table)
	{
		if (colors_in_color_table > 256)
			colors_in_color_table = 256;

		if (_dib_header->header_type() != dib_header_type::bitmap_core_header)
		{
			stream.read(palette, sizeof(uint32_t), colors_in_color_table);
		}
		else // old bitmap format
		{
			assert(_dib_header->size() == 12);
			struct pixel3
			{
				uint8_t x;	uint8_t y;	uint8_t z;
			};
			
			pixel3 pixel3_palette[256];
			stream.read(pixel3_palette, sizeof(pixel3), colors_in_color_table);

			for (int i = 0; i < colors_in_color_table; ++i)
				palette[i] = (pixel3_palette[i].x) + (pixel3_palette[i].y << 8) + (pixel3_palette[i].z << 16);
		}
	}

}


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

	reader::reader(input_stream& stream)
		: _stream(stream)
	{
		
	}

	reader::reader(input_stream* stream)
		: _stream(*stream)
	{
		
	}

	reader::~reader()
	{
	}

	void reader::read()
	{
		input_stream_handle streamHandle(_stream);

		read_header();
		read_dib_header();

		read_image();
	}


	void reader::read_header()
	{
		_stream.read(&_header, sizeof(main_header), 1);
		if (_header.magic[0] != 'B' || _header.magic[1] != 'M')
		{
			throw exception(std::string("Bad magic number. The file should begin from: BM. Readed: ") + std::string(_header.magic, _header.magic + 2));
		}
	}

	void reader::read_dib_header()
	{
		int32_t dib_header_size;
		_stream.read(&dib_header_size, sizeof(int32_t), 1);
		_dib_header = dib_header::create_header(dib_header_size);
		_stream.read(_dib_header->data(), dib_header_size - sizeof(int32_t), 1);
		
	}

	void reader::read_palette()
	{
		memset(_palette, 0, 256 * sizeof(uint32_t));

		int16_t bit_count = _dib_header->bit_count();
		if (bit_count > 8)
			return;

		int palette_size = (uint8_t)_dib_header->palette_colors();
		if (palette_size == 0)
			palette_size = (1 << bit_count);
		
		if (_dib_header->header_type() != dib_header_type::bitmap_core_header)
		{
			_stream.read(_palette, sizeof(uint32_t), palette_size);
		}
		else // old bitmap format
		{
			assert(_dib_header->size() == 12);
			struct pixel3
			{
				uint8_t x;	uint8_t y;	uint8_t z;
			};

			pixel3 pixel3_palette[256];
			_stream.read(pixel3_palette, sizeof(pixel3), palette_size);

			for (int i = 0; i < palette_size; ++i)
				_palette[i] = (pixel3_palette[i].x) + (pixel3_palette[i].y << 8) + (pixel3_palette[i].z << 16);
		}
	}

	bool reader::is_palette_black_white()
	{
		return _palette[0] == 0 && _palette[1] == 0xFFFFFF;
	}

	void reader::read_1bpp(int width, int height, int row_size, bool flipped)
	{
		_stream.seek(_header.offset);
		std::unique_ptr<uint8_t[]> line_buffer(new uint8_t[row_size]);

		int	row = flipped ? height - 1 : 0;
		const int inc = flipped ? -1 : 1;
		const int end = flipped ? -1 : height - 1;

		if (is_palette_black_white())
		{
			_image.reset(width, height, 1);
			for (; row != end; row += inc)
			{
				int streamPos = 0;
				uint8_t* data = _image.get_row_begin(row);
				const uint8_t* const data_end = data + width;

				_stream.read(line_buffer.get(), sizeof(uint8_t), row_size);

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
		else
		{
			_image.reset(width, height, 3);
			for (; row != end; row += inc)
			{
				uint8_t* data = _image.get_row_begin(row);
				const uint8_t* const data_end = data + width * 3;
				_stream.read(line_buffer.get(), sizeof(uint8_t), row_size);
				int streamPos = 0;
				while (data < data_end)
				{
					uint8_t value = line_buffer[streamPos];
					for (int k = 7; data < data_end && k >= 0; --k)
					{
						int color = _palette[((value >> k) & 1)];
						data[0] = (uint8_t)(color >> 16);
						data[1] = (uint8_t)(color >> 8);
						data[2] = (uint8_t)color;
						data += 3;
					}
					++streamPos;
				}
			}
		}
	}

	void reader::read_4bpp(int width, int height, int row_size, bool flipped)
	{
		_image.reset(width, height, 3);
		_stream.seek(_header.offset);

		std::unique_ptr<uint8_t[]> line_buffer(new uint8_t[row_size]);
		int	row = flipped ? height - 1 : 0;
		const int inc = flipped ? -1 : 1;
		const int end = flipped ? -1 : height - 1;

		for (; row != end; row += inc)
		{
			_stream.read(line_buffer.get(), sizeof(uint8_t), row_size);
			uint8_t* begin = _image.get_row_begin(row);
			const uint8_t* const end = _image.get_row_end(row);
			uint8_t* data = line_buffer.get();
			while (begin != end)
			{
				uint8_t value = *data;
				uint32_t color = _palette[value >> 4];
				begin[0] = (uint8_t)(color >> 16);
				begin[1] = (uint8_t)(color >> 8);
				begin[2] = (uint8_t)color;
				begin += 3;

				if (begin < end)
				{
					uint32_t color = _palette[value & 0xF];
					begin[0] = (uint8_t)(color >> 16);
					begin[1] = (uint8_t)(color >> 8);
					begin[2] = (uint8_t)color;
					begin += 3;
				}
				++data;
			}
		}
	}

	void reader::read_8bpp(int width, int height, int row_size, bool flipped)
	{
		_image.reset(width, height, 3);
		_stream.seek(_header.offset);

		std::unique_ptr<uint8_t[]> line_buffer(new uint8_t[row_size]);
		int	row = flipped ? height - 1 : 0;
		const int inc = flipped	? -1 : 1;
		const int end = flipped ? -1 : height - 1;

		for (; row != end; row += inc)
		{
			_stream.read(line_buffer.get(), sizeof(uint8_t), row_size);
			uint8_t* begin = _image.get_row_begin(row);
			const uint8_t* const end = _image.get_row_end(row);
			uint8_t* data = line_buffer.get();
			while (begin != end)
			{
				uint32_t color = _palette[*data];
				++data;
				begin[0] = (uint8_t)(color >> 16);
				begin[1] = (uint8_t)(color >> 8);
				begin[2] = (uint8_t)color;
				begin += 3;
			}
		}
	}

	void reader::read_24bpp(int width, int height, int row_size, bool flipped)
	{
		_image.reset(width, height, 3, row_size);

		_stream.seek(_header.offset);
		_stream.read(_image.data(), sizeof(uint8_t), row_size * height);

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
	 
	//32bpp not fully work yet
	void reader::read_32bpp(int width, int height, int row_size, bool flipped)
	{
		const int channels = 4;
		_image.reset(width, height, channels);
		_stream.seek(_header.offset);

		std::unique_ptr<uint8_t[]> line_buffer(new uint8_t[row_size]);
		//int	row = flipped ? height - 1 : 0;
		const int inc = flipped ? -1 : 1;
		const int end = flipped ? -1 : height - 1;

		_stream.read(_image.data(), sizeof(uint8_t), row_size * height);
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
					a += channels;
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
					b += channels;
					a[0] = (uint8_t)(bc >> 16);
					a[1] = (uint8_t)(bc >> 8);
					a[2] = (uint8_t)bc;
					a += channels;
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

	void reader::read_image()
	{
		const dib_header& info_header = *_dib_header;
		const int32_t width = info_header.width();
		const int32_t height = abs(info_header.height());
		const int32_t bit_count = info_header.bit_count();
		const bool flipped = info_header.height() > 0;
		const int row_size = ((bit_count * width + 31) / 32) * 4; //padding to 4 bytes

		read_palette();

		if (bit_count == 1)
		{
			read_1bpp(width, height, row_size, flipped);
		}
		else if (bit_count == 4)
		{
			read_4bpp(width, height, row_size, flipped);
		}
		else if ( bit_count == 8)
		{
			read_8bpp(width, height, row_size, flipped);
		}
		else if (bit_count == 24)
		{		
			read_24bpp(width, height, row_size, flipped);
		}
		else if (bit_count == 32)
		{
			read_32bpp(width, height, row_size, flipped);
		}
		else
		{
			throw exception(std::string("not supported bpp ") + std::to_string(bit_count));
		}
	}

}

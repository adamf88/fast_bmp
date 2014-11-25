
#include <memory>
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

		if (_dib_header_size == 40)
			stream.read(&_dib_header, sizeof(dib_header), 1);
		else
			throw exception("not supported dib header");
	}

	void reader::read_image(input_stream& stream)
	{
		int imageSize = _dib_header.imageSize;

		if (imageSize == 0)
			imageSize = _dib_header.width * _dib_header.height;

		const int width = _dib_header.width;
		const bool flipped = _dib_header.height > 0;
		const int height = (flipped ? 1 : -1) * _dib_header.height;


		if (_dib_header.bitCount == 1)
		{
			_image.reset(width, height, 1);

			stream.seek(_header.offset);

			const int row_size = ((_dib_header.width + 31) / 32) * 4; //padding to 4 bytes
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
		else if (_dib_header.bitCount == 24)
		{
			const int rowSize = (((int32_t)_dib_header.bitCount * _dib_header.width + 31) / 32) * 4;
			
			_image.reset(width, height, 3, rowSize);

			stream.seek(_header.offset);
			stream.read(_image.data(), sizeof(uint8_t), rowSize * height);

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
						b[0] = ac >> 16;
						b[1] = ac >> 8;
						b[2] = ac;
						b += 3;
						a[0] = bc >> 16;
						a[1] = bc >> 8;
						a[2] = bc;
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
	}

	void reader::read_palette(input_stream& stream)
	{

	}

}
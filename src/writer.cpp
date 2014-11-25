
#include <vector>
#include "writer.h"
#include "image.h"

namespace fbmp
{

	class output_stream_handle
	{
	public:
		output_stream_handle(output_stream& stream)
			: m_stream(stream)
		{
			m_stream.open_for_writing();
		}

		output_stream_handle(const output_stream_handle&) = delete;
		output_stream_handle& operator=(const output_stream_handle&) = delete;

		~output_stream_handle()
		{
			m_stream.close();
		}

	private:
		output_stream& m_stream;
	};

	struct PixelFormat
	{
		uint16_t r;
		uint16_t g;
		uint16_t b;
		uint16_t a;
	};

	void writer::write(output_stream& stream, main_header _main_header, dib_header _dib_header, const image& _image)
	{
		output_stream_handle handle(stream);

		_dib_header.height = _dib_header.height > 0 ? -_dib_header.height : _dib_header.height;
		int width = _dib_header.width;
		int height = -_dib_header.height;
		const int rowSize = ((width + 31) / 32) * 4;
		const int pixelFormatSize = _dib_header.bitCount == 1 ? 8 : 0;
		_main_header.offset = sizeof(main_header)+sizeof(dib_header)+sizeof(int32_t)+pixelFormatSize; //+ rozmiar
		const int imageSize = rowSize * height + _main_header.offset;
		_main_header.file_size = imageSize;

		int32_t headerSize = sizeof(dib_header)+sizeof(uint32_t);

		stream.write(&_main_header, sizeof(main_header), 1);
		stream.write(&headerSize, sizeof(int32_t), 1);
		stream.write(&_dib_header, sizeof(dib_header), 1);

		if (_dib_header.bitCount == 1)
		{
			PixelFormat f;
			f.r = 0;
			f.g = 0;
			f.b = 0xffff;
			f.a = 0xffff;
			stream.write(&f, sizeof(PixelFormat), 1);

			std::vector<uint8_t> dataToWrite;
			for (int y = height - 1; y >= 0; --y)
			{
				const uint8_t* row = _image.get_row_begin(y);
				const uint8_t* const rowEnd = row + width;
				dataToWrite.resize(0);

				while (row < rowEnd)
				{
					uint8_t value = 0;
					for (int i = 7; i >= 0 && row < rowEnd; --i)
					{
						value |= (*row >> 7) << i;
						++row;
					}
					dataToWrite.push_back(value);
				}
				dataToWrite.resize(rowSize, 0);
				stream.write(&dataToWrite[0], sizeof(uint8_t), dataToWrite.size());
			}
		}
		else if (_dib_header.bitCount == 24)
		{
			image image_copy = _image;
			for (int i = 0; i < height; ++i)
			{
				uint8_t* a = image_copy.get_row_begin(i);
				const uint8_t* const end = image_copy.get_row_end(i);
				while (a < end)
				{
					uint8_t _r = a[0];
					a[0] = a[2];
					a[2] = _r;
					a += 3;
				}
			}
			stream.write(image_copy.data(), sizeof(uint8_t), image_copy.pitch() * image_copy.height());
		}
	}

}

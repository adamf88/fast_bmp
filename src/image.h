
#pragma once
#ifndef FBMP_IMAGE_H
#define FBMP_IMAGE_H

#include <cstdint>
#include <string>

#include "exception.h"

namespace fbmp
{

	class image
	{
	public:

		inline image() = default;
		inline image(size_t width, size_t height, size_t channels);
		inline image(size_t width, size_t height, size_t channels, size_t pitch);
		inline image(size_t width, size_t height, size_t channels, size_t pitch, uint8_t* data);


		inline image(const image& img);
		inline image(image&& img);

		inline ~image();

		inline image& operator=(const image& img);
		image& operator=(image&& img);

		inline void reset(size_t width, size_t height, size_t channels, size_t pitch = 0);
		inline void reset(size_t width, size_t height, size_t channels, size_t pitch, uint8_t* data);

		inline size_t width() const;
		inline size_t height() const;
		inline size_t pitch() const;
		inline size_t channels() const;

		inline const uint8_t* data() const;
		inline uint8_t* data();

		inline uint8_t* get_row_begin(size_t row);
		inline const uint8_t* get_row_begin(size_t row) const;

		inline uint8_t* get_row_end(size_t row);
		inline const uint8_t* get_row_end(size_t row) const;

		inline bool own_data() const;

		inline uint8_t* release();

	private:
		inline void dealloc();

	private:
		size_t _width = 0;
		size_t _height = 0;
		size_t _channels = 0;
		size_t _pitch = 0;

		bool		_ownData = true;
		uint8_t*	_dataPointer = nullptr;
	};

	inline image::image(size_t width, size_t height, size_t channels)
		: image(width, height, channels, 0)
	{}

	inline image::image(size_t width, size_t height, size_t channels, size_t pitch)
	{
		reset(width, height, channels, pitch);
	}

	inline image::image(size_t width, size_t height, size_t channels, size_t pitch, uint8_t* data)
	{
		reset(width, height, channels, pitch, data);
	}

	inline image::image(const image& img)
	{
		reset(img._width, img._height, img._channels, img._pitch);
		std::memcpy(_dataPointer, img._dataPointer, _pitch * _height);
	}

	inline image::image(image&& img)
		: image(img._width, img._height, img._channels, img._pitch, img._dataPointer)
	{
		img.release();
		img.reset(0, 0, 0, 0, nullptr);
	}

	inline image::~image()
	{
		dealloc();
	}

	inline image& image::operator=(const image& img)
	{
		if (this == &img)
			return *this;

		reset(img._width, img._height, img._channels, img._pitch);
		std::memcpy(_dataPointer, img._dataPointer, _pitch * _height);

		return *this;
	}

	inline image& image::operator=(image&& img)
	{
		std::swap(_width, img._width);
		std::swap(_height, img._height);
		std::swap(_channels, img._channels);
		std::swap(_pitch, img._pitch);
		std::swap(_ownData, img._ownData);
		std::swap(_dataPointer, img._dataPointer);

		return *this;
	}

	inline void image::reset(size_t width, size_t height, size_t channels, size_t pitch)
	{
		size_t minRowSize = width * channels;

		if (pitch && minRowSize > pitch)
			throw exception("Pitch is too small.");

		if (!pitch)
			pitch = minRowSize;

		if (_pitch * _height == pitch * height)
			return;

		uint8_t* newData = new uint8_t[pitch * height];
		dealloc();

		_ownData = true;
		_dataPointer = newData;
		_width = width;
		_height = height;
		_channels = channels;
		_pitch = pitch;
	}

	inline void image::reset(size_t width, size_t height, size_t channels, size_t pitch, uint8_t* data)
	{
		dealloc();

		_width = width;
		_height = height;
		_channels = channels;
		_pitch = pitch;
		_dataPointer = data;
		_ownData = false;
	}

	inline size_t image::width() const
	{
		return _width;
	}

	inline size_t image::height() const
	{
		return _height;
	}

	inline size_t image::pitch() const
	{
		return _pitch;
	}

	inline size_t image::channels() const
	{
		return _channels;
	}

	inline const uint8_t* image::data() const
	{
		return image::_dataPointer;
	}

	inline uint8_t* image::data()
	{
		return _dataPointer;
	}

	inline uint8_t* image::release()
	{
		uint8_t* result = _dataPointer;
		_ownData = false;
		_width = 0;
		_height = 0;
		_dataPointer = 0;
		return result;
	}

	inline uint8_t* image::get_row_begin(size_t row)
	{
		return _dataPointer + _pitch * row;
	}

	inline const uint8_t* image::get_row_begin(size_t row) const
	{
		return _dataPointer + _pitch * row;
	}

	inline uint8_t* image::get_row_end(size_t row)
	{
		return _dataPointer + _pitch * row + _width * _channels;
	}

	inline const uint8_t* image::get_row_end(size_t row) const
	{
		return _dataPointer + _pitch * row + _width;
	}

	inline bool image::own_data() const
	{
		return _ownData;
	}

	inline void image::dealloc()
	{
		if (own_data())
			delete[] _dataPointer;

		_dataPointer = nullptr;
		_ownData = true;
		_width = 0;
		_height = 0;
		_pitch = 0;
		_channels = 0;
	}
}

#endif //FBMP_IMAGE_H

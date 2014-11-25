
#pragma once
#ifndef FBMP_STREAM_H
#define FBMP_STREAM_H

namespace fbmp
{

	class input_stream
	{
	public:
		virtual ~input_stream() {}
		virtual void open_for_reading() = 0;
		virtual void close() = 0;
		virtual void read(void* buffer, size_t element_size, size_t count) = 0;
		virtual void seek(int position) = 0;
	};

	class output_stream
	{
	public:
		virtual ~output_stream() {}
		virtual void open_for_writing() = 0;
		virtual void close() = 0;
		virtual void write(const void* buffer, size_t element_size, size_t count) = 0;
	};

}

#endif //FBMP_FILE_STREAM_H
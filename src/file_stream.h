
#pragma once
#ifndef FBMP_FILE_STREAM_H
#define FBMP_FILE_STREAM_H

#include <cstdio>
#include "exception.h"
#include "stream.h"

namespace fbmp
{

	class file_input_stream : public input_stream
	{
	public:
		file_input_stream(const char* fileName)
			: m_file(nullptr)
			, m_fileName(fileName)
		{}

		file_input_stream(file_input_stream&& is)
		{
			std::swap(m_file, is.m_file);
			std::swap(m_fileName, is.m_fileName);
		}

		file_input_stream(const file_input_stream&) = delete;

		~file_input_stream()
		{
			close();
		}

		file_input_stream& operator=(file_input_stream&) = delete;

		file_input_stream& operator=(file_input_stream&& is)
		{
			std::swap(m_file, is.m_file);
			std::swap(m_fileName, is.m_fileName);
			return *this;
		}

		void open_for_reading() override
		{
			if (m_file != nullptr)
			{
				seek(0);
			}
			else
			{
				m_file = fopen(m_fileName.c_str(), "rb");

				if (m_file == nullptr)
				{
					throw exception( std::string("Can not open file: {") + m_fileName + "} for reading.");
				}
			}
		}

		void close() override
		{
			if (m_file != nullptr)
			{
				fclose(m_file);
				m_file = nullptr;
			}
		}

		void read(void* buffer, size_t element_size, size_t count) override
		{
			size_t readed = fread(buffer, element_size, count, m_file);
			if (readed != count)
				throw exception("can not read expected size of data");
		}

		void seek(int pos) override
		{
			fseek(m_file, pos, SEEK_SET);
		}

	private:
		FILE* m_file = nullptr;
		std::string m_fileName = nullptr;
	};

	class file_output_stream : public output_stream
	{
	public:
		file_output_stream()
			: m_file(nullptr)
			, m_fileName(nullptr)
		{}

		file_output_stream(const char* fileName)
			: m_file(nullptr)
			, m_fileName(fileName)
		{
		}

		~file_output_stream()
		{
			close();
		}

		void open_for_writing()
		{
			if (m_file != nullptr)
			{
				close();
			}

			m_file = fopen(m_fileName, "wb");

			if (m_file == nullptr)
			{
				throw exception();
			}
		}

		void close() override
		{
			if (m_file != nullptr)
			{
				fclose(m_file);
				m_file = nullptr;
			}
		}

		void write(const void* buffer, size_t element_size, size_t count) override
		{
			size_t written = fwrite(buffer, element_size, count, m_file);
			if (written != count)
			{
				throw exception("Can not write expected size of data");
			}
		}

	private:
		FILE* m_file;
		const char* m_fileName;
	};

}

#endif //FBMP_FILE_STREAM_H
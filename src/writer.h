
#pragma once
#ifndef FBMP_WRITER_H
#define FBMP_WRITER_H

#include "stream.h"
#include "data_types.h"
#include "image.h"

namespace fbmp
{

	class writer
	{
	public:
		void write(output_stream& stream, main_header& header, const dib_header& dib_header, const image& image);
	};

}

#endif //FBMP_WRITER_H

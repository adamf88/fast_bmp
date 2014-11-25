
#pragma once
#ifndef FBMP_EXCEPTION_H
#define FBMP_EXCEPTION_H

#include <string>

namespace fbmp
{

	class exception : public std::exception
	{
	public:
		exception() = default;

		exception(const char* const& msg)
			: std::exception(msg)
		{}

		exception(const std::string& msg)
			: std::exception(msg.c_str())
		{}
	};

}

#endif //FBMP_EXCEPTION_H

#include "IO.h"
#include <iostream>
#include <fstream>

std::string readFile(const std::string & filename)
{
	std::ifstream inStream(filename);
	if (!inStream.good())
	{
		SAT_DEBUG_LOG_ERROR("[IO.cpp] File not found:\"%s\"\n", filename.c_str());
		return std::string();
	}
	std::string data(std::istreambuf_iterator<char>(inStream), (std::istreambuf_iterator<char>()));
	return data;
}

std::string zeroPadNumber(unsigned int num, unsigned int padding)
{
	std::string ret = std::to_string(num);
	// Append zeros
	for (unsigned int i = 0; i < padding - ret.length(); i++)
	{	ret = "0" + ret;	}
	return ret;
}
#include "hash.hpp"


#define A 54059
#define B 76963 
#define C 86969 
#define FIRSTH 37 
#pragma once


unsigned int calcHash(const char* s)
{
	unsigned h = FIRSTH;
	while (*s) {
		h = (h * A) ^ (s[0] * B);
		s++;
	}
	return h;
}

unsigned int riWHash(const wchar_t* s)
{
	unsigned h = FIRSTH;
	while (true) {
		auto l = *s;
		auto i = *(s + 1);
		if (*s == 0 && (*(s + 1)) == 0)
			break;

		h = (h * A) ^ (s[0] * B);
		s++;
	}
	return h;
}
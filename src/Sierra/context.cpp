#include "Sierra/context.h"
#include <stdlib.h>
#include <string.h>
/*
"uint8",
"uint16",
"uint32",
"uint64",
"int8",
"int16",
"int32",
"int64",
"hwuint8",
"hwuint16",
"hwuint32",
"hwuint64",
"hwint8",
"hwint16",
"hwint32",
"hwint64"
*/

using namespace archivist::sierra;

std::function<char*(char*)> phony_converter_generator(size_t sz)
{
	return [=](char* in)->char* {
		char* ptr=(char*)sierra_allocate(sz);
		memcpy(ptr,in,sz);
		return ptr;
	};
}

std::function<char*(char*)> bswap_converter_generator(size_t sz)
{
	if(sz==1)
	return phony_converter_generator(1);

	if(sz==2)
	return [=](char* in)->char* {
		uint16_t* ptr=(uint16_t*)sierra_allocate(sz);
		*ptr=primitives::ltou16(*(uint16_t*)in);
		return (char*)ptr;
	};
	
	if(sz==4)
	return [=](char* in)->char* {
		uint32_t* ptr=(uint32_t*)sierra_allocate(sz);
		*ptr=primitives::ltou32(*(uint32_t*)in);
		return (char*)ptr;
	};
	
	if(sz==8)
	return [=](char* in)->char* {
		uint64_t* ptr=(uint64_t*)sierra_allocate(sz);
		*ptr=primitives::ltou64(*(uint64_t*)in);
		return (char*)ptr;
	};

	throw std::runtime_error(std::string("BSWAP undefined for given size ")+std::to_string(sz));
}

SierraContext::SierraContext()
{
	{
		SierraType sierra_hwuint8;
		sierra_hwuint8.size=0;
		sierra_hwuint8.name="limiter";
		_types[sierra_hwuint8.name]=sierra_hwuint8;
		SierraField single;
		single.name="self";
		single.converter=phony_converter_generator(sierra_hwuint8.size);
		single.offset=0;
		single.type=&_types[sierra_hwuint8.name];
		_types[sierra_hwuint8.name].fields.push_back(single);
	}
	{
		SierraType sierra_hwuint8;
		sierra_hwuint8.size=1;
		sierra_hwuint8.name="hwuint8";
		_types[sierra_hwuint8.name]=sierra_hwuint8;
		SierraField single;
		single.name="self";
		single.converter=phony_converter_generator(sierra_hwuint8.size);
		single.offset=0;
		single.type=&_types[sierra_hwuint8.name];
		_types[sierra_hwuint8.name].fields.push_back(single);
	}
	{
		SierraType sierra_hwuint;
		sierra_hwuint.size=2;
		sierra_hwuint.name="hwuint16";
		_types[sierra_hwuint.name]=sierra_hwuint;
		SierraField single;
		single.name="self";
		single.converter=phony_converter_generator(sierra_hwuint.size);
		single.offset=0;
		single.type=&_types[sierra_hwuint.name];
		_types[sierra_hwuint.name].fields.push_back(single);
	}
	{
		SierraType sierra_hwuint;
		sierra_hwuint.size=4;
		sierra_hwuint.name="hwuint32";
		_types[sierra_hwuint.name]=sierra_hwuint;
		SierraField single;
		single.name="self";
		single.converter=phony_converter_generator(sierra_hwuint.size);
		single.offset=0;
		single.type=&_types[sierra_hwuint.name];
		_types[sierra_hwuint.name].fields.push_back(single);
	}
	{
		SierraType sierra_hwuint;
		sierra_hwuint.size=8;
		sierra_hwuint.name="hwuint64";
		_types[sierra_hwuint.name]=sierra_hwuint;
		SierraField single;
		single.name="self";
		single.converter=phony_converter_generator(sierra_hwuint.size);
		single.offset=0;
		single.type=&_types[sierra_hwuint.name];
		_types[sierra_hwuint.name].fields.push_back(single);
	}
	{
		SierraType sierra_hwuint8;
		sierra_hwuint8.size=1;
		sierra_hwuint8.name="uint8";
		_types[sierra_hwuint8.name]=sierra_hwuint8;
		SierraField single;
		single.name="self";
		single.converter=bswap_converter_generator(sierra_hwuint8.size);
		single.offset=0;
		single.type=&_types[sierra_hwuint8.name];
		_types[sierra_hwuint8.name].fields.push_back(single);
	}
	{
		SierraType sierra_hwuint;
		sierra_hwuint.size=2;
		sierra_hwuint.name="uint16";
		_types[sierra_hwuint.name]=sierra_hwuint;
		SierraField single;
		single.name="self";
		single.converter=bswap_converter_generator(sierra_hwuint.size);
		single.offset=0;
		single.type=&_types[sierra_hwuint.name];
		_types[sierra_hwuint.name].fields.push_back(single);
	}
	{
		SierraType sierra_hwuint;
		sierra_hwuint.size=4;
		sierra_hwuint.name="uint32";
		_types[sierra_hwuint.name]=sierra_hwuint;
		SierraField single;
		single.name="self";
		single.converter=bswap_converter_generator(sierra_hwuint.size);
		single.offset=0;
		single.type=&_types[sierra_hwuint.name];
		_types[sierra_hwuint.name].fields.push_back(single);
	}
	{
		SierraType sierra_hwuint;
		sierra_hwuint.size=8;
		sierra_hwuint.name="uint64";
		_types[sierra_hwuint.name]=sierra_hwuint;
		SierraField single;
		single.name="self";
		single.converter=bswap_converter_generator(sierra_hwuint.size);
		single.offset=0;
		single.type=&_types[sierra_hwuint.name];
		_types[sierra_hwuint.name].fields.push_back(single);
	}
	/* */
	{
		SierraType sierra_hwuint8;
		sierra_hwuint8.size=1;
		sierra_hwuint8.name="hwint8";
		_types[sierra_hwuint8.name]=sierra_hwuint8;
		SierraField single;
		single.name="self";
		single.converter=phony_converter_generator(sierra_hwuint8.size);
		single.offset=0;
		single.type=&_types[sierra_hwuint8.name];
		_types[sierra_hwuint8.name].fields.push_back(single);
	}
	{
		SierraType sierra_hwuint;
		sierra_hwuint.size=2;
		sierra_hwuint.name="hwint16";
		_types[sierra_hwuint.name]=sierra_hwuint;
		SierraField single;
		single.name="self";
		single.converter=phony_converter_generator(sierra_hwuint.size);
		single.offset=0;
		single.type=&_types[sierra_hwuint.name];
		_types[sierra_hwuint.name].fields.push_back(single);
	}
	{
		SierraType sierra_hwuint;
		sierra_hwuint.size=4;
		sierra_hwuint.name="hwint32";
		_types[sierra_hwuint.name]=sierra_hwuint;
		SierraField single;
		single.name="self";
		single.converter=phony_converter_generator(sierra_hwuint.size);
		single.offset=0;
		single.type=&_types[sierra_hwuint.name];
		_types[sierra_hwuint.name].fields.push_back(single);
	}
	{
		SierraType sierra_hwuint;
		sierra_hwuint.size=8;
		sierra_hwuint.name="hwint64";
		_types[sierra_hwuint.name]=sierra_hwuint;
		SierraField single;
		single.name="self";
		single.converter=phony_converter_generator(sierra_hwuint.size);
		single.offset=0;
		single.type=&_types[sierra_hwuint.name];
		_types[sierra_hwuint.name].fields.push_back(single);
	}
	{
		SierraType sierra_hwuint8;
		sierra_hwuint8.size=1;
		sierra_hwuint8.name="int8";
		_types[sierra_hwuint8.name]=sierra_hwuint8;
		SierraField single;
		single.name="self";
		single.converter=bswap_converter_generator(sierra_hwuint8.size);
		single.offset=0;
		single.type=&_types[sierra_hwuint8.name];
		_types[sierra_hwuint8.name].fields.push_back(single);
	}
	{
		SierraType sierra_hwuint;
		sierra_hwuint.size=2;
		sierra_hwuint.name="int16";
		_types[sierra_hwuint.name]=sierra_hwuint;
		SierraField single;
		single.name="self";
		single.converter=bswap_converter_generator(sierra_hwuint.size);
		single.offset=0;
		single.type=&_types[sierra_hwuint.name];
		_types[sierra_hwuint.name].fields.push_back(single);
	}
	{
		SierraType sierra_hwuint;
		sierra_hwuint.size=4;
		sierra_hwuint.name="int32";
		_types[sierra_hwuint.name]=sierra_hwuint;
		SierraField single;
		single.name="self";
		single.converter=bswap_converter_generator(sierra_hwuint.size);
		single.offset=0;
		single.type=&_types[sierra_hwuint.name];
		_types[sierra_hwuint.name].fields.push_back(single);
	}
	{
		SierraType sierra_hwuint;
		sierra_hwuint.size=8;
		sierra_hwuint.name="int64";
		_types[sierra_hwuint.name]=sierra_hwuint;
		SierraField single;
		single.name="self";
		single.converter=bswap_converter_generator(sierra_hwuint.size);
		single.offset=0;
		single.type=&_types[sierra_hwuint.name];
		_types[sierra_hwuint.name].fields.push_back(single);
	}
}


std::function<void*(size_t)> 		archivist::sierra::sierra_allocate=malloc;
std::function<void*(void*,size_t)> 	archivist::sierra::sierra_reallocate=realloc;
std::function<void(void*)> 			archivist::sierra::sierra_free=free;
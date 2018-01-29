#pragma once
#include <Sierra/context.h>
#include "Sierra/tokenizer.h"

namespace archivist
{
	namespace sierra
	{
		class token_expected_exception{
		public:
			SierraToken previous;
		};

		class parsing_context{
		public:
			SierraTokenString data;
			size_t index;
			SierraToken next()
			{
				if(index++>=data.size())
				{
					throw token_expected_exception({data[index-1]});
				}
				return data[index];
			}
		};
	}
}
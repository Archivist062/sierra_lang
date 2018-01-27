#pragma once
#include "Sierra/tokenizer.h"
#include "Sierra/context.h"

namespace archivist
{
	namespace sierra
	{
		void parse_tokens(archivist::sierra::SierraContext& ctx, archivist::sierra::SierraTokenString data);

		inline void parse_string(archivist::sierra::SierraContext& ctx, std::string code)
		{
			auto tks = archivist::sierra::tokenize_string(code);
			parse_tokens(ctx,tks);
		}
		
		void js_parse(const CFunctionsScopePtr &c, void *userdata);
	}
}
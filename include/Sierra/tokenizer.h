#pragma once
#include <string>
#include <utility>
#include <vector>
#include "TinyJS/TinyJS.h"
#include <sstream>

namespace archivist{
	namespace sierra{

		enum tk_type{
			TK_keyword,
			TK_operator,
			TK_symbol,
			TK_integer,
			TK_limit,
			TK_statement_end,
			TK_binfield
		};

		typedef std::pair<tk_type,std::string> deprecated_SierraToken;

		struct SierraToken{
			tk_type type;
			std::string token;
			size_t line;
		};

		typedef std::vector<SierraToken> SierraTokenString;

		SierraTokenString tokenize_string(std::string);

		void js_token(const CFunctionsScopePtr &c, void *userdata);
	}
}
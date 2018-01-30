#pragma once
#include "Sierra/context.h"
#include "Sierra/parser_utils.h"

namespace archivist
{
	namespace sierra
	{
		namespace parsers
		{
			SierraField parse_field(SierraContext& ctx, parsing_context& parsectx,const std::vector<SierraField>& prev_fields);
			void parse_type(SierraContext& ctx, parsing_context& parsectx);
		}
	}
}
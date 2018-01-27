#include "Sierra/parser.h"
#include <iostream>
#include <exception>

using namespace archivist::sierra;

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

SierraField parse_field(SierraContext& ctx, parsing_context& parsectx)
{
	SierraToken offset,type,name,defaultval;
	
}

void parse_type(SierraContext& ctx, parsing_context& parsectx)
{
	SierraToken name,tmp;
	std::vector<SierraField> fields;
	try{
		name=parsectx.next();
		if(name.type!=tk_type::TK_symbol)
		{
			throw std::runtime_error(
				std::string("Expected symbol name, got")
				+std::string(":")
				+name.token
			);
		}
	}catch(token_expected_exception& e)
	{
		parsectx.index--;
		throw std::runtime_error(
			std::string("Expected symbol name but none was provided")
		);
	}
	try{
		tmp=parsectx.next();
		if(tmp.type!=tk_type::TK_limit || tmp.token!="{")
		{
			throw std::runtime_error(
				std::string("Expected '{' name, got")
				+std::string(":")
				+name.token
			);
		}
	}catch(token_expected_exception& e)
	{
		parsectx.index--;
		throw std::runtime_error(
			std::string("Met EOF while expecting '{'")
		);
	}

	try{
		tmp=parsectx.next();
	}catch(token_expected_exception& e)
	{
		parsectx.index--;
		throw std::runtime_error(
			std::string("Met EOF while expecting a valid field")
		);
	}
	while(tmp.type!=tk_type::TK_limit || tmp.token!="}")
	{
		fields.push_back(parse_field(ctx,parsectx));
		
		try{
			tmp=parsectx.next();
		}catch(token_expected_exception& e)
		{
			parsectx.index--;
			throw std::runtime_error(
				std::string("Met EOF while expecting a valid field or a '}'")
			);
		}
	}
	SierraType type = {name.token,fields,0};
	ctx._types[type.name]=type;
}

void archivist::sierra::parse_tokens(SierraContext& ctx, SierraTokenString data)
{
	parsing_context parsectx={data,0};
	try{
		while(parsectx.index<parsectx.data.size())
		{
			if(parsectx.data[parsectx.index].type==tk_type::TK_keyword)
			{
				if(parsectx.data[parsectx.index].token=="type")
					parse_type(ctx,parsectx);
				else
					throw std::runtime_error(
						std::string("Unexpected keyword")
						+std::string(":")
						+parsectx.data[parsectx.index].token
					);
			}else{
				throw std::runtime_error(
					std::string("Unexpected token")
					+std::string(":")
					+parsectx.data[parsectx.index].token
				);
			}
		}
	}
	catch (const std::runtime_error& ex)
	{
		std::cerr<<"Parsing error at line "<<parsectx.data[parsectx.index].line<<":\n\t"<<ex.what()<<std::endl;
	}
}
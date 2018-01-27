#include "Sierra/parser.h"
#include <iostream>
#include <exception>
#include <algorithm>
#include <cctype>

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
	SierraField ret;
	try{
		offset=parsectx.next();
		if(offset.type!=tk_type::TK_integer)
		{
			throw std::runtime_error(
				std::string("Expected integer, got")
				+std::string(":")
				+offset.token
			);
		}
		type=parsectx.next();
		if(type.type!=tk_type::TK_symbol)
		{
			throw std::runtime_error(
				std::string("Expected symbol, got")
				+std::string(":")
				+type.token
			);
		}
		auto it_type = ctx._types.find(type.token);
		if(it_type==ctx._types.end())
		{
			throw std::runtime_error(
				std::string("undefined type")
				+std::string(":")
				+type.token
			);
		}
		name=parsectx.next();
		if(name.type!=tk_type::TK_symbol)
		{
			throw std::runtime_error(
				std::string("Expected symbol, got")
				+std::string(":")
				+name.token
			);
		}

		auto tmp=parsectx.next();
		if(tmp.type!=tk_type::TK_statement_end || tmp.token!=";")
		{
			throw std::runtime_error(
				std::string("Expected ';', got")
				+std::string(":")
				+tmp.token
			);
		}

		ret.name=name.token;
		ret.type=&(*it_type).second;
		char* dummy=(char*)offset.token.c_str()+offset.token.size();
		ret.offset=strtoll(offset.token.c_str(),&dummy,10);
	}catch(token_expected_exception& e)
	{
		parsectx.index--;
		throw std::runtime_error(
			std::string("Met EOF during declaration")
		);
	}
	return ret;
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
				+tmp.token
			);
		}
	}catch(token_expected_exception& e)
	{
		parsectx.index--;
		throw std::runtime_error(
			std::string("Met EOF while expecting '{'")
		);
	}

	while(tmp.type!=tk_type::TK_limit || tmp.token!="}")
	{
		fields.push_back(parse_field(ctx,parsectx));
		
		try{
			tmp=parsectx.next();
			parsectx.index--;
		}catch(token_expected_exception& e)
		{
			throw std::runtime_error(
				std::string("Met EOF while expecting a valid field or a '}'")
			);
		}
	}
	parsectx.index+=2;
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








SierraContext js_ctx;

void archivist::sierra::js_parse(const CFunctionsScopePtr &c, void *userdata) {
	std::string code = c->getArgument("code")->toString();
	std::stringstream val;
	parse_string(js_ctx,code);
	for(auto type : js_ctx._types)
	{
		std::cout<<type.second.name<<":"<<type.second.size<<std::endl;
		for(auto field : type.second.fields)
		{
			std::cout<<"\t"<<field.type->name<<" "<<field.offset<<":"<<field.name<<std::endl;
		}
	}
}
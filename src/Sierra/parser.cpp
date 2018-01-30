#include "Sierra/parser.h"
#include "Sierra/parser_utils.h"
#include "Sierra/SubParsers/type_parser.h"
#include <iostream>
#include <fstream>
#include <exception>
#include <algorithm>
#include <cctype>
#include "Sierra/SubParsers/type_parser.h"

using namespace archivist::sierra;


void archivist::sierra::parse_tokens(SierraContext& ctx, SierraTokenString data)
{
	parsing_context parsectx={data,0};
	try{
		while(parsectx.index<parsectx.data.size())
		{
			if(parsectx.data[parsectx.index].type==tk_type::TK_keyword)
			{
				if(parsectx.data[parsectx.index].token=="type")
					archivist::sierra::parsers::parse_type(ctx,parsectx);
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

void archivist::sierra::js_parsef(const CFunctionsScopePtr &c, void *userdata) {
	std::string file = c->getArgument("filename")->toString();
	std::string code;
	std::stringstream val;
	std::ifstream fop(file);
	while(fop.good())
		code+=fop.get();
	parse_string(js_ctx,code);
	for(auto type : js_ctx._types)
	{
		std::cout<<type.second.name<<":"<<type.second.size<<std::endl;
		for(auto field : type.second.fields)
		{
			std::cout<<"\t"<<field.type->name<<" "<<field.offset<<":"<<field.name;
			if(field.repeated)
			{
				std::cout<<"*"<<field.repeat_size->name;
			}
			std::cout<<std::endl;
		}
	}
}
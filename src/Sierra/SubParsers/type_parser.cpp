#include <string>
#include "Sierra/SubParsers/type_parser.h"

using namespace archivist::sierra;
using namespace parsers;


/**
 * @brief parse and return a field
 * 
 * @param ctx 
 * @param parsectx 
 * @param prev_fields 
 * @return SierraField 
 */
SierraField archivist::sierra::parsers::parse_field(SierraContext& ctx, parsing_context& parsectx,const std::vector<std::shared_ptr<SierraField> >& prev_fields, size_t align)
{
	SierraToken offset,type,name,defaultval,repeat_field;
	bool repeated=false;
	SierraField ret;
	try{
		offset=parsectx.next();
		if(offset.type==tk_type::TK_integer)
		{
			char* dummy=(char*)offset.token.c_str()+offset.token.size();
			ret.offset=strtoll(offset.token.c_str(),&dummy,10);
		}
		else if(offset.type==tk_type::TK_keyword && offset.token=="auto")
		{
			size_t max_offset=0;
			for(auto field : prev_fields)
			{
				max_offset=std::max(field->offset+field->type->size,max_offset);
			}
			ret.offset=max_offset+max_offset%align;
		}
		else
		{
			throw std::runtime_error(
				std::string("Expected integer, got")
				+std::string(":")
				+offset.token
			);
		}
		type=parsectx.next();
		auto it_type = ctx._types.begin();
		if(type.type==tk_type::TK_symbol)
		{
			it_type = ctx._types.find(type.token);
			if(it_type==ctx._types.end())
			{
				throw std::runtime_error(
					std::string("undefined type")
					+std::string(":")
					+type.token
				);
			}
		}else if(type.type==tk_type::TK_keyword && type.token=="bin"){
			auto open=parsectx.next();
			if(open.type!=tk_type::TK_limit || open.token!="(")
			{
				throw std::runtime_error(
					std::string("Expected '(', got")
					+std::string(":")
					+open.token
				);
			}

			auto field_size=parsectx.next();
			if(field_size.type!=tk_type::TK_integer)
			{
				throw std::runtime_error(
					std::string("Expected integer, got")
					+std::string(":")
					+field_size.token
				);
			}

			auto close=parsectx.next();
			if(close.type!=tk_type::TK_limit || close.token!=")")
			{
				throw std::runtime_error(
					std::string("Expected ')', got")
					+std::string(":")
					+close.token
				);
			}

			std::string binfield_name=std::string("bin(")+field_size.token+")";
			it_type = ctx._types.find(binfield_name);
			if(it_type==ctx._types.end())
			{
				char* dummy=(char*)field_size.token.c_str()+field_size.token.size();
				SierraType sierra_hwuint8;
				sierra_hwuint8.size=strtoll(field_size.token.c_str(),&dummy,10);
				sierra_hwuint8.name=binfield_name;
				ctx._types[sierra_hwuint8.name]=std::make_shared<SierraType>(sierra_hwuint8);
				SierraField single;
				single.name="self";
				single.converter=[=](char* in)->char* {
					char* ptr=(char*)sierra_allocate(sierra_hwuint8.size);
					memcpy(ptr,in,sierra_hwuint8.size);
					return ptr;
				};
				single.offset=0;
				single.type=ctx._types[sierra_hwuint8.name];
				ctx._types[sierra_hwuint8.name]->fields.push_back(std::make_shared<SierraField>(single));
				it_type = ctx._types.find(binfield_name);
			}

		}else{
			throw std::runtime_error(
				std::string("Expected symbol, got")
				+std::string(":")
				+type.token
			);
		}

		{
			auto tmp=parsectx.next();
			if(tmp.type==tk_type::TK_operator && tmp.token=="*")
			{
				repeat_field=parsectx.next();
				repeated=true;
				if(repeat_field.type!=tk_type::TK_symbol)
				{
					throw std::runtime_error(
						std::string("Expected symbol as repeat field size, got")
						+std::string(":")
						+repeat_field.token
					);
				}
				ret.repeat_size=nullptr;
				for(size_t rep=0;rep<prev_fields.size();rep++)
					if(prev_fields[rep]->name==repeat_field.token)
					{
						ret.repeat_size=prev_fields[rep];
						break;
					}
				
				if(ret.repeat_size==nullptr)
				{
					throw std::runtime_error(
						std::string("Expected symbol as repeat field size, got")
						+std::string(":")
						+repeat_field.token
						+std::string(" which is not valid field")
					);
				}
				name=parsectx.next();
			}
			else
			{
				name=tmp;
			}
		}

		if(name.type!=tk_type::TK_symbol)
		{
			throw std::runtime_error(
				std::string("Expected symbol, got")
				+std::string(":")
				+name.token
			);
		}
		for(auto rep : prev_fields)
		if(rep->name==name.token)
		{
			throw std::runtime_error(
				std::string("Field")
				+std::string(":")
				+name.token
				+std::string(" already defined")
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
		ret.type=(*it_type).second;
		ret.repeated=repeated;
	}catch(token_expected_exception& e)
	{
		parsectx.index--;
		throw std::runtime_error(
			std::string("Met EOF during declaration")
		);
	}
	return ret;
}


/**
 * @brief Parse a type from a parsing context into a SierraContext
 * 
 * @param ctx Context where data is parsed in
 * @param parsectx Context data is parsed from
 */
void archivist::sierra::parsers::parse_type(SierraContext& ctx, parsing_context& parsectx)
{
	SierraToken name,tmp;
	SierraType type;
	size_t align=1,finish=1;
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
		
		for(
			tmp=parsectx.next();
			tmp.type!=tk_type::TK_limit && tmp.token!="{";
			tmp=parsectx.next()
		)
		{
			if(tmp.type==tk_type::TK_keyword && tmp.token=="align")
			{
				auto open=parsectx.next();
				if(open.type!=tk_type::TK_limit || open.token!="(")
				{
					throw std::runtime_error(
						std::string("Expected '(', got")
						+std::string(":")
						+open.token
					);
				}
	
				auto field_size=parsectx.next();
				if(field_size.type!=tk_type::TK_integer)
				{
					throw std::runtime_error(
						std::string("Expected integer, got")
						+std::string(":")
						+field_size.token
					);
				}
	
				auto close=parsectx.next();
				if(close.type!=tk_type::TK_limit || close.token!=")")
				{
					throw std::runtime_error(
						std::string("Expected ')', got")
						+std::string(":")
						+close.token
					);
				}
				char* dummy=(char*)field_size.token.c_str()+field_size.token.size();
				align=strtoll(field_size.token.c_str(),&dummy,10);
				if(align<=0)
				{
					throw std::runtime_error(
						std::string("Expected non null non negative alignment, got")
						+std::string(":")
						+std::to_string(align)
					);
				}
			}
			else if(tmp.type==tk_type::TK_keyword && tmp.token=="finish")
			{
				auto open=parsectx.next();
				if(open.type!=tk_type::TK_limit || open.token!="(")
				{
					throw std::runtime_error(
						std::string("Expected '(', got")
						+std::string(":")
						+open.token
					);
				}
	
				auto field_size=parsectx.next();
				if(field_size.type!=tk_type::TK_integer)
				{
					throw std::runtime_error(
						std::string("Expected integer, got")
						+std::string(":")
						+field_size.token
					);
				}
	
				auto close=parsectx.next();
				if(close.type!=tk_type::TK_limit || close.token!=")")
				{
					throw std::runtime_error(
						std::string("Expected ')', got")
						+std::string(":")
						+close.token
					);
				}
				char* dummy=(char*)field_size.token.c_str()+field_size.token.size();
				finish=strtoll(field_size.token.c_str(),&dummy,10);
				if(finish<=0)
				{
					throw std::runtime_error(
						std::string("Expected non null non negative finish alignment, got")
						+std::string(":")
						+std::to_string(finish)
					);
				}
				
			}
			else
			{
				throw std::runtime_error(
					std::string("Expected '{' or option, got")
					+std::string(":")
					+tmp.token
				);
			}
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
		type.fields.push_back(
			std::make_shared<SierraField>(
				archivist::sierra::parsers::parse_field(ctx,parsectx,type.fields,align)
			)
		);
		
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
	size_t max_offset=0;
	bool dynamic=false;
	for(auto n : type.fields)
	{
		max_offset=std::max(n->offset+n->type->size,max_offset)+std::max(n->offset+n->type->size,max_offset)%finish;
		dynamic=dynamic||n->repeated||n->type->dynamic;
	}
	type.name=name.token;
	type.size=max_offset;
	type.dynamic=dynamic;
	type.alignment=align;
	type.final_alignment=finish;
	ctx._types[type.name]=std::make_shared<SierraType>(type);
}
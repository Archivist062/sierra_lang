#include <Sierra/tokenizer.h>
#include <experimental/string_view>
#include <sstream>
#include <iostream>
#include <set>

namespace std{
	using string_view = std::experimental::string_view;
	using wstring_view = std::experimental::wstring_view;
}

using namespace archivist::sierra;

struct tokenizing_state{
	std::string_view code;
	size_t position;
	size_t line;
};

std::set<char> LWS_set={'\n','\t',' '};
std::set<char> limits_set={'{','}','(',')','[',']'};
std::set<std::string> keywords_set={
	"type",
	"bin",
	"auto"
};
std::string statement_end(";");
std::string assignment_operator("=");


void skip_lws(tokenizing_state& state)
{
	while(LWS_set.find(state.code[state.position])!=LWS_set.end())
	{
		if(state.code[state.position]=='\n')
			state.line++;
		if(state.position+1>=state.code.length())
			break;
		state.position++;
	}
}


void skip_line(tokenizing_state& state)
{
	while(state.code[state.position]!='\n')
	{
		if(state.position+1>=state.code.length())
			break;
		state.position++;
	}
	if(state.code[state.position]=='\n')
		state.line++;
}

void parse_delegator(SierraTokenString& target,tokenizing_state& state)
{
	std::stringstream sstr;
	if(isalpha(state.code[state.position]))
	{
		sstr<<state.code[state.position];
		if(state.position+1>=state.code.length())
			goto build;
		state.position++;
		while(
			isalnum(state.code[state.position])
			|| state.code[state.position]=='_'
		)
		{
			sstr<<state.code[state.position];
			if(state.position+1>=state.code.length())
				break;
			state.position++;
		}
		goto build;
	}
	else
	{
		throw std::runtime_error(std::string("Unexpected character:")+state.code[state.position]);
	}
	build:
	if(keywords_set.find(sstr.str())!=keywords_set.end())
	{
		target.push_back({tk_type::TK_keyword,sstr.str(),state.line});
	}
	else
	{
		target.push_back({tk_type::TK_symbol,sstr.str(),state.line});
	}
}

SierraTokenString archivist::sierra::tokenize_string(std::string code)
{
	SierraTokenString ret;
	tokenizing_state state = {std::string_view(code.c_str(),code.length()),0,1};
	try
	{
		skip_lws(state);
		while(state.position<code.length())
		{
			switch(state.code[state.position]){
				case '{':
				case '}':
				case '(':
				case ')':
				case '[':
				case ']':
				{
					std::stringstream sstr;
					sstr<<state.code[state.position];
					ret.push_back({tk_type::TK_limit,sstr.str(),state.line});
					state.position++;
					break;
				}
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				{
					std::stringstream sstr;
					while(state.code[state.position]>='0' && state.code[state.position]<='9')
					{
						sstr<<state.code[state.position];
						if(state.position+1>=state.code.length())
							break;
						state.position++;
					}
					ret.push_back({tk_type::TK_integer,sstr.str(),state.line});
					break;
				}
				case ';':
				{
					ret.push_back({tk_type::TK_statement_end,statement_end,state.line});
					state.position++;
					break;
				}
				case '=':
				{
					ret.push_back({tk_type::TK_operator,assignment_operator,state.line});
					state.position++;
					break;
				}
				case '*':
				{
					ret.push_back({tk_type::TK_operator,"*",state.line});
					state.position++;
					break;
				}
				case '#':
				{
					skip_line(state);
					state.position++;
					break;
				}
				default:
				{
					parse_delegator(ret,state);
				}
			}
			skip_lws(state);
		}
	}
	catch (const std::runtime_error& ex)
	{
		std::cerr<<"Tokenization error at line "<<state.line<<":\n\t"<<ex.what()<<std::endl;
	}

	return ret;
}









void archivist::sierra::js_token(const CFunctionsScopePtr &c, void *userdata) {
	std::string code = c->getArgument("code")->toString();
	std::stringstream val;
	auto n = tokenize_string(code);
	for(auto v : n)
		val<<v.token<<"|";
	
	c->setReturnVar(c->newScriptVar(val.str()));
}
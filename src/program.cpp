#include "TinyJS/TinyJS.h"
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include "Sierra/tokenizer.h"
#include "Sierra/parser.h"


void js_print(const CFunctionsScopePtr &v, void *) {
	printf("> %s\n", v->getArgument("text")->toString().c_str());
}

void js_dump(const CFunctionsScopePtr &v, void *) {
	v->getContext()->getRoot()->trace(">  ");
}


char *topOfStack;
#define sizeOfStack 1*1024*1024 /* for example 1 MB depend of Compiler-Options */
#define sizeOfSafeStack 50*1024 /* safety area */

int main(int , char **)
{
	char dummy;
	topOfStack = &dummy;
	CTinyJS *js = new CTinyJS();
	js->addNative("function sierra_tok(code)", &archivist::sierra::js_token, 0);
	js->addNative("function sierra_parse(code)", &archivist::sierra::js_parse, 0);
	js->addNative("function sierra_fparse(filename)", &archivist::sierra::js_parsef, 0);
	js->addNative("function print(text)", &js_print, 0);
	js->addNative("function dump()", &js_dump, js);
	/* Execute out bit of code - we could call 'evaluate' here if
		we wanted something returned */
	js->setStackBase(topOfStack-(sizeOfStack-sizeOfSafeStack));
	try {
		js->execute("var lets_quit = 0; function quit() { lets_quit = 1; }");
		js->execute("print(\"Interactive mode... Type quit(); to exit, or print(...); to print something, or dump() to dump the symbol table!\");");
	} catch (CScriptException *e) {
		printf("%s\n", e->toString().c_str());
		delete e;
	}
	int lineNumber = 0;
	while (js->evaluate("lets_quit") == "0") {
		std::string buffer;
		if(!std::getline(std::cin, buffer)) break;
		try {
			js->execute(buffer, "console.input", lineNumber++);
		} catch (CScriptException *e) {
			printf("%s\n", e->toString().c_str());
			delete e;
		}
	}
	delete js;
	return 0;
}
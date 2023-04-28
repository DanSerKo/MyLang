#include "LexAnalyzer.h"
#include "SemanticAnalyzer.h"
#include "Comp.h"

TID Tree;
ControlTypes Control;
TableToFunc FuncControl;

LexAnalyzer Geter;
Lexeme NowLexeme;

void nxt() {
	if (Geter.Empty())
		throw std::string("Lexemes not finish");
	NowLexeme = Geter.GetLexem();
}

bool isType(std::string name) {
	std::vector <std::string> Types = { "int", "int64", "float", "char", "bool" };
	return std::count(Types.begin(), Types.end(), name);
}

bool isMass(std::string name) {
	std::vector <std::string> Types = { "string" };
	return std::count(Types.begin(), Types.end(), name);
}

void Error() {
	throw std::string("Error in line ") + std::to_string(NowLexeme.x + 1);
}

void Start(); 
void inGlobal();
void Func();
void Arguments(); 
void Declare(); 
void While();
void If();
void Else();
void For();
void Assign();
void Cin();
void Cout(); 
void Return();
std::string Variable(); 
void String();
void Data(); 
void Object(); 
void Expression(); 
void Expression2();
void Expression3();
void Expression4();
void Expression5();
void Expression6();

void Block(bool flag = 1) {
	if (flag)
		Tree.CreateTID();
	if (NowLexeme.name != "{")
		Error();
	nxt();
	while (NowLexeme.name != "}") {
		if (NowLexeme.name == "while") {
			While();
			nxt();
			continue;
		}
		if (NowLexeme.name == "for") {
			For();
			nxt();
			continue;
		}
		if (NowLexeme.name == "if") {
			If();
			continue;
		}
		if (NowLexeme.name == "cin")
			Cin();
		else if (NowLexeme.name == "cout")
			Cout();
		else if (NowLexeme.name == "return")
			Return();
		else if (NowLexeme.type == types::KEYWORD)
			Declare();
		else
			Assign();
		if (NowLexeme.name != ";")
			Error();
		nxt();
	}
	if (flag)
		Tree.UpTree();
}


void Data() {
	if (NowLexeme.type != types::IDENTIFIER)
		Error();
	auto now = NowLexeme;
	nxt();
	if (NowLexeme.name == "(") {
		nxt();
		std::vector <std::string> param;
		while (NowLexeme.name != ")") {
			Expression();
			param.push_back(Control.GetType());
			if (NowLexeme.name == ")")
				continue;
			if (NowLexeme.name != ",")
				Error();
			nxt();
		}
		nxt();
		if (param != FuncControl.GetParFunc(now.name).ArgumType) {
			throw std::string("invalid function arguments in line ") + std::to_string(now.x + 1);
		}
		Control.Push(FuncControl.GetParFunc(now.name).MyType);
		return;
	}
	int k = 0;
	while (NowLexeme.name == "[") {
		k++;
		nxt();
		Expression();
		std::string Get = Control.GetType();
		if (Get != "int" && Get != "int64") {
			Error();
		}
		if (NowLexeme.name == "]") {
			nxt();
			continue;
		}
		Error();
	}
	Control.Push(Tree.GetType(now.name).type);
	if (k != Tree.GetType(now.name).Dim)
		throw std::string("incorrect dimensions in line ") + std::to_string(now.x + 1);
}

void String() {
	if (NowLexeme.name == "'") {
		nxt();
		if (NowLexeme.name.size() != 1)
			Error();
		nxt();
		if (NowLexeme.name != "'")
			Error();
		nxt();
		Control.Push("char");
		return;
	}
	if (NowLexeme.name != "\"")
		Error();
	nxt();
	while (NowLexeme.name != "\"")
		nxt();
	nxt();
	Control.Push("string");
}

void Object() {
	if (NowLexeme.type == types::NUMBER) {
		if (count(NowLexeme.name.begin(), NowLexeme.name.end(), '.')) {
			Control.Push("float");
		}
		else
			Control.Push("int");
		nxt();
		return;
	}
	if (NowLexeme.type == types::IDENTIFIER) {
		Data();
		return;
	}
	if (NowLexeme.name == "\"" || NowLexeme.name == "'") {
		String();
		return;
	}
	Error();
}

std::string Variable() {
	if (NowLexeme.type != types::IDENTIFIER)
		Error();
	std::string NowType = Tree.GetType(NowLexeme.name).type;
	auto now = NowLexeme;
	nxt();
	int k = 0;
	while (NowLexeme.name == "[") {
		k++;
		nxt();
		Expression();
		std::string Get = Control.GetType();
		if (Get != "int" && Get != "int64") {
			Error();
		}
		if (NowLexeme.name == "]") {
			nxt();
			continue;
		}
		Error();
	}
	if (k != Tree.GetType(now.name).Dim)
		throw std::string("incorrect dimensions in line ") + std::to_string(now.x + 1);
	return NowType;
}

void Return() {
	if (NowLexeme.name != "return")
		Error();
	nxt();
	Expression();
	if (Control.GetType() != FuncControl.GetParFunc(FuncControl.GetFunc()).MyType)
		throw std::string("incorrect type in line ") + std::to_string(NowLexeme.x + 1);		
}

void Cout() {
	if (NowLexeme.name != "cout")
		Error();
	nxt();
	if (NowLexeme.name != "(")
		Error();
	nxt();
	Expression();
	if (NowLexeme.name != ")")
		Error();
	nxt();
}

void Cin() {
	if (NowLexeme.name != "cin")
		Error();
	nxt();
	if (NowLexeme.name != "(")
		Error();
	nxt();
	Variable();
	if (NowLexeme.name != ")")
		Error();
	nxt();
}

void Assign() {
	auto NowType = Variable();
	if (NowLexeme.name == "(") {
		nxt();
		while (NowLexeme.name != ")") {
			Expression();
			if (NowLexeme.name == ")")
				continue;
			if (NowLexeme.name != ",")
				Error();
			nxt();
		}
		nxt();
		return;
	}
	if (NowLexeme.name != "=")
		Error();
	nxt();
	Expression(); 
	std::string Type = Control.GetType();
	if (Type != NowType) {
		if ((NowType == "int" || NowType == "int64" || NowType == "float") 
			&& (Type == "int" || Type == "int64" || Type == "float"))
			return;
		Error();
	}
}

void While() {
	if (NowLexeme.name != "while")
		Error();
	nxt();
	if (NowLexeme.name != "(")
		Error();
	nxt();
	Expression();
	if (Control.GetType() != "bool")
		Error();
	if (NowLexeme.name != ")")
		Error();
	nxt();
	Block();
}

void Else() {
	if (NowLexeme.name != "else")
		Error();
	nxt();
	Block();
}

void If() {
	if (NowLexeme.name != "if")
		Error();
	nxt();
	if (NowLexeme.name != "(")
		Error();
	nxt();
	Expression();
	if (Control.GetType() != "bool")
		Error();
	if (NowLexeme.name != ")")
		Error();
	nxt();
	Block();
	nxt();
	if (NowLexeme.name == "else") {
		Else();
		nxt();
	}
}

void For() {
	if (NowLexeme.name != "for")
		Error();
	nxt();
	if (NowLexeme.name != "(")
		Error();
	nxt();
	Assign();
	if (NowLexeme.name != ";")
		Error();
	nxt();
	Expression();
	if (Control.GetType() != "bool")
		Error();
	if (NowLexeme.name != ";")
		Error();
	nxt();
	Assign();
	if (NowLexeme.name != ")")
		Error();
	nxt();
	Block();
}

void Expression6() {
	if (NowLexeme.name != "(")
		Object();
	else {
		nxt();
		Expression();
		nxt();
	}
}

void Expression5() {
	Expression6();
	while (NowLexeme.name == "*" || NowLexeme.name == "%" || NowLexeme.name == "/") {
		auto Now = NowLexeme;
		nxt();
		Expression6();
		Control.Update(Now);
	}
}

void Expression4() {
	Expression5();
	while (NowLexeme.name == "+" || NowLexeme.name == "-") {
		auto Now = NowLexeme;
		nxt();
		Expression5();
		Control.Update(Now);
	}
}

void Expression3() {
	Expression4();
	while (NowLexeme.name == "^" || NowLexeme.name == "&" || NowLexeme.name == "|") {
		auto Now = NowLexeme;
		nxt();
		Expression4();
		Control.Update(Now);
	}
}

void Expression2() {
	Expression3();
	while (NowLexeme.name == "==" || NowLexeme.name == ">" || NowLexeme.name == "<" || NowLexeme.name == "!=") {
		auto Now = NowLexeme;
		nxt();
		Expression3();
		Control.Update(Now);
	}
}

void Expression() {
	Expression2();
	while (NowLexeme.name == "and" || NowLexeme.name == "or") {
		auto Now = NowLexeme;
		nxt();
		Expression2();
		Control.Update(Now);
	}
}

void Declare() { 
	std::string type;
	int k = 0;
	std::string name;
	if (NowLexeme.type == types::KEYWORD) { 
		if (isType(NowLexeme.name) || isMass(NowLexeme.name)) {
			type = NowLexeme.name;
			nxt();
			if (NowLexeme.type == types::IDENTIFIER) {
				name = NowLexeme.name;
				nxt();
				while (NowLexeme.name == "[") {
					k++;
					nxt();
					if (NowLexeme.type == types::NUMBER) {
						nxt();
						if (NowLexeme.name == "]") {
							nxt();
							continue;
						}
					}
					Error();
				}
				if (NowLexeme.type == types::DELIMETED) {
					Tree.Push(name, type, k);
					return;
				}
			}
		}
	}
	Error();
}

void Arguments() {
	if (NowLexeme.name != "(")
		Error();
	nxt();
	bool flag = 1;
	while (NowLexeme.type == types::KEYWORD) {
		FuncControl.AddArguments(NowLexeme.name);
		if (!flag)
			Error();
		Declare();
		if (NowLexeme.name != ",")
			flag = 0;
		else
			nxt();
	}
	if (NowLexeme.name != ")")
		Error();
	nxt();
}

void Func() {
	if (NowLexeme.type == types::KEYWORD && (isType(NowLexeme.name) 
		|| isMass(NowLexeme.name) || NowLexeme.name == "void")) {
		std::string type = NowLexeme.name;
		nxt();
		if (NowLexeme.type == types::IDENTIFIER || NowLexeme.name == "main") {
			Registry[NowLexeme.name] = Poliz.size();
			FuncControl.AddFunc(NowLexeme.name, type);
			nxt();
			Tree.CreateTID();
			Arguments();
			Block(0);

			Tree.UpTree();
			return;
		}
	}
	Error();
}

void inGlobal() {
	if (NowLexeme.name == "func") {
		nxt();
		Func();
		return;
	}
	else {
		Declare();
		if (NowLexeme.name == ";") {
			return;
		}
	}
	Error();
}

void Start() {
	while (!Geter.Empty()) {
		nxt();
		inGlobal();
	}
}

void SyntaxAnalzer() {
	try {
		Start();
		if (!Geter.Empty()) {
			throw std::string("Not Empty Lexemes");
		}
		std::cout << "OK";
	}
	catch (std::string str){
		std::cout << str;
	}
}

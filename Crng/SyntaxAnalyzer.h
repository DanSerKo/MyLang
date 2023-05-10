#include "LexAnalyzer.h"
#include "SemanticAnalyzer.h"
#include "Comp.h"

int Pred = 0;
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
void Declare(bool f); 
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
	if (flag) {
		Poliz.push_back({ types::Unknow, "newTIDtoE" });
		Tree.CreateTID();
	}
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
			Declare(0);
		else
			Assign();
		if (NowLexeme.name != ";")
			Error();
		nxt();
	}
	if (flag) {
		Poliz.push_back({ types::Unknow, "upTIDtoE" });
		Tree.UpTree();
	}
}


void Data() { 
	if (NowLexeme.type != types::IDENTIFIER)
		Error();
	auto now = NowLexeme;
	nxt();
	if (NowLexeme.name == "(") {
		Poliz.push_back({ types::Unknow, "NewTID" });
		Poliz.push_back({ types::NUMBER, std::to_string(Poliz.size() + 4) });
		Poliz.push_back({ types::Unknow, "NewGO" });
		Poliz.push_back({ types::NUMBER, std::to_string(Registry[now.name].first) });
		Poliz.push_back({ types::Jump, "!" });
		nxt();
		std::vector <std::string> param;
		int k = 0;
		auto ArgumName = FuncControl.GetParFunc(now.name).ArgumName;
		while (NowLexeme.name != ")") {
			Poliz.push_back({ types::IDENTIFIER, ArgumName[k]});
			Poliz.push_back({ types::NUMBER, "0" });
			Poliz.push_back({ types::Unknow, "&" });			
			Pred = 1;
			Expression();
			Pred = 0;
			Poliz.push_back({ types::KEYWORD, "=" });
			k++;
			param.push_back(Control.GetType());
			if (NowLexeme.name == ")")
				continue;
			if (NowLexeme.name != ",")
				Error();
			nxt();
		}
		Poliz.push_back({ types::NUMBER, std::to_string(Poliz.size() + 4) });
		Poliz.push_back({ types::Unknow, "NewGO" });
		Poliz.push_back({ types::NUMBER, std::to_string(Registry[now.name].second) });
		Poliz.push_back({ types::Jump, "!" });
		nxt();
		if (param != FuncControl.GetParFunc(now.name).ArgumType) {
			throw std::string("invalid function arguments in line ") + std::to_string(now.x + 1);
		}
		Control.Push(FuncControl.GetParFunc(now.name).MyType);
		return;
	}
	if (Pred)
		Poliz.push_back({ types::Unknow, "0pred" });
	Poliz.push_back(now);
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
	Poliz.push_back({ types::NUMBER, std::to_string(k) });
	Poliz.push_back({ types::Unknow, "build" });
	Control.Push(Tree.GetType(now.name).type);
	if (k != Tree.GetType(now.name).Dim)
		throw std::string("incorrect dimensions in line ") + std::to_string(now.x + 1);
}

void String() {
	if (NowLexeme.name == "'") {
		nxt();
		if (NowLexeme.name.size() != 1)
			Error();
		Poliz.push_back({ types::STRING, "'" + NowLexeme.name + "'" });
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
	Poliz.push_back({ types::STRING, "\"" });
	while (NowLexeme.name != "\"") {
		Poliz.back().name += NowLexeme.name;
		nxt();
	}	
	Poliz.back().name += "\"";
	nxt();
	Control.Push("string");
}

void Object() {
	if (NowLexeme.name == "true" || NowLexeme.name == "false") {
		Poliz.push_back(NowLexeme);
		Control.Push("bool");
		nxt();
		return;
	}
	if (NowLexeme.type == types::NUMBER) {
		Poliz.push_back(NowLexeme);
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
	auto now = NowLexeme;
	nxt();
	if (NowLexeme.name == "(") {
		Poliz.push_back({ types::Unknow, "NewTID" });
		Poliz.push_back({ types::NUMBER, std::to_string(Poliz.size() + 4) });
		Poliz.push_back({ types::Unknow, "NewGO" });
		Poliz.push_back({ types::NUMBER, std::to_string(Registry[now.name].first) });
		Poliz.push_back({ types::Jump, "!" });
		nxt();
		std::vector <std::string> param;
		int k = 0;
		auto ArgumName = FuncControl.GetParFunc(now.name).ArgumName;
		while (NowLexeme.name != ")") {
			Poliz.push_back({ types::IDENTIFIER, ArgumName[k] });
			Poliz.push_back({ types::NUMBER, "0" });
			Poliz.push_back({ types::Unknow, "&" });
			Pred = 1;
			Expression();
			Pred = 0;
			Poliz.push_back({ types::KEYWORD, "=" });
			k++;
			param.push_back(Control.GetType());
			if (NowLexeme.name == ")")
				continue;
			if (NowLexeme.name != ",")
				Error();
			nxt();
		}
		Poliz.push_back({ types::NUMBER, std::to_string(Poliz.size() + 4) });
		Poliz.push_back({ types::Unknow, "NewGO" });
		Poliz.push_back({ types::NUMBER, std::to_string(Registry[now.name].second) });
		Poliz.push_back({ types::Jump, "!" });
		nxt();
		if (param != FuncControl.GetParFunc(now.name).ArgumType) {
			throw std::string("invalid function arguments in line ") + std::to_string(now.x + 1);
		}
		Control.Push(FuncControl.GetParFunc(now.name).MyType);
		return "IsFunc";
	}
	Poliz.push_back(now);
	std::string NowType = Tree.GetType(now.name).type;
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
	Poliz.push_back({ types::NUMBER, std::to_string(k) });
	Poliz.push_back({ types::Unknow, "&" });
	if (k != Tree.GetType(now.name).Dim)
		throw std::string("incorrect dimensions in line ") + std::to_string(now.x + 1);
	return NowType;
	/*
	if (NowLexeme.type != types::IDENTIFIER)
		Error();
	Poliz.push_back(NowLexeme);
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
	Poliz.push_back({ types::NUMBER, std::to_string(k) });
	Poliz.push_back({ types::Unknow, "&" });
	if (k != Tree.GetType(now.name).Dim)
		throw std::string("incorrect dimensions in line ") + std::to_string(now.x + 1);
	return NowType;
	*/
}

void Return() {
	if (NowLexeme.name != "return")
		Error();
	nxt();
	Expression();
	Poliz.push_back({ types::Unknow, "CloseTID" });
	Poliz.push_back({ types::Jump, "!GO" });
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
	Poliz.push_back({ types::KEYWORD, "cout" });
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
	Poliz.push_back({ types::KEYWORD, "cin" });
	Poliz.push_back({ types::KEYWORD, "=" });
	if (NowLexeme.name != ")")
		Error();
	nxt();
}

void Assign() {
	auto NowType = Variable();
	if (NowType == "IsFunc")
		return;
	if (NowLexeme.name != "=")
		Error();
	nxt();
	Expression();
	Poliz.push_back({ types::KEYWORD, "=" });
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
	int p0 = Poliz.size();
	Expression();
	if (Control.GetType() != "bool")
		Error();
	if (NowLexeme.name != ")")
		Error();
	int p1 = Poliz.size();
	Poliz.push_back({ types::NUMBER, "_" });
	Poliz.push_back({ types::Jump, "F!" });
	nxt();
	Block();
	Poliz.push_back({ types::NUMBER, std::to_string(p0)});
	Poliz.push_back({ types::Jump, "!" });
	Poliz[p1].name = std::to_string(Poliz.size());
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
	int p1 = Poliz.size();
	Poliz.push_back({ types::NUMBER, "_" });
	Poliz.push_back({ types::Jump, "F!" });

	nxt();
	Block();


	int p2 = Poliz.size();
	Poliz.push_back({ types::NUMBER, "_" });
	Poliz.push_back({ types::Jump, "!" });
	nxt();
	Poliz[p1].name = std::to_string(Poliz.size());
	if (NowLexeme.name == "else") {
		Else();
		nxt();
	}
	Poliz[p2].name = std::to_string(Poliz.size());	
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
	int p0 = Poliz.size();
	Expression();
	int p1 = Poliz.size();
	Poliz.push_back({ types::NUMBER, "_" });
	Poliz.push_back({ types::Jump, "T!" });
	int p2 = Poliz.size();
	Poliz.push_back({ types::NUMBER, "_" });
	Poliz.push_back({ types::Jump, "!" });
	if (Control.GetType() != "bool")
		Error();
	if (NowLexeme.name != ";")
		Error();
	nxt();
	int p3 = Poliz.size();
	Assign();
	Poliz.push_back({ types::NUMBER, std::to_string(p0) });
	Poliz.push_back({ types::Jump, "!" });
	if (NowLexeme.name != ")")
		Error();
	nxt();
	Poliz[p1].name = std::to_string(Poliz.size());
	Block();
	Poliz.push_back({ types::NUMBER, std::to_string(p3) });
	Poliz.push_back({ types::Jump, "!" });

	Poliz[p2].name = std::to_string(Poliz.size());
}

void Expression6() {
	if (NowLexeme.name != "(") {
		Object();
	}
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
		Poliz.push_back(Now);
		Control.Update(Now);
	}
}

void Expression4() {
	Expression5();
	while (NowLexeme.name == "+" || NowLexeme.name == "-") {
		auto Now = NowLexeme;
		nxt();
		Expression5();
		Poliz.push_back(Now);
		Control.Update(Now);
	}
}

void Expression3() {
	Expression4();
	while (NowLexeme.name == "^" || NowLexeme.name == "&" || NowLexeme.name == "|") {
		auto Now = NowLexeme;
		nxt();
		Expression4();
		Poliz.push_back(Now);
		Control.Update(Now);
	}
}

void Expression2() {
	Expression3();
	while (NowLexeme.name == "==" || NowLexeme.name == ">" || NowLexeme.name == "<" || NowLexeme.name == "!=") {
		auto Now = NowLexeme;
		nxt();
		Expression3();
		Poliz.push_back(Now);
		Control.Update(Now);
	}
}

void Expression() {
	Expression2();
	while (NowLexeme.name == "and" || NowLexeme.name == "or") {
		auto Now = NowLexeme;
		nxt();
		Expression2();
		Poliz.push_back(Now);
		Control.Update(Now);
	}
}

void Declare(bool flag = 0) { 
	std::string type;
	int k = 0;
	std::string name;
	if (NowLexeme.type == types::KEYWORD) { 
		if (isType(NowLexeme.name) || isMass(NowLexeme.name)) {
			type = NowLexeme.name;
			nxt();
			if (NowLexeme.type == types::IDENTIFIER) {
				Poliz.push_back(NowLexeme);
				name = NowLexeme.name;
				nxt();
				while (NowLexeme.name == "[") {
					k++;
					nxt();
					if (NowLexeme.type == types::NUMBER && !count(NowLexeme.name.begin(), NowLexeme.name.end(), '.')) {
						Poliz.push_back(NowLexeme);
						nxt();
						if (NowLexeme.name == "]") {
							nxt();
							continue;
						}
					}
					Error();
				}
				if (NowLexeme.type == types::DELIMETED) {
					if (flag) {
						FuncControl.AddArguments(name, type);
					}
					Poliz.push_back({ types::NUMBER, std::to_string(k) });
					Poliz.push_back({ types::KEYWORD, type });
					Tree.Push(name, type, k);
					return;
				}
			}
		}
	}
	Error();
}

void Arguments(bool IsMain) { 
	if (NowLexeme.name != "(")
		Error();
	nxt();
	bool flag = 1;
	while (NowLexeme.type == types::KEYWORD) {
		if (!flag)
			Error();
		Declare(1);
		if (NowLexeme.name != ",")
			flag = 0;
		else
			nxt();
	}
	if (!IsMain)
		Poliz.push_back({ types::Jump, "!GO" });
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
			bool IsMain = NowLexeme.name == "main";
			auto IsName = NowLexeme.name;
			
			int p1 = Poliz.size();
			if (!IsMain) {
				Poliz.push_back({ types::NUMBER, "_" });
				Poliz.push_back({ types::Jump, "!" });
			}
			else {
				Poliz.push_back({ types::Unknow, "NewTID" });
			}
			Registry[IsName].first = Poliz.size();
			FuncControl.AddFunc(NowLexeme.name, type);
			nxt();
			Tree.CreateTID();
			Arguments(IsMain);
			Registry[IsName].second = Poliz.size();
			Block(0);
			Tree.UpTree();
			Poliz.push_back({types::Unknow, "CloseTID"});
			if (!IsMain) {
				Poliz.push_back({types::Jump, "!GO"});
				Poliz[p1].name = std::to_string(Poliz.size());
			}
			else {
				Poliz.push_back({ types::KEYWORD, "Exit" });
			}
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
		Declare(0);
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

bool SyntaxAnalzer() {
	try {
		Start();
		if (!Geter.Empty()) {
			throw std::string("Not Empty Lexemes");
		}
		return 1;
	}
	catch (std::string str){
		std::cout << str;
		return 0;
	}
}

#pragma once
#include <map>
#include <string>
#include <vector>
#include <cassert>

struct ParType {
	std::string type;
	int Dim; 
	ParType(std::string t = "-", int k = 0) {
		type = t;
		Dim = k;
	}
};

class TableInBlock {
public:
	TableInBlock(TableInBlock* p = nullptr) {
		parent = p;
	}
	void Push(std::string name, std::string type, int k) {
		if (table.count(name))
			throw std::string(name + " declared in this block");
		table[name] = { type, k };
	}
	TableInBlock* Up() {
		return parent;
	}
	bool Count(std::string name) {
		return table.count(name);
	}
	ParType Get(std::string name) {
		return table[name];
	}
private:
	std::map <std::string, ParType> table;
	TableInBlock* parent;
};

class TID {
public:
	TID() {
		Now = new TableInBlock(nullptr);
	}
	void CreateTID() {
		Now = new TableInBlock(Now);
	}
	void UpTree() {
		Now = Now->Up();
	}
	void Push(std::string name, std::string type, int k) {
		Now->Push(name, type, k);
	}
	ParType GetType(std::string name) {
		auto check = Now;
		while (check != nullptr) {
			if (check->Count(name))
				return check->Get(name);
			check = check->Up();
		}
		throw std::string(name + " not declared");
	}
private:
	TableInBlock* Now;
};

class ControlTypes {
public:
	void Push(std::string type) {
		_type.push_back(type);
	}
	void Update(Lexeme Now) {
		std::string a = _type.back();
		_type.pop_back();
		std::string b = _type.back();
		_type.pop_back();
		std::string operation = Now.name;
		if (operation == "&" || operation == "|" || operation == "^" || operation == "+"
			|| operation == "-" || operation == "*" || operation == "/" || operation == "%") {
			if (a == "int64" && (b == "int" || b == "int64")) {
				_type.push_back("int64");
				return;
			}
			if (b == "int64" && (a == "int" || a == "int64")) {
				_type.push_back("int64");
				return;
			}
			if (a == "int" && b == "int") {
				_type.push_back("int");
				return;
			}
		}
		if (operation == "+" || operation == "-" || operation == "*" || operation == "/") {
			if (a == "float" && (b == "int" || b == "int64" || b == "float")) {
				_type.push_back("float");
				return;
			}
			if (b == "float" && (a == "int" || a == "int64" || a == "float")) {
				_type.push_back("float");
				return;
			}
		}
		if (operation == "+") {
			if ((a == "string" || a == "char") && (b == "string" || b == "char")) {
				_type.push_back("string");
				return;
			}
		}
		if (operation == "==" || operation == ">" || operation == "<" || operation == "!=") {
			if (a == b || ((a == "int64" || a == "int" || a == "float") && (b == "int64" || b == "int" || b == "float"))) {
				_type.push_back("bool");
				return;
			}
		}
		if (operation == "and" || operation == "or") {
			if (a == "bool" && b == "bool") {
				_type.push_back("bool");
				return;
			}
		}
		throw std::string("incorrect expression in line ") + std::to_string(Now.x + 1);
	}
	std::string GetType() {
		auto res = _type.back();
		_type.pop_back();
		return res;
	}
private:
	std::vector <std::string> _type;
};

struct ParFunc {
	std::string MyType;
	std::vector <std::string> ArgumType;
	std::vector <std::string> ArgumName;
};

class TableToFunc {
public:
	void AddFunc(std::string name, std::string type) {
		if (table.count(name))
			throw std::string("function " + name + " declared");
		table[name].MyType = type;
		NowFunc = name;
	}
	void AddArguments(std::string name, std::string type) {
		table[NowFunc].ArgumName.push_back(name);
		table[NowFunc].ArgumType.push_back(type);
	}
	ParFunc GetParFunc(std::string name) {
		if (!table.count(name)) {
			throw std::string("function " + name + " not declared");
		}
		return table[name];
	}
	std::string GetFunc() {
		return NowFunc;
	}
private:
	std::string NowFunc;
	std::map <std::string, ParFunc> table;
};

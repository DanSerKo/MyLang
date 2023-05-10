#include "Lexeme.h"

std::vector <Lexeme> Poliz;
std::map <std::string, std::pair <int, int> > Registry;

struct node {
	std::string type;
	std::vector <int> sz;
	std::vector <std::string> cont;
};


class TIDtoExecuter {
public:
	TIDtoExecuter(TIDtoExecuter* p = nullptr) {
		parent = p;
	}
	void Push(std::string name, std::string type, std::vector <int> sz) {
		if (table.count(name))
			throw std::string(name + " declared in this block");
		int k = 1;
		for (auto& o : sz) {
			k *= o;
		}
		table[name] = { type, sz, std::vector <std::string>(k) };
	}
	std::string Read(std::string name, std::vector <int> pos) {
		int p = 0;
		auto type = table[name].type;
		auto sz = table[name].sz;
		for (int i = 0; i < pos.size(); i++)
			p = p * sz[i] + pos[i];
		return {table[name].cont[p] };
	}
	void Write(std::string name, std::vector <int> pos, std::string to) {
		int p = 0;
		auto type = table[name].type;
		auto sz = table[name].sz;
		for (int i = 0; i < pos.size(); i++)
			p = p * sz[i] + pos[i];
		table[name].cont[p] = to;
	}
	TIDtoExecuter* Up() {
		return parent;
	}
	bool Count(std::string name) {
		return table.count(name);
	}
	std::string GetType(std::string name) {
		return table[name].type;
	}
private:
	std::map <std::string, node> table;
	TIDtoExecuter* parent;
};

class RealTID {
public:
	RealTID(TIDtoExecuter* p) {
		Now = new TIDtoExecuter(p);
	}
	void CreateTID() {
		Now = new TIDtoExecuter(Now);
	}
	void UpTree() {
		Now = Now->Up();
	}
	void Push(std::string name, std::string type, std::vector <int> sz) {
		Now->Push(name, type, sz);
	}
	std::string Read(std::string name, std::vector <int> pos) {
		auto check = Now;
		while (check != nullptr) {
			if (check->Count(name)) {
				return check->Read(name, pos);
			}
			check = check->Up();
		}
	}
	std::string GetType(std::string name) {
		auto check = Now;
		while (check != nullptr) {
			if (check->Count(name))
				return check->GetType(name);
			check = check->Up();
		}
	}
	void Write(std::string name, std::vector <int> pos, std::string to) {
		auto check = Now;
		while (check != nullptr) {
			if (check->Count(name)) {
				check->Write(name, pos, to);
				return;
			}
			check = check->Up();
		}
	}
private:
	TIDtoExecuter* Now;
};


class Executer {
public:
	Executer(const std::vector <Lexeme>& Poliz) {
		_poliz = Poliz;
		Global = new TIDtoExecuter();
		t = 0;
	}
	void Execute() {
		auto now = _poliz[t];
		while (now.name != "Exit") {
			next(now);
			t++;
			now = _poliz[t];
		}
	}
private:
	void next(Lexeme now) {
		if (now.type == types::Jump) {
			if (now.name == "!") {
				t = stoi(stack.back()) - 1;
				stack.pop_back();
				return;
			}
			if (now.name == "!GO") {
				t = GO.back() - 1;
				GO.pop_back();
				return;
			}
			if (now.name == "T!") {
				std::string k = stack.back();
				stack.pop_back();
				if (stack.back() == "true") {
					t = stoi(k) - 1;
				}
				stack.pop_back();
				return;
			}
			if (now.name == "F!") {
				std::string k = stack.back();
				stack.pop_back();
				if (stack.back() == "false") {
					t = stoi(k) - 1;
				}
				stack.pop_back();
				return;
			}
		}
		if (now.type == types::Unknow) {
			if (now.name == "NewGO") {
				GO.push_back(stoi(stack.back()));
				stack.pop_back();
				return;
			}
			if (now.name == "NewTID") {
				StackToFunc.push_back(RealTID(&Global));
				return;
			}
			if (now.name == "CloseTID") {
				StackToFunc.pop_back();
				return;
			}
			if (now.name == "newTIDtoE") {
				StackToFunc.back().CreateTID();
				return;
			}
			if (now.name == "upTIDtoE") {
				StackToFunc.back().UpTree();
				return;
			}
			if (now.name == "&") {
				int k = stoi(stack.back());
				stack.pop_back();
				std::vector <int> sz;
				for (int i = 0; i < k; i++) {
					sz.push_back(stoi(stack.back()));
					stack.pop_back();
				}
				auto name = stack.back();
				stack.pop_back();
				ustack.push_back({ name, sz });
				return;
			}
			if (now.name == "build") {
				int k = stoi(stack.back());
				stack.pop_back();
				std::vector <int> sz;
				for (int i = 0; i < k; i++) {
					sz.push_back(stoi(stack.back()));
					stack.pop_back();
				}
				auto name = stack.back();
				stack.pop_back();
				std::string nw;
				if (!stack.empty() && stack.back() == "0pred") {
					nw = StackToFunc[(int)StackToFunc.size() - 2].Read(name, sz);
					stack.pop_back();
				}
				else {
					nw = StackToFunc.back().Read(name, sz);
				}
				stack.push_back(nw);
				return;
			}
		}
		if (now.type == types::OPERATION) {
			if (now.name == "+") {
				std::string b = stack.back();
				stack.pop_back();
				std::string a = stack.back();
				stack.pop_back();
				if (b[0] == '"' || b[0] == '\'') {
					std::reverse(a.begin(), a.end());
					std::reverse(b.begin(), b.end());
					a.pop_back();
					b.pop_back();
					std::reverse(a.begin(), a.end());
					std::reverse(b.begin(), b.end());
					a.pop_back();
					b.pop_back();
					stack.push_back("\"" + a + b + "\"");
				}
				else {
					if (count(a.begin(), a.end(), '.') || count(b.begin(), b.end(), '.'))
						stack.push_back(std::to_string(stod(a) + stod(b)));
					else
						stack.push_back(std::to_string(stoll(a) + stoll(b)));
				}
				return;
			}
			if (now.name == "-") {
				std::string b = stack.back();
				stack.pop_back();
				std::string a = stack.back();
				stack.pop_back();
				if (count(a.begin(), a.end(), '.') || count(b.begin(), b.end(), '.'))
					stack.push_back(std::to_string(stod(a) - stod(b)));
				else
					stack.push_back(std::to_string(stoll(a) - stoll(b)));
				return;
			}
			if (now.name == "/") {
				std::string b = stack.back();
				stack.pop_back();
				std::string a = stack.back();
				stack.pop_back();
				if (count(a.begin(), a.end(), '.') || count(b.begin(), b.end(), '.'))
					stack.push_back(std::to_string(stod(a) / stod(b)));
				else
					stack.push_back(std::to_string(stoll(a) / stoll(b)));
				return;
			}
			if (now.name == "*") {
				std::string b = stack.back();
				stack.pop_back();
				std::string a = stack.back();
				stack.pop_back();
				if (count(a.begin(), a.end(), '.') || count(b.begin(), b.end(), '.'))
					stack.push_back(std::to_string(stod(a) * stod(b)));
				else
					stack.push_back(std::to_string(stoll(a) * stoll(b)));
				return;
			}
			if (now.name == "%") {
				std::string b = stack.back();
				stack.pop_back();
				std::string a = stack.back();
				stack.pop_back();
				stack.push_back(std::to_string(stoll(a) % stoll(b)));
				return;
			}
			if (now.name == "&") {
				std::string b = stack.back();
				stack.pop_back();
				std::string a = stack.back();
				stack.pop_back();
				stack.push_back(std::to_string(stoll(a) & stoll(b)));
				return;
			}
			if (now.name == "|") {
				std::string b = stack.back();
				stack.pop_back();
				std::string a = stack.back();
				stack.pop_back();
				stack.push_back(std::to_string(stoll(a) | stoll(b)));
				return;
			}
			if (now.name == "^") {
				std::string b = stack.back();
				stack.pop_back();
				std::string a = stack.back();
				stack.pop_back();
				stack.push_back(std::to_string(stoll(a) ^ stoll(b)));
				return;
			}
			if (now.name == "<") {
				std::string b = stack.back();
				stack.pop_back();
				std::string a = stack.back();
				stack.pop_back();
				if (stod(a) < stod(b))
					stack.push_back("true");
				else
					stack.push_back("false");
				return;
			}
			if (now.name == ">") {
				std::string b = stack.back();
				stack.pop_back();
				std::string a = stack.back();
				stack.pop_back();
				if (stod(a) > stod(b))
					stack.push_back("true");
				else
					stack.push_back("false");
				return;
			}
		}
		if (now.type == types::NUMBER || now.type == types::STRING || now.type == types::IDENTIFIER || now.name == "true" || now.name == "false" || now.name == "0pred") {
			stack.push_back(now.name);
			return;
		}
		if (now.name == "int" || now.name == "int64" || now.name == "string"
			|| now.name == "char" || now.name == "float" || now.name == "bool") {
			int k = stoi(stack.back());
			stack.pop_back();
			std::vector <int> sz;
			for (int i = 0; i < k; i++) {
				sz.push_back(stoi(stack.back()));
				stack.pop_back();
			}
			auto name = stack.back();
			stack.pop_back();
			if (!StackToFunc.empty())
				StackToFunc.back().Push(name, now.name, sz);
			else
				Global.Push(name, now.name, sz);
			return;
		}
		if (now.type == types::KEYWORD) {
			if (now.name == "cin") {
				std::string nw;
				std::cin >> nw;

				std::string name = ustack.back().first;
				std::string tp;
				tp = StackToFunc.back().GetType(name);
				if (tp == "string")
					stack.push_back("\"" + nw + "\"");
				else if (tp == "char")
					stack.push_back("'" + nw + "'");
				else
					stack.push_back(nw);
				return;
			}
			if (now.name == "cout") {
				std::string nw = stack.back();
				if (nw == "\"\\n\"")
					std::cout << std::endl;
				else if (nw == "\"\"")
					std::cout << " ";
				else {
					if (nw[0] == '\'' || nw[0] == '"') {
						reverse(nw.begin(), nw.end());
						nw.pop_back();
						reverse(nw.begin(), nw.end());
						nw.pop_back();
					}
					std::cout << nw;
				}
				stack.pop_back();
				return;
			}
			if (now.name == "=") {
				auto [name, sz] = ustack.back();
				ustack.pop_back();
				auto to = stack.back();
				stack.pop_back();
				if (!StackToFunc.empty())
					StackToFunc.back().Write(name, sz, to);
				else
					Global.Write(name, sz, to);
				return;
			}
			if (now.name == "==") {
				std::string b = stack.back();
				stack.pop_back();
				std::string a = stack.back();
				stack.pop_back();
				if (count(a.begin(), a.end(), '.')) {
					while (a.back() == '0')
						a.pop_back();
					if (a.back() == '.')
						a.pop_back();
				}
				if (count(b.begin(), b.end(), '.')) {
					while (b.back() == '0')
						b.pop_back();
					if (b.back() == '.')
						b.pop_back();
				}
				if (a == b)
					stack.push_back("true");
				else
					stack.push_back("false");
				return;
			}
			if (now.name == "!=") {
				std::string b = stack.back();
				stack.pop_back();
				std::string a = stack.back();
				stack.pop_back();
				if (count(a.begin(), a.end(), '.')) {
					while (a.back() == '0')
						a.pop_back();
					if (a.back() == '.')
						a.pop_back();
				}
				if (count(b.begin(), b.end(), '.')) {
					while (b.back() == '0')
						b.pop_back();
					if (b.back() == '.')
						b.pop_back();
				}
				if (a != b)
					stack.push_back("true");
				else
					stack.push_back("false");
				return;
			}
			if (now.name == "or") {
				std::string b = stack.back();
				stack.pop_back();
				std::string a = stack.back();
				stack.pop_back();
				if (a == "true" || b == "true")
					stack.push_back("true");
				else
					stack.push_back("false");
				return;
			}
			if (now.name == "and") {
				std::string b = stack.back();
				stack.pop_back();
				std::string a = stack.back();
				stack.pop_back();
				if (a == "true" && b == "true")
					stack.push_back("true");
				else
					stack.push_back("false");
				return;
			}
		}
		std::cout << "Error, unknow" << ' ' << now.name;
		exit(0);
	}
	int t;
	std::vector <int> GO;
	std::vector <Lexeme> _poliz; 
	std::vector <std::string> stack; 
	std::vector <std::pair <std::string, std::vector <int> >> ustack;
	TIDtoExecuter Global; 
	std::vector <RealTID> StackToFunc;
};


/*
Не работает обьявление функций заранее
Не работают массивы в аргументах функции 
Не работают вложенные функции 
*/
#define _CRT_SECURE_NO_WARNINGS
#include "Lexeme.h"
#include <fstream>

class LexAnalyzer {
public:
	LexAnalyzer() {
		std::ifstream tin("input.txt");
		std::string s;
		std::string text;
		while (getline(tin, s)) {
			for (int i = 0; i < (int)s.size() - 1; i++)
				if (s[i] == '/' && s[i + 1] == '/') {
					s.erase(s.begin() + i, s.end());
					break;
				}
			text += s + '\n';
		}
		tin.close();
		s.clear();
		int x = 0, y = 0;
		for (int i = 0; i < text.size(); i++) {
			if (std::count(DELIMETED.begin(), DELIMETED.end(), text[i])) {
				if (s.size())
					LEX.push_back({ GetType(s), s, x, y });
				if (text[i] != ' ' && text[i] != '\n' && text[i] != '\t')
					LEX.push_back({ types::DELIMETED, std::string(1, text[i]), x, y });
				if (text[i] == '\n') {
					x++;
					y = 0;
				}
				s.clear();
			}
			else if (std::count(OPERATION.begin(), OPERATION.end(), text[i])) {
				if (s.size())
					LEX.push_back({ GetType(s), s, x, y });
				if (text[i] != ' ' && text[i] != '\n' && text[i] != '\t')
					LEX.push_back({ types::OPERATION, std::string(1, text[i]), x, y });
				s.clear();
			}
			else {
				s.push_back(text[i]);
			}
			y++;
		}
	}
	bool Empty() {
		return LEX.empty();
	}
	Lexeme GetLexem() {
		auto q = LEX.front();
		/*if (q.type == types::Unknow) {
			throw std::string("invalid Lexem's type");
		}*/
		LEX.pop_front();
		return q;
	}
private:
	std::deque <Lexeme> LEX;
	std::vector <char> DELIMETED = { ',', ';', ' ', '(', ')', '{', '}', '\'', '"', '\n', '[', ']', '\t'};
	std::vector <char> OPERATION = { '*', '/', '-', '+', '%', '^', '<', '>', '&', '|' };
	std::vector <std::string> KEYWORD = { "for", "while", "and", "or", "string", "int", "int64", "main", "cin",
	"cout", "float", "if", "else", "endl", "return", "char", "bool", "void", "func", "==", "=", "!=", "\\n"};

	types GetType(const std::string& s) {
		if (std::count(KEYWORD.begin(), KEYWORD.end(), s))
			return types::KEYWORD;
		if (std::isalpha(s[0])) {
			for (auto& r : s)
				if (r == '.') {
					return types::Unknow;
				}
			return types::IDENTIFIER;
		}
		if (std::isdigit(s[0])) {
			int f = 0;
			for (auto& r : s) {
				if ((r != '.' || f) && !std::isdigit(r)) {
					return types::Unknow;
				}
				if (r == '.')
					f = 1;
			}
			return types::NUMBER;
		}
		return types::Unknow;
	}
};

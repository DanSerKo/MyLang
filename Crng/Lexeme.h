#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <deque>
#include <map>

enum types {
	DELIMETED,
	OPERATION,
	KEYWORD,
	IDENTIFIER,
	NUMBER, 
	Unknow,
	Jump
};

struct Lexeme {
	types type;
	std::string name;
	int	x, y;
};


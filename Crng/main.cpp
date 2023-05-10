#include "SyntaxAnalyzer.h"

int main() {
	if (SyntaxAnalzer()) {
		for (int i = 0; i < Poliz.size(); i++)
			std::cout << i << ' ' << Poliz[i].name << std::endl;
		std::cout << "----------------------------------------------------\n";
		Executer ex(Poliz);
		ex.Execute();
	}
}

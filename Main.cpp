#include "Lexer.hpp"
#include "Parser.hpp"
#include <cstring>

int main(int argc, char *argv[]){
	cout << "This is lexer of subset of SIGNAL programming language. Made by Maksym Sadovenko." << endl;
	Lexer lexer;
	Parser parser;
	for(int i = 1; i < argc; i++){
		fstream input_file(argv[i]);
		if (!input_file.is_open()) {
			cout << "Failed to open " << argv[i] << endl;
		}else{
			cout << "File " << argv[i] << " was opened" << endl;
			string output_file_name(argv[i]);
    		output_file_name.replace(strlen(argv[i]) - 4, strlen("_generated"), "_generated");
			input_file.close();
			if(lexer.lexical_analysis(output_file_name + "_lexer.txt", argv[i])){
				if(parser.parser_analysis(lexer.get_lexems(), output_file_name + "_parser.txt")){
					cout << "Parser ended without error" << endl;
				}else {
					cout << "Parser ended with error" << endl;
				}
			}
		}
	}
	return 0;
}
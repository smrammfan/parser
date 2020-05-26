#pragma once
#include "TranslatorConstants.hpp"

using namespace std;

class Lexer {
public:
	Lexer();
	~Lexer(){};

	bool lexical_analysis(string, string);
	vector<lexem*> get_lexems() const {return lexem_table;};
	
private:
	fstream input_file;

	int lex_counter;
	vector <lexem*> lexem_table;
	vector <string> identifier_talbe;
	vector <string> digit_table;
	vector <const lexem*> key_word_table;
	
	int line;
	int save_line;
	
	int column;
	int save_column;
	
	int identifier_counter;
	int digit_counter;
	int ASCIIArr[128];
	int pos;
	char sbuff;
	bool was_error;
	void add_lexem(int, int, int, string);
	bool size_out();
	void INP();
	void SPACE();
	void IDN();
	void DG();
	void BCOM(); // begin comment
	void COM(string); // comment
	void ECOM(string); // end comment
	void DM();
	void ERR(int, string);
	int search_in_digit_table(string);
	int search_in_identifier_table(string);
	int search_in_key_word_table(string);
	void make_listing(string);
};
#include "Lexer.hpp"

Lexer::Lexer() {
	key_word_table.push_back(&PROGRAM);
	key_word_table.push_back(&BEGIN);
	key_word_table.push_back(&END);
	key_word_table.push_back(&LABEL);
	key_word_table.push_back(&GOTO);
	key_word_table.push_back(&LINK);
	key_word_table.push_back(&IN);
	key_word_table.push_back(&OUT);

	for (int i = 0; i < 128; i++) {
		if ((i == 9) || (i == 10) || (i == 32) || (i == 12) || (i == 13))
			ASCIIArr[i] = 0;
		else {
			if ((i < 91) && (i > 64))
				ASCIIArr[i] = 1;
			else {
				if ((i < 58) && (i > 47))
					ASCIIArr[i] = 2;
				else {
					if ((i == ')') || (i == '(') || (i == '*') || (i == '.') || (i == ';') || (i == ',') || (i == ':'))
						ASCIIArr[i] = 3;
					else {
						ASCIIArr[i] = 4;
					}
				}
			}
		}
	}
}

void Lexer::add_lexem(int code, int line, int column, string name) {
	lexem_table.push_back(new lexem(code, line, column, name));
}

bool Lexer::size_out(){
	return !input_file.eof();
}

bool Lexer::lexical_analysis(string output_file_name, string input_file_name) {
	input_file.open(input_file_name);
	lexem_table.clear();
	identifier_talbe.clear();
	digit_table.clear();
	pos = 0;
	line = 1;
	column = 1;
	digit_counter = MIN_DIGIT_CODE;
	identifier_counter = MIN_IDENTIFIER_CODE;
	was_error = false;

	sbuff = input_file.get();
	if (size_out())
		INP();

	input_file.close();
	lexem_table.push_back(new lexem(END_OF_FILE_CODE, line - 1, pos, "<END-OF-FILE>"));

	make_listing(output_file_name);
	return !was_error;
}

void Lexer::INP(){
	while (size_out()){
		switch (ASCIIArr[sbuff]){
		case 0:
			SPACE();
			break;
		case 1:
			IDN();
			break;
		case 2:
			DG();
			break;
		case 3:
			DM();
			break;
		case 4:
			ERR(LEX_ERROR_CODE_IMPOSIBLE_CHAR, "");
			break;
		}
	}
}

void Lexer::SPACE(){
	if (size_out()){
		while ((size_out()) && (ASCIIArr[sbuff] == 0)){
			if (sbuff == 10){
				pos++;
				column++;
				sbuff = input_file.get();
				line++;
				column = 1;
			}
			else {
				pos++;
				column++;
				sbuff = input_file.get();
			}
		}
		return;
	}
}

void Lexer::DG(){
	save_line = line;
	save_column = column;
	string Buf;
	while ((size_out()) && (ASCIIArr[sbuff] == 2)){
		Buf += sbuff;
		pos++;
		sbuff = input_file.get();
		column++;
	}

	switch (ASCIIArr[sbuff]){
	case 0:
	case 3:
	case 4:
	{
		int n = search_in_digit_table(Buf);
		if (n == -1){
			add_lexem(digit_counter, save_line, save_column, Buf);
			digit_table.push_back(Buf);
			digit_counter++;
		}else{
			add_lexem(n + MIN_DIGIT_CODE, save_line, save_column, Buf);
		}
		Buf = "";
		return;
	}
	break;
	case 1:
	case 5:
		ERR(LEX_ERROR_CODE_IMPOSIBLE_CHAR, Buf);
		break;
	}
}

void Lexer::IDN(){
	int n;
	save_line = line;
	save_column = column;
	string Buf;
	while ((size_out()) && ((ASCIIArr[sbuff] == 2) || (ASCIIArr[sbuff] == 1))){
		Buf += sbuff;
		pos++;
		sbuff = input_file.get();
		column++;
	}

	if (ASCIIArr[sbuff] == 5){
		ERR(LEX_ERROR_CODE_IMPOSIBLE_CHAR, Buf);
	}
	else
	{
		n = search_in_key_word_table(Buf);
		if (n == -1) {
			n = search_in_identifier_table(Buf);
			if (n == -1)
			{
				add_lexem(identifier_counter, save_line, save_column, Buf);
				identifier_talbe.push_back(Buf);
				identifier_counter++;
			}
			else
			{
				add_lexem(n + MIN_IDENTIFIER_CODE, save_line, save_column, Buf);
			}
		}
		else add_lexem(key_word_table[n]->code, save_line, save_column, key_word_table[n]->name);
		Buf = "";
		return;
	}
}

void Lexer::BCOM(){
		pos++;
		sbuff = input_file.get();
		column++;
		COM("(*");
}

void Lexer::COM(string Buf){	
	while ((size_out()) && (sbuff != '*')){
		Buf += sbuff;
		if (sbuff == '\n'){
			line++;
			column = 1;
			pos++;
			sbuff = input_file.get();
		}
		else {
			pos++;
			sbuff = input_file.get();
			column++;
		}
	}
	if(!size_out()) ERR(LEX_ERROR_CODE_NOT_END_COMMENT, Buf);
	else ECOM(Buf);
}

void Lexer::ECOM(string Buf){
	sbuff = input_file.get();
	pos++;
	column++;
	
	if ((size_out()) && (sbuff == ')')){
		pos++;
		sbuff = input_file.get();
		column++;
		return;
	}
	else if(!size_out()){
		ERR(LEX_ERROR_CODE_NOT_END_COMMENT, Buf);
	}else COM(Buf);
	
}

void Lexer::DM(){ 
	save_line = line;
	save_column = column;
	string Buf = "";

	if (sbuff == '('){
		Buf += sbuff;
		pos++;
		sbuff = input_file.get();
		column++;
		if (sbuff == '*') {
			BCOM();
		}
		else {
			add_lexem('(', line, column - 1, Buf);
			return;
		}
	}else {
		if (sbuff == '*'){
			Buf += sbuff;
			ERR(LEX_ERROR_CODE_IMPOSIBLE_CHAR, Buf);
		}
		else
		{
			Buf += sbuff;
			add_lexem(sbuff, line, column, Buf);
			pos++;
			sbuff = input_file.get();
			column++;
			return;
		}
	}
}

void Lexer::ERR(int code_error, string pt){
	string Buf = pt;
	was_error = true;
	if ((sbuff == '*') || (sbuff == ')')){
		Buf += sbuff;
		add_lexem(code_error, save_line, save_column, Buf);
		pos++;
		sbuff = input_file.get();
		column++;
		Buf = "";
		INP();
	}
	else
	{
		while ((ASCIIArr[sbuff] != 0) && (ASCIIArr[sbuff] != 3) && (size_out())){
			Buf += sbuff;
			pos++;
			sbuff = input_file.get();
			column++;
		}

		add_lexem(code_error, save_line, save_column, Buf);
		Buf = "";
		INP();
	}
}

int Lexer::search_in_identifier_table(string Ident){
	for (int i = 0; i < identifier_talbe.size(); i++){
		if (identifier_talbe[i] == Ident)
			return i;
	}
	return -1;
}

int Lexer::search_in_digit_table(string Digit){
	for (int i = 0; i < digit_table.size(); i++){
		if (digit_table[i] == Digit)
			return i;
	}
	return -1;
}

int Lexer::search_in_key_word_table(string Ident){
	for (int i = 0; i < key_word_table.size(); i++){
		if (key_word_table[i]->name == Ident)
			return i;
	}
	return -1;
}

void Lexer::make_listing(string filename) {
	ofstream file(filename);
	file << setw(10) << "code" << setw(10) << "line" << setw(10) << "column" << setw(20) << "name" << endl << endl;
	for (int i = 0; i < lexem_table.size() - 1; i++){
			if (lexem_table[i]->code == LEX_ERROR_CODE_IMPOSIBLE_CHAR) {
				file << "Lexical error on line " << lexem_table[i]->line << " column " << lexem_table[i]->column << ": Impossible combination of characters: " << lexem_table[i]->name << endl;
				cout << "Lexical error on line " << lexem_table[i]->line << " column " << lexem_table[i]->column << ": Impossible combination of characters: " << lexem_table[i]->name << endl;
			}else if(lexem_table[i]->code == LEX_ERROR_CODE_NOT_END_COMMENT){
				file << "Lexical error on line " << lexem_table[i]->line << " column " << lexem_table[i]->column << ": Comment was not closed: " << lexem_table[i]->name << endl;
				cout << "Lexical error on line " << lexem_table[i]->line << " column " << lexem_table[i]->column << ": Comment was not closed: " << lexem_table[i]->name << endl;
			}else
				file << setw(10) << lexem_table[i]->code << setw(10) << lexem_table[i]->line << setw(10) << lexem_table[i]->column << setw(20) << lexem_table[i]->name << endl;
	}
	file << endl << endl;
	file.close();
	if (was_error)
		cout << "Lexical analysis was ended with error." << endl;
	else
		cout << "Lexical analysis was ended without error." << endl;
	
}


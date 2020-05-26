#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>

using namespace std;
struct lexem {
	lexem(int code, int line, int column, string name){
		this->code = code;
		this->line = line;
		this->column = column;
		this->name = name;
	}
	lexem(string name, int code){
		this->name = name;
		this->code = code;
		line = column = 0;
	}
	int code;
	int line;
	int column;
	string name;
};

struct node {
	node(int code, string name, int deep){
		this->code = code;
		this->deep = deep;
		this->name = name;
	};

    int code;
    string name;
    int deep; 
};

const int MIN_KEY_WORDS_CODE = 401;
const int MIN_DIGIT_CODE = 501;
const int MIN_IDENTIFIER_CODE = 1001;
const int LEX_ERROR_CODE_IMPOSIBLE_CHAR = 2000;
const int LEX_ERROR_CODE_NOT_END_COMMENT = 2001;
const int END_OF_FILE_CODE = 3001;

const lexem PROGRAM("PROGRAM", 401);
const lexem BEGIN("BEGIN", 402);
const lexem END("END", 403);
const lexem LABEL("LABEL", 404);
const lexem GOTO("GOTO", 405);
const lexem LINK("LINK", 406);
const lexem IN("IN", 407);
const lexem OUT("OUT", 408);

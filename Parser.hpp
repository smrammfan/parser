#pragma once
#include "TranslatorConstants.hpp"
#include <list>

class Parser {
public:
    Parser();
    ~Parser(){};
    
    bool parser_analysis(vector<lexem*>, string);
private:
    vector<lexem*> lexems;
    vector<const lexem*> statement_words;
    list<node*> tree;
    string error_text;
    bool wasError;

    bool signal_program();
    bool program(int&, int);
    bool procedure_identifier(int&, int);
    bool identifier(int &lexem_number, int deep);
    bool declarations(int &lexem_number, int deep);
    bool label_declarations(int &lexem_number, int deep);
    bool labels_list(int &lexem_number, int deep);
    bool block(int &lexem_number, int deep);
    bool statements_list(int &lexem_number, int deep);
    bool statement(int &lexem_number, int deep);
    bool unsinged_integer(int &lexem_number, int deep);
    bool variable_identifier(int &lexem_number, int deep);

    bool is_statement(string lexem);
    bool is_unsigned_integer(int lexem_number);
    void make_listing(string);

    void error(int, int, string, string);
    void errorEOF(int, int, string);

};
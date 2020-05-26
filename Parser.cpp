#include "Parser.hpp"


Parser::Parser(){
    statement_words.push_back(&GOTO);
    statement_words.push_back(&LINK);
    statement_words.push_back(&IN);
    statement_words.push_back(&OUT);
}

void Parser::error(int row, int column, string expected, string found) {
    error_text = "Parser: Error (line " + to_string(row) + ", column " + to_string(column) + "): " +
            "'" + expected + "' expected but '" + found + "' was found";
}

void Parser::errorEOF(int row, int column, string found) {
    error_text = "Parser: Error (line " + to_string(row) + ", column " + to_string(column) + "): " +
            "end of file expected but '" + found + "' was found";
}


bool Parser::parser_analysis(vector<lexem*> lexems, string output_file_name){
    this->tree.clear();
    error_text = "";
    this->lexems = lexems;
    wasError = !(signal_program());
    make_listing(output_file_name);
    return !wasError;
}



bool Parser::signal_program() {
    int lexem_number = 0, deep = 1;
    tree.push_back(new node(0, "<signal-proogram>", 0));
    if (!program(lexem_number, deep))
        return false;

    if (lexems[++lexem_number]->code != END_OF_FILE_CODE) {
         errorEOF(lexems[lexem_number]->line, lexems[lexem_number]->column, lexems[lexem_number]->name);
        return false;
    }
    
    tree.push_back(new node(0, "</signal-proogram>", 0));
    return true;
}
 
bool Parser::program(int &lexem_number, int deep) {
    tree.push_back(new node(0, "<program>", deep));
    deep++;

    if (lexems[lexem_number]->name == "PROGRAM") {
        tree.push_back(new node(lexems[lexem_number]->code ,lexems[lexem_number]->name, deep));
        if(procedure_identifier(++lexem_number, deep)){
            
            if (lexems[++lexem_number]->name == ";") {
                tree.push_back(new node(lexems[lexem_number]->code ,lexems[lexem_number]->name, deep));
                    
                    if(block(++lexem_number, deep)){
                        if(lexems[++lexem_number]->name == "."){
                            tree.push_back(new node(lexems[lexem_number]->code ,lexems[lexem_number]->name, deep));
                            tree.push_back(new node(0, "</program>", deep - 1));
                            return true;
                        }else {
                            error(lexems[lexem_number]->line, lexems[lexem_number]->column, ".", lexems[lexem_number]->name);
                            return false;
                        } 
                    } else {
                        return false;
                    }
            }else {
                error(lexems[lexem_number]->line, lexems[lexem_number]->column,  ";", lexems[lexem_number]->name);
            }
        }else{
            error(lexems[lexem_number]->line, lexems[lexem_number]->column,  "<procedure-identifier>", lexems[lexem_number]->name);
        }
        

    } else {
        error(lexems[lexem_number]->line, lexems[lexem_number]->column, "PROGRAM", lexems[lexem_number]->name);
    }

    return false;
}

bool Parser::procedure_identifier(int &lexem_number, int deep) {
    tree.push_back(new node(0, "<procedure-identifier>", deep));
    deep++;
    if (identifier(lexem_number, deep)) {
        tree.push_back(new node(0, "</procedure-identifier>", deep - 1));
        return true;
    }
    tree.pop_back();
    return false;
}

bool Parser::identifier(int &lexem_number, int deep) {
    tree.push_back(new node(0, "<identifier>", deep));
    deep++;

    if (lexems[lexem_number]->code >= MIN_IDENTIFIER_CODE) {
        tree.push_back(new node(lexems[lexem_number]->code ,lexems[lexem_number]->name, deep));
        tree.push_back(new node(0, "</identifier>", deep - 1));
        return true;
    }

    tree.pop_back();
    return false;
}

bool Parser::block(int &lexem_number, int deep) {
    tree.push_back(new node(0, "<block>", deep));
    deep++;
    
    if(!declarations(lexem_number, deep)){
        return false;
    }
    if (lexems[++lexem_number]->name == "BEGIN") {
        tree.push_back(new node(lexems[lexem_number]->code ,lexems[lexem_number]->name, deep));
        if(statements_list(++lexem_number, deep)){
           
        }else {
            return false;
        }

        if (lexems[++lexem_number]->name == "END") {
            tree.push_back(new node(lexems[lexem_number]->code ,lexems[lexem_number]->name, deep));
            tree.push_back(new node(0, "</block>", deep - 1));
            return true;
        } else {
            error(lexems[lexem_number]->line, lexems[lexem_number]->column, "END", lexems[lexem_number]->name);
            return false;
        } 

    } else {
        error(lexems[lexem_number]->line, lexems[lexem_number]->column, "BEGIN", lexems[lexem_number]->name);
        return false;
    }
}


bool Parser::declarations(int &lexem_number, int deep){
    tree.push_back(new node(0, "<declarations>", deep));
    deep++;
    if(!label_declarations(lexem_number, deep)) return false;

    tree.push_back(new node(0, "</declarations>", deep - 1));
    return true;
}

bool Parser::label_declarations(int &lexem_number, int deep){
    tree.push_back(new node(0, "<label-declarations>", deep));
    deep++;
    if (lexems[lexem_number]->name == "LABEL") {
        tree.push_back(new node(lexems[lexem_number]->code ,lexems[lexem_number]->name, deep));
        if(unsinged_integer(++lexem_number, deep)){
            if(labels_list(++lexem_number, deep)){
                tree.push_back(new node(lexems[lexem_number]->code ,lexems[lexem_number]->name, deep));
                tree.push_back(new node(0, "</label-declarations>", deep - 1));
                return true;
            }else {
                return false;
            }
        }else {
            error(lexems[lexem_number]->line, lexems[lexem_number]->column, "<unsigned-integer>", lexems[lexem_number]->name);
            return false;
        }
    } else {
        tree.push_back(new node(0, "<empty>", deep));
        tree.push_back(new node(0, "</label-declarations>", deep - 1));
        --lexem_number;
        return true;
    }
}

bool Parser::labels_list(int &lexem_number, int deep){
    tree.push_back(new node(0, "<labels-list>", deep));
    deep++;

    if(lexems[lexem_number]->name == ";"){
        tree.push_back(new node(0, "<empty>", deep));
        tree.push_back(new node(0, "</labels-list>", deep - 1));
        return true;
    }else if(lexems[lexem_number]->name == ","){
        tree.push_back(new node(lexems[lexem_number]->code ,lexems[lexem_number]->name, deep));
        if(unsinged_integer(++lexem_number, deep)){
            bool res = labels_list(++lexem_number, deep);
            if(res) tree.push_back(new node(0, "</labels-list>", deep - 1));
            return res;    
        }else {
            error(lexems[lexem_number]->line, lexems[lexem_number]->column, "<unsigned-integer>", lexems[lexem_number]->name);
            return false;
        }
    }else {
            error(lexems[lexem_number]->line, lexems[lexem_number]->column, "';' or ','", lexems[lexem_number]->name);
            return false;
    }
}


bool Parser::statements_list(int &lexem_number, int deep) {
    tree.push_back(new node(0, "<statement-list>", deep));
    deep++;

    if (is_statement(lexems[lexem_number]->name)) {
        if(statement(lexem_number, deep)){
            tree.push_back(new node(lexems[lexem_number]->code ,lexems[lexem_number]->name, deep));
            bool res = statements_list(++lexem_number, deep);
            if(res) tree.push_back(new node(0, "</statement-list>", deep - 1));
            return res;
        }else{
            return false;
        }
    }else if (is_unsigned_integer(lexem_number) && lexems[lexem_number + 1]->name == ":"){
           if(statement(lexem_number, deep)){
                tree.push_back(new node(lexems[lexem_number]->code ,lexems[lexem_number]->name, deep));
                bool res = statements_list(++lexem_number, deep);
                if(res) tree.push_back(new node(0, "</statement-list>", deep - 1));
                return res;
           }else {
               return false;
           }
    } else {
        tree.push_back(new node(0, "<empty>", deep));
        --lexem_number;
        tree.push_back(new node(0, "</statement-list>", deep - 1));
        return true;
    }
}

bool Parser::statement(int &lexem_number, int deep){
    tree.push_back(new node(0, "<statement>", deep));
    deep++;
     
    if(lexems[lexem_number]->name == "GOTO" || lexems[lexem_number]->name == "IN" || lexems[lexem_number]->name == "OUT"){
         tree.push_back(new node(lexems[lexem_number]->code ,lexems[lexem_number]->name, deep));
         if(unsinged_integer(++lexem_number, deep)){
             if(lexems[++lexem_number]->name == ";"){
                 tree.push_back(new node(0, "</statement>", deep - 1));
                 return true;
             }else {
                error(lexems[lexem_number]->line, lexems[lexem_number]->column, "';'", lexems[lexem_number]->name);
                return false;
             }
        }else {
            error(lexems[lexem_number]->line, lexems[lexem_number]->column, "<unsigned-integer>", lexems[lexem_number]->name);
            return false;
        }
    } else if (lexems[lexem_number]->name == "LINK") {
        tree.push_back(new node(lexems[lexem_number]->code ,lexems[lexem_number]->name, deep));
        if(variable_identifier(++lexem_number, deep)){
           if(lexems[++lexem_number]->name == ","){
                tree.push_back(new node(lexems[lexem_number]->code ,lexems[lexem_number]->name, deep));
                if(unsinged_integer(++lexem_number, deep)){
                    if(lexems[++lexem_number]->name == ";"){
                        tree.push_back(new node(0, "</statement>", deep - 1));
                        return true;
                    }else {
                        error(lexems[lexem_number]->line, lexems[lexem_number]->column, "';'", lexems[lexem_number]->name);
                        return false;
                    }
                }else {
                    error(lexems[lexem_number]->line, lexems[lexem_number]->column, "<unsigned-integer>", lexems[lexem_number]->name);
                    return false;
                }
           }else {
               error(lexems[lexem_number]->line, lexems[lexem_number]->column, ",", lexems[lexem_number]->name);
               return false; 
           }
        }else{
            error(lexems[lexem_number]->line, lexems[lexem_number]->column, "<variable-identifier>", lexems[lexem_number]->name);
            return false; 
        }
        
    }else if(is_unsigned_integer(lexem_number) && lexems[lexem_number + 1]->name == ":") {
            unsinged_integer(lexem_number, deep);
            tree.push_back(new node(lexems[++lexem_number]->code ,lexems[lexem_number]->name, deep));
            bool res = statement(++lexem_number, deep);
            if(res) tree.push_back(new node(0, "</statement>", deep - 1));
            return res;
    }else {
        error(lexems[lexem_number]->line, lexems[lexem_number]->column, "<statement>", lexems[lexem_number]->name);
        lexems.pop_back();
        return false;
    }

}

bool Parser::is_unsigned_integer(int lexem_number){
    return (lexems[lexem_number]->code >= MIN_DIGIT_CODE && lexems[lexem_number]->code < MIN_IDENTIFIER_CODE);
}

bool Parser::unsinged_integer(int &lexem_number, int deep) {
    tree.push_back(new node(0, "<unsigned-integer>", deep));
    deep++;

    if (is_unsigned_integer(lexem_number)) {
        tree.push_back(new node(lexems[lexem_number]->code ,lexems[lexem_number]->name, deep));
        tree.push_back(new node(0, "</unsigned-integer>", deep - 1));
        return true;
    }
    
    tree.pop_back();
    return false;
}

bool Parser::variable_identifier(int &lexem_number, int deep) {
    tree.push_back(new node(0, "<variable-identifier>", deep));
    deep++;

    if (identifier(lexem_number, deep)) {
        tree.push_back(new node(0, "</variable-identifier>", deep - 1));
       return true;
    }

    tree.pop_back();
    return false;
}

bool Parser::is_statement(string lexem_name){
    for(const lexem* lex : statement_words){
        if(lex->name == lexem_name) return true;
    }   
    return false;
}



void Parser::make_listing(string output_file_name) {
	ofstream output_file(output_file_name);
	
	for (node* branch : tree) {
		for (int i = 0; i < branch->deep; i++) {
			output_file << "|   ";
		}

		if (branch->code != 0) {
			output_file << setw(6) << left << branch->code;
		}
		output_file << branch->name << endl;
	}
	output_file << endl << endl;
    if (!error_text.empty()) {
		output_file << error_text << endl << endl;
        cout << error_text << endl;
	}
	output_file.close();
}


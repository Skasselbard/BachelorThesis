#include <iostream>
#include <sstream>

#include "diagnosis.h"
#include "levenshtein.h"
#include "symboltable.h"

const char* identifier_t_names[] = {"module", "enum", "place", "transition", "variable", "sort", "structtype", "function", "constant" };

std::string SymbolTable::best_match;
SymbolTable* SymbolTable::currentScope;
std::set<std::string> SymbolTable::names;

SymbolTable::SymbolTable() {
    current_index = 0;
}

int SymbolTable::variables() {
    return currentScope->current_index;
}

void SymbolTable::pushScope() {
    SymbolTable* newScope = new SymbolTable;
    newScope->parent = currentScope;
    currentScope = newScope;
}

void SymbolTable::popScope() {
    currentScope = currentScope->parent;
}

void SymbolTable::define(std::string name, identifier_t type, YYLTYPE location) {
    currentScope->_define(name, type, location);
}

Symbol * SymbolTable::use(std::string name, YYLTYPE location) {
    return currentScope->_use(name, location);
}

void SymbolTable::_define(std::string name, identifier_t type, YYLTYPE location) {
    if (table[name] != NULL) {
        yylloc = location;

        std::stringstream s;
        s << "redefinition of '" << name << "'; previously defined at " << (filename ? filename : "stdin") << ":" << table[name]->location.first_line << ":" << table[name]->location.first_column;

        yyerror(s.str().c_str());
    } else {
        names.insert(name);
        table[name] = new Identifier(name, type, location);
        if (type == VARIABLE) {
            index[name] = ++current_index;
        } else {
            index[name] = 0;
        }
    }
}

Symbol * SymbolTable::_use(std::string name, YYLTYPE location) {
    if (table[name] != NULL) {
        table[name]->used.push_back(location);
        return index[name];
    } else {
        // symbol is not known here - check the parent scopes
        if (parent != NULL) {
            return parent->_use(name, location);
        }

        yylloc = location;

        std::stringstream s;
        s << "use of undeclared identifier '" << name << "'";

        std::string the_best_match = "";
        int best_score = 10;
        for (std::set<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
            int distance = levenshtein_distance(name, *it);
            if (name != *it and distance < best_score) {
                best_score = distance;
                the_best_match = *it;
            }
        }

        best_match = the_best_match;
        if (the_best_match != "") {
            s << "; did you mean '" << the_best_match << "'?";
        }

        yyerror(s.str().c_str());
        return 0; // will never be reached
    }
}

std::vector<Identifier*> Identifier::identifiers;

Identifier::Identifier(std::string name, identifier_t type, YYLTYPE location) : name(name), type(type), location(location) {
    identifiers.push_back(this);
}

void Identifier::print(FILE *out) {
    for (std::vector<Identifier*>::iterator it = identifiers.begin(); it != identifiers.end(); ++it) {
        fprintf(out, "%s (%s)\n", (*it)->name.c_str(), identifier_t_names[(*it)->type]);
        fprintf(out, "- defined in %d:%d\n", (*it)->location.first_line, (*it)->location.first_column);
        if ((*it)->used.empty()) {
            fprintf(out, "- never used\n");
        } else {
            fprintf(out, "- used in");
            for (int i = 0; i < (*it)->used.size(); ++i) {
                fprintf(out, " %d:%d", (*it)->used[i].first_line, (*it)->used[i].first_column);
            }
            fprintf(out, "\n");
        }
        fprintf(out, "\n");
    }
}


/********************************* new ************************************/
Symbol::Symbol(std::string n, SymbolTab* table, kinds _kind) : kind(_kind) {
    table->add(this);
    char * c = new char[n.length()+1];
    strcpy(c,n.c_str());
    name=c;
}

Symbol::~Symbol() {}

//PlSymbol::PlSymbol(char* txt, UType* _sort): Symbol(txt, PlaceTable, pl), place(NULL), sort(_sort) {}

//PlSymbol::PlSymbol(Place* p, UType* _sort): Symbol(p->name, PlaceTable, pl), place(p), sort(_sort) {}

//TrSymbol::TrSymbol(char* c): Symbol(c, TransitionTable, tr), transition(NULL), vars(NULL), guard(NULL) {}

SoSymbol::SoSymbol(/*char**/std::string name, UType* ty): Symbol(name.c_str(), GlobalTable, so), type(ty) {}

/*StSymbol::StSymbol(char* name): Symbol(name, BuchiTable, st) {
    state = new buchistate(name);
}*///TODO

RcSymbol::RcSymbol(char* name, UType* ty): Symbol(name, GlobalTable, rc), type(ty) {}

VaSymbol::VaSymbol(char* name, UVar* t): Symbol(name, LocalTable, va), var(t) {}

FcSymbol::FcSymbol(char* name, UFunction* f): Symbol(name, GlobalTable, fc), function(f) {}

EnSymbol::EnSymbol(char* name): Symbol(name, GlobalTable, en) {}

IdList::IdList(char* _name, IdList* _next) : name(_name), next(_next) {}

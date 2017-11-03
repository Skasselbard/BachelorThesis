#pragma once

#include <cstdio>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <Frontend/Parser/ParserNet.hh>
#include <Frontend/Parser/unfold.h>

class Identifier;

typedef enum {
    MODULE, ENUM, PLACE, TRANSITION, VARIABLE, SORT, STRUCTTYPE, FUNCTION, CONSTANT
} identifier_t;

class HLSymbolTable {
    public:
        HLSymbolTable();

        static int variables();

        static void pushScope();
        static void popScope();
        static void define(std::string, identifier_t, YYLTYPE);
        static int use(std::string, YYLTYPE);
        static std::string best_match;

    private:
        int current_index;
        
        static HLSymbolTable* currentScope;
        static std::set<std::string> names;

        HLSymbolTable* parent;
        std::map<std::string, Identifier*> table;
        std::map<std::string, int> index;
        void _define(std::string, identifier_t, YYLTYPE);
        int _use(std::string, YYLTYPE);
};


class Identifier {
    public:
        static std::vector<Identifier*> identifiers;
        static void print(FILE *);

        std::string name;
        identifier_t type;
        YYLTYPE location;

        std::vector<YYLTYPE> used;

        Identifier(std::string, identifier_t, YYLTYPE);
};

/***************************************** new **********************************************/

class Symbol;

class SymbolTab {
    public:
        Symbol** table;
        Symbol* lookup(/*char**/std::string);
        void add(Symbol*);
        SymbolTab(unsigned int = 65536);
        ~SymbolTab();
        unsigned int card;
        unsigned int size;
};

extern SymbolTab* PlaceTable, * TransitionTable;

typedef enum {va, en, rc, fc, so, pl, tr, fp, st} kinds;

class Symbol {
    public:
        char* name;
        kinds kind;
        Symbol* next;
        Symbol(char*, SymbolTab*, kinds);
        ~Symbol();
};

/*class PlSymbol: public Symbol {
    public:
        Place* place;
        UType* sort;
        PlSymbol(Place*, UType*);  // LL- places
        PlSymbol(char*, UType*);   // HL-places
};*/

/*class TrSymbol: public Symbol {
    public:
        Transition* transition;
        SymbolTab* vars;
        UExpression* guard;
        TrSymbol(char*);
};*///TODO

class VaSymbol: public Symbol {
    public:
        UVar* var;
        VaSymbol(char*, UVar*);
};

class EnSymbol: public Symbol {
    public:
        int ord;
        UType* type;
        EnSymbol(char*);
};

class FcSymbol: public Symbol {
    public:
        UFunction* function;
        FcSymbol(char*, UFunction*);
};

class RcSymbol: public Symbol {
    public:
        UType* type;
        int index;
        RcSymbol(char*, UType*);
};

class SoSymbol: public Symbol {
    public:
        UType* type;
        SoSymbol(/*char**/ std::string, UType*);
};

class StSymbol: public Symbol {
    public:
//        buchistate* state;
        StSymbol(char*);
};


class IdList {
    public:
        char* name;
        IdList* next;
        IdList(char*, IdList*);
};

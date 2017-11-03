#pragma once

#include <cstdio>
#include <map>
#include <set>
#include <vector>
#include <string>
#include "k.h"
#include "parser.h"
#include "unfold.h"
#include <Core/Dimensions.h>

class Identifier;
class Symbol;

typedef enum {
    MODULE, ENUM, PLACE, TRANSITION, VARIABLE, SORT, STRUCTTYPE, FUNCTION, CONSTANT
} identifier_t;

class SymbolTable {
    public:
        SymbolTable();

        static int variables();

        static void pushScope();
        static void popScope();
        static void define(std::string, identifier_t, YYLTYPE);
        static Symbol * use(std::string, YYLTYPE);
        static std::string best_match;

    private:
        int current_index;
        
        static SymbolTable* currentScope;
        static std::set<std::string> names;

        SymbolTable* parent;
        std::map<std::string, Identifier*> table;
        std::map<std::string, int> index;
        void _define(std::string, identifier_t, YYLTYPE);
        Symbol * _use(std::string, YYLTYPE);
};


class Identifier {
    public:
        static std::vector<Identifier*> identifiers;
        static void print(FILE *);

        std::string name;
        identifier_t type;
        YYLTYPE location;
	Symbol * content;

        std::vector<YYLTYPE> used;

        Identifier(std::string, identifier_t, YYLTYPE);
};

/***************************************** new **********************************************/

class Symbol   // abstract collection of all symbol content classes
{
};
	

class PlSymbol: public Symbol {
    public:
        arrayindex_t place;
        UType* sort;
        PlSymbol(capacity_t, std::string);  // LL- places
        PlSymbol(capacity_t, std::string, UType*);   // HL-places
};

class TrSymbol: public Symbol {
    public:
        arrayindex_t transition;
        SymbolTable* vars;
        UExpression* guard;
        TrSymbol(fairnessAssumption_t, std::string);
};

class VaSymbol: public Symbol {
    public:
        UVar* var;
        VaSymbol(std::string, UVar*);
};

class EnSymbol: public Symbol {
    public:
        int ord;
        UType* type;
        EnSymbol(std::string);
};

class FcSymbol: public Symbol {
    public:
        UFunction* function;
        FcSymbol(std::string, UFunction*);
};

class RcSymbol: public Symbol {
    public:
        UType* type;
        int index;
        RcSymbol(std::string, UType*);
};

class SoSymbol: public Symbol {
    public:
        UType* type;
        SoSymbol(/*char**/ std::string, UType*);
};

class StSymbol: public Symbol {
    public:
//        buchistate* state;
        StSymbol(std::string);
};


//class IdList {
    //public:
        //char* name;
        //IdList* next;
        //IdList(char*, IdList*);
//};

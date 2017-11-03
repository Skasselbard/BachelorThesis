#include <cstdio>
#include <Highlevel/k.h>
#include <Highlevel/unpk.h>
#include <Highlevel/rk.h>
#include <Highlevel/diagnosis.h>
#include <Highlevel/scanner.h>
#include <Highlevel/parser.h>
#include <Highlevel/symboltable.h>
#include <Highlevel/unfold.h>

kc::net root;

void printer(const char *s, kc::uview v)
{
    fprintf(yyout, "%s", s);
}

int main(int argc, char* argv[]) {
    // initialize global variables
    yyin = stdin;
    yyout = stdout;

    // process command line parameters for input and output
    if (argc > 3) {
        yyerror("wrong number of parameters");
    }
    if (argc > 1) {
        filename = argv[1];
        yyin = fopen(argv[1], "r");
        if (yyin == NULL) {
            yyerror("cannot open file for reading");
        }
    }
    if (argc > 2) {
        yyout = fopen(argv[2], "w");
        if (yyout == NULL) {
            yyerror("cannot open file for writing");
        }
    }

    // start the parser
    yyparse();

    //// print the identifiers
    //Identifier::print(yyout);

    //// print the syntax tree
    //root->print();

    //root->rewrite(kc::simplify);

    root->unparse(printer, kc::typecheck);

    fprintf(yyout, "\n");

    // tidy up
    fclose(yyin);
    yylex_destroy();

    return 0;
}

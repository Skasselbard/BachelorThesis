#include <cstdio>
#include <cstdlib>
#include "scanner.h"
#include "diagnosis.h"
#include "symboltable.h"

char* filename = NULL;

void yyerror(char const* msg) {
    // check if an input file is actually open
    if (yyin == NULL || yyout == NULL) {
        fprintf(stderr, "%s%s:%s ", _bold_, (filename ? filename : "stdin"), _c_);
    } else {
        fprintf(stderr, "%s%s:%d:%d:%s ", _bold_, (filename ? filename : "stdin"),
                yylloc.first_line, yylloc.first_column, _c_);
    }

    // print the actual error
    fprintf(stderr, "%s %s\n", _cbad_("error:"), msg);

    // if we have an open file, print an excerpt
    if (filename and yyin != NULL and yyout != NULL) {
        printExcerpt(yylloc);
    } else {
        if (yytext != NULL) {
            fprintf(stderr, "last token: %s\n", yytext);
        }
    }

    exit(1);
}

void printExcerpt(YYLTYPE loc) {
    char* line = NULL;
    size_t len = 0;

    // go back to the beginning of the file
    rewind(yyin);

    fprintf(stderr, "\n");

    // skip lines until error
    for (int i = 0; i < loc.first_line - 1; ++i) {
        getline(&line, &len, yyin);
    }

    // print last line before the error (context)
    fprintf(stderr, "%s%3d%s  %s", _cl_, loc.first_line - 1, _c_, line);

    // get error line
    getline(&line, &len, yyin);

    // print line number
    fprintf(stderr, "%s%3d%s  ", _cl_, loc.first_line, _c_);

    // print error line
    for (int i = 0; i < strlen(line); ++i) {
        // replace tabs by spaces
        if (line[i] == '\t') {
            fprintf(stderr, " ");
            continue;
        }

        if (i >= loc.first_column - 1 && i <= loc.last_column - 1) {
            fprintf(stderr, "%s%c%s", _cr_, line[i], _c_);
        } else {
            fprintf(stderr, "%c", line[i]);
        }
    }


    fprintf(stderr, "     ");
    // print arrow indicating the error
    for (int i = 0; i < strlen(line); ++i) {
        // the beginning
        if (i == loc.first_column - 1) {
            fprintf(stderr, "%s", _cgood_("^"));
        } else {
            // following error characters
            if (i > loc.first_column - 1 && i <= loc.last_column - 1) {
                fprintf(stderr, "%s", _cgood_("~"));
            } else {
                fprintf(stderr, " ");
            }
        }
    }

    // if we have a best matching alternative, print it
    if (SymbolTable::best_match != "") {
        fprintf(stderr, "\n     ");
        for (int i = 0; i < loc.first_column - 1; ++i) {
            fprintf(stderr, " ");
        }
        fprintf(stderr, "%s", _cgood_(SymbolTable::best_match));
    }

    fprintf(stderr, "\n\n");
}

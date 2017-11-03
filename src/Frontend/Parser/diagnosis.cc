#include <cstdio>
#include <cstdlib>
#include <InputOutput/Reporter.h>
#include <Frontend/Parser/ast-system-k.h>
#include <Frontend/Parser/ast-system-yystype.h>
#include <Frontend/Parser/diagnosis.h>
#include <Frontend/Parser/symboltable.h>
#include <Frontend/Parser/ParserNet.hh>

extern FILE * ptnetlola_in, * ptnetlola_out;
extern char * ptnetlola_text;
char* filename = NULL;

void ptnetlola_error(char const* msg) {
    // check if an input file is actually open
    if (ptnetlola_in == NULL || ptnetlola_out == NULL) {
        fprintf(stderr, "%s%s:%s ", _bold_, (filename ? filename : "stdin"), _c_);
    } else {
        fprintf(stderr, "%s%s:%d:%d:%s ", _bold_, (filename ? filename : "stdin"),
                ptnetlola_lloc.first_line, ptnetlola_lloc.first_column, _c_);
    }

    // print the actual error
    fprintf(stderr, "%s %s\n", _cbad_("error:"), msg);

    // if we have an open file, print an excerpt
    if (filename and ptnetlola_in != NULL and ptnetlola_out != NULL) {
        printExcerpt(ptnetlola_lloc);
    } else {
        if (ptnetlola_text != NULL) {
            fprintf(stderr, "last token: %s\n", ptnetlola_text);
        }
    }

    exit(1);
}

void printExcerpt(YYLTYPE loc) {
    char* line = NULL;
    size_t len = 0;

    // go back to the beginning of the file
    rewind(ptnetlola_in);

    fprintf(stderr, "\n");

    // skip lines until error
    for (int i = 0; i < loc.first_line - 1; ++i) {
        getline(&line, &len, ptnetlola_in);
    }

    // print last line before the error (context)
    fprintf(stderr, "%s%3d%s  %s", _cl_, loc.first_line - 1, _c_, line);

    // get error line
    getline(&line, &len, ptnetlola_in);

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
    if (HLSymbolTable::best_match != "") {
        fprintf(stderr, "\n     ");
        for (int i = 0; i < loc.first_column - 1; ++i) {
            fprintf(stderr, " ");
        }
        fprintf(stderr, "%s", _cgood_(HLSymbolTable::best_match));
    }

    fprintf(stderr, "\n\n");
}

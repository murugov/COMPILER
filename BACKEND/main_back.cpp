#include "translator.hpp"


int main()                  // argc/argv
{
    LogFileOpener(PATH_TO_LOGFILE);

    trans_t *trans = (trans_t*)calloc(1, sizeof(trans_t));
    transCtor(trans, "reports/ast.txt");

    TranslateTree(trans, "src/CompileFiles/source.asm");

    printf(ANSI_COLOR_GREEN "Successfully translated\n" ANSI_COLOR_RESET);

    transDtor(trans);
    LogFileCloser();

    return 0;
}
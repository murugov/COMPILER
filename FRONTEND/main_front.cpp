#include "parser.hpp"
#include "ast_parser.hpp"


int main(int argc, char *argv[]) 
{
    open_log_file(PATH_TO_LOGFILE);

    const char *file_name = (argc > 1) ? argv[1] : "src/data.txt";

    parser_t *parser = (parser_t*)calloc(1, sizeof(parser_t));

    FILE *src_file = fopen(file_name, "r");
    if (src_file == NULL) { printf(ANSI_COLOR_RED "Error: Cannot open file %s\n" ANSI_COLOR_RESET, file_name); return FRONT_ERROR; }

    parser_init(parser, src_file);

    node_t *ast = parse_ast(parser);
    if (ast)
    {
        printf(ANSI_COLOR_GREEN "Successfully parsed\n" ANSI_COLOR_RESET);
        dump_tree(ast, __func__);
        FILE* report = fopen("reports/ast.txt", "w");
        ast_writer(ast, report);
        fclose(report);
        node_purge(ast);
    }
    else
        printf(ANSI_COLOR_RED "Parsing failed!\n" ANSI_COLOR_RESET);    
    
    parser_free(parser);
    close_log_file();

    return 0;
}
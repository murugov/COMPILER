#include "ast_parser.hpp"
#include "find_file_size.hpp"


static const char* arg_type_set[] = {"LINK", "NUM", "OP" , "VAR", "FUNC"};
static int LEN_ARG_TYPE_SET = sizeof(arg_type_set) / sizeof(arg_type_set[0]);

static int check_type(const char * __restrict type);


char* ast_reader(FILE *src_file)
{    
    const ssize_t file_size = find_file_size(src_file);
    if (file_size < 0) { return NULL; }

    char *buffer = (char*)calloc((size_t)file_size + 1, sizeof(char));
    if (buffer == NULL) { fclose(src_file); return NULL; }

    const size_t len_buffer = fread(buffer, sizeof(char), (size_t)file_size, src_file);
    buffer[len_buffer] = '\0';

    fclose(src_file);

    return buffer;
}

#define SKIP_SPACES(ptr) while (isspace((int)*(ptr))) (ptr)++
#define SKIP_TYPE(ptr) while (*(ptr) != ':' && *(ptr) != '\0') (ptr)++
#define SKIP_VALUE(ptr) while (*(ptr) != '\"' && *(ptr) != '\0') (ptr)++

node_t* node_reader(char ** __restrict cur_ptr, node_t * __restrict parent)
{
    if (cur_ptr == NULL || *cur_ptr == NULL) { return NULL; }
    
    char *cur_pos = *cur_ptr;
    SKIP_SPACES(cur_pos);

    if (*cur_pos == '(')
    {
        cur_pos++;
        SKIP_SPACES(cur_pos);

        char *type_start = cur_pos;
        SKIP_TYPE(cur_pos);

        if (*cur_pos == '\0')
        {
            printf(ANSI_COLOR_RED "Unexpected end\n" ANSI_COLOR_RESET);
            return NULL;
        }
        
        char saved_char = *cur_pos;
        *cur_pos = '\0';
        type_t type = (type_t)check_type(type_start);
        *cur_pos = saved_char;
        
        if (type < 0)
        {
            printf(ANSI_COLOR_RED "Unknown type '%s'\n" ANSI_COLOR_RESET, type_start);
            return NULL;
        }
        
        cur_pos++;
        SKIP_SPACES(cur_pos);
        
        if (*cur_pos != '"')
        {
            printf(ANSI_COLOR_RED "Expected '\"' after type\n" ANSI_COLOR_RESET);
            return NULL;
        }
        cur_pos++;
        
        char *val_start = cur_pos;
        SKIP_VALUE(cur_pos);
        
        if (*cur_pos == '\0')
        {
            printf(ANSI_COLOR_RED "Unexpected EOF in value\n" ANSI_COLOR_RESET);
            return NULL;
        }

        saved_char = *cur_pos;
        *cur_pos = '\0';

        val value = {};                         // ???
        switch (type)
        {
            case ARG_LINK:
                value.link = *val_start;
                break;
                
            case ARG_OP:
                value.op = hash_func_str(val_start);
                break;

            case ARG_VAR:
                value.var = strdup(val_start);
                break;
            
            case ARG_NUM:
                value.num = atoi(val_start);
                break;

            case ARG_FUNC:
                value.func = strdup(val_start);
                break;

            default:
                break;
        }

        *cur_pos = saved_char;
        cur_pos++;
        
        node_t *new_node = node_new((type_t)type, value, NULL, NULL);
        if (new_node == NULL) { return NULL; }
        
        new_node->parent = parent;
        *cur_ptr = cur_pos;
        
        new_node->left  = node_reader(cur_ptr, new_node);
        new_node->right = node_reader(cur_ptr, new_node);
        
        cur_pos = *cur_ptr;
        SKIP_SPACES(cur_pos);
        
        if (*cur_pos == ')')
        {
            cur_pos++;
        }
        else
        {
            printf(ANSI_COLOR_RED "Expected ')'\n" ANSI_COLOR_RESET);
            node_purge(new_node);
            return NULL;
        }
        
        *cur_ptr = cur_pos;
        return new_node;
    }
    else if (strncmp(cur_pos, "nil", 3) == 0)
    {
        cur_pos += 3;
        SKIP_SPACES(cur_pos);
        *cur_ptr = cur_pos;
        
        return NULL;
    }
    
    printf(ANSI_COLOR_RED "Unrecognized format at '%c'\n" ANSI_COLOR_RESET, *cur_pos);
    return NULL;
}

#undef SKIP_SPACES
#undef SKIP_TYPE
#undef SKIP_VALUE


static int check_type(const char * __restrict type)
{
    for (int i = 0; i < LEN_ARG_TYPE_SET; i++)
    {
        if (strcmp(type, arg_type_set[i]) == 0) { return i; }
    }
    
    printf(ANSI_COLOR_RED "Unrecognized type '%s'\n" ANSI_COLOR_RESET, type);
    return -1;
}
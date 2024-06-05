#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum
{
    TOKEN_NUMBER,
    TOKEN_ID,
    TOKEN_OP,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_KEYWORD,
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_PREPROCESSOR,
    TOKEN_EOF,
    TOKEN_UNKNOWN
} TokenType;

typedef struct
{
    TokenType type;
    char *value;
    int line_no;
} Token;

const char *token_type_strings[] = {
    "NUMBER",
    "ID",
    "OP",
    "LPAREN",
    "RPAREN",
    "LBRACE",
    "RBRACE",
    "LBRACKET",
    "RBRACKET",
    "SEMICOLON",
    "COMMA",
    "KEYWORD",
    "STRING",
    "CHAR",
    "PREPROCESSOR",
    "EOF",
    "UNKNOWN"};

const char *keywords[] = {"int", "float", "char", "double", "void", "if", "else", "for", "while", "return", "include", "define"};
const char *operators[] = {"+", "-", "*", "/", "%", "=", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!", "&", "|", "^", "~", "<<", ">>", "++", "--"};

Token get_next_token();
Token peek_next_token();
void match(TokenType expected);
void program();
void external_declaration();
void function_definition();
void parameter_list();
void parameter();
void declaration();
void statement();
void expression_statement();
void return_statement();
void expression();
void term();
void factor();
void parse();
void for_statement();
void while_statement();

Token *tokens;
int current_token = 0;
int token_count = 0;

int is_keyword(const char *str)
{
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
    {
        if (strcmp(str, keywords[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int is_operator(char c)
{
    for (int i = 0; i < sizeof(operators) / sizeof(operators[0]); i++)
    {
        if (strchr(operators[i], c))
        {
            return 1;
        }
    }
    return 0;
}

Token *tokenize(const char *code, int *token_count)
{
    Token *tokens = malloc(sizeof(Token) * 2048); // Example size, adjust as needed
    *token_count = 0;
    int line_no = 1;

    for (int i = 0; i < strlen(code); i++)
    {
        char c = code[i];
        if (isdigit(c))
        {
            // Handle numbers
            int start = i;
            while (isdigit(code[i]))
                i++;
            char *num = strndup(code + start, i - start);
            tokens[(*token_count)++] = (Token){TOKEN_NUMBER, num, line_no};
            i--;
        }
        else if (isalpha(c) || c == '_')
        {
            // Handle identifiers and keywords
            int start = i;
            while (isalnum(code[i]) || code[i] == '_')
                i++;
            char *id = strndup(code + start, i - start);
            if (is_keyword(id))
            {
                tokens[(*token_count)++] = (Token){TOKEN_KEYWORD, id, line_no};
            }
            else
            {
                tokens[(*token_count)++] = (Token){TOKEN_ID, id, line_no};
            }
            i--;
        }
        else if (c == '"')
        {
            // Handle string literals
            int start = ++i;
            while (code[i] != '"')
                i++;
            char *str = strndup(code + start, i - start);
            tokens[(*token_count)++] = (Token){TOKEN_STRING, str, line_no};
        }
        else if (c == '\'')
        {
            // Handle char literals
            int start = ++i;
            while (code[i] != '\'')
                i++;
            char *ch = strndup(code + start, i - start);
            tokens[(*token_count)++] = (Token){TOKEN_CHAR, ch, line_no};
        }
        else if (c == '#')
        {
            // Handle preprocessor directives
            int start = i;
            while (code[i] != '\n' && code[i] != '\0')
                i++;
            char *directive = strndup(code + start, i - start);
            tokens[(*token_count)++] = (Token){TOKEN_PREPROCESSOR, directive, line_no};
            line_no++;
        }
        else if (c == '(')
        {
            tokens[(*token_count)++] = (Token){TOKEN_LPAREN, strndup("(", 1), line_no};
        }
        else if (c == ')')
        {
            tokens[(*token_count)++] = (Token){TOKEN_RPAREN, strndup(")", 1), line_no};
        }
        else if (c == '{')
        {
            tokens[(*token_count)++] = (Token){TOKEN_LBRACE, strndup("{", 1), line_no};
        }
        else if (c == '}')
        {
            tokens[(*token_count)++] = (Token){TOKEN_RBRACE, strndup("}", 1), line_no};
        }
        else if (c == '[')
        {
            tokens[(*token_count)++] = (Token){TOKEN_LBRACKET, strndup("[", 1), line_no};
        }
        else if (c == ']')
        {
            tokens[(*token_count)++] = (Token){TOKEN_RBRACKET, strndup("]", 1), line_no};
        }
        else if (c == ';')
        {
            tokens[(*token_count)++] = (Token){TOKEN_SEMICOLON, strndup(";", 1), line_no};
        }
        else if (c == ',')
        {
            tokens[(*token_count)++] = (Token){TOKEN_COMMA, strndup(",", 1), line_no};
        }
        else if (c == '\n')
        {
            line_no++;
        }
        else if (isspace(c))
        {
            continue;
        }
        else if (is_operator(c))
        {
            // Handle operators
            int start = i;
            while (is_operator(code[i]))
                i++;
            char *op = strndup(code + start, i - start);
            tokens[(*token_count)++] = (Token){TOKEN_OP, op, line_no};
            i--;
        }
        else
        {
            tokens[(*token_count)++] = (Token){TOKEN_UNKNOWN, strndup(&c, 1), line_no};
        }
    }

    tokens[(*token_count)++] = (Token){TOKEN_EOF, NULL, line_no};
    return tokens;
}

void print_tokens(Token *tokens, int token_count)
{
    for (int i = 0; i < token_count; i++)
    {
        printf("<%s, %s> line %d\n", token_type_strings[tokens[i].type], tokens[i].value ? tokens[i].value : "null", tokens[i].line_no);
    }
}

char *read_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = malloc(length + 1);
    if (!buffer)
    {
        perror("Failed to allocate buffer");
        exit(EXIT_FAILURE);
    }
    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);
    return buffer;
}

// Syntax Analyser Part
Token get_next_token()
{
    return tokens[current_token++];
}

Token peek_next_token()
{
    return tokens[current_token];
}

void parse()
{
    current_token = 0;
    program();
    if (tokens[current_token].type == TOKEN_EOF)
    {
        printf("Syntax analysis successful\n");
    }
    else
    {
        fprintf(stderr, "Syntax analysis failed: Did not reach EOF\n");
    }
}

void program()
{
    while (tokens[current_token].type != TOKEN_EOF)
    {
        external_declaration();
    }
}

void external_declaration()
{
    printf("<%s, %s> \n", token_type_strings[tokens[current_token].type], tokens[current_token].value ? tokens[current_token].value : "null");
    int type = tokens[current_token].type;
    if (type == TOKEN_KEYWORD)
    {
        if (tokens[current_token + 2].type == TOKEN_LPAREN)
        {
            function_definition();
        }
        else
        {
            declaration();
        }
    }
    else if (type == TOKEN_PREPROCESSOR)
    {
        match(TOKEN_PREPROCESSOR);
    }
    else
    {
        fprintf(stderr, "Syntax error at line %d\n", tokens[current_token].line_no);
        exit(EXIT_FAILURE);
    }
}

void function_definition()
{
    match(TOKEN_KEYWORD);
    match(TOKEN_ID);
    match(TOKEN_LPAREN);
    if (tokens[current_token].type != TOKEN_RPAREN)
    {
        parameter_list();
    }
    match(TOKEN_RPAREN);
    match(TOKEN_LBRACE);
    while (tokens[current_token].type != TOKEN_RBRACE)
    {
        statement();
    }
    match(TOKEN_RBRACE);
}

void declaration()
{
    match(TOKEN_KEYWORD);
    match(TOKEN_ID);
    while (tokens[current_token].type == TOKEN_COMMA)
    {
        match(TOKEN_COMMA);
        match(TOKEN_ID);
    }
    match(TOKEN_SEMICOLON);
}

void match(TokenType expected)
{
    printf("From match: \n");
    printf("<%s, %s> \n", token_type_strings[tokens[current_token].type], tokens[current_token].value ? tokens[current_token].value : "null");
    if (tokens[current_token].type == expected)
    {
        current_token++;
    }
    else
    {
        fprintf(stderr, "Syntax error: Expected %s but got %s at line %d\n", token_type_strings[expected], token_type_strings[tokens[current_token].type], tokens[current_token].line_no);
        exit(EXIT_FAILURE);
    }
}

void parameter_list()
{
    parameter();
    while (tokens[current_token].type == TOKEN_COMMA)
    {
        match(TOKEN_COMMA);
        parameter();
    }
}

void parameter()
{
    match(TOKEN_KEYWORD);
    match(TOKEN_ID);
}

void statement()
{
    printf("From statement: \n");
    printf("<%s, %s> \n", token_type_strings[tokens[current_token].type], tokens[current_token].value ? tokens[current_token].value : "null");
    if (tokens[current_token].type == TOKEN_KEYWORD)
    {
        if (strcmp(tokens[current_token].value, "return") == 0)
        {
            return_statement();
        }
        else if (strcmp(tokens[current_token].value, "for") == 0)
        {
            for_statement();
        }
        else if (strcmp(tokens[current_token].value, "while") == 0)
        {
            while_statement();
        }
        else
        {
            printf("going to expression_statement\n");
            expression_statement();
        }
    }
    else
    {
        expression_statement();
    }
}

void for_statement()
{
    printf("From for_statement: \n");
    match(TOKEN_KEYWORD); // match 'for'
    match(TOKEN_LPAREN);

    if (tokens[current_token].type != TOKEN_SEMICOLON)
    {
        expression();
    }
    match(TOKEN_SEMICOLON);

    if (tokens[current_token].type != TOKEN_SEMICOLON)
    {
        expression();
    }
    match(TOKEN_SEMICOLON);

    if (tokens[current_token].type != TOKEN_RPAREN)
    {
        expression();
    }
    match(TOKEN_RPAREN);

    match(TOKEN_LBRACE);
    while (tokens[current_token].type != TOKEN_RBRACE)
    {
        statement();
    }
    match(TOKEN_RBRACE);
}

void while_statement()
{
    match(TOKEN_KEYWORD); // match 'while'
    match(TOKEN_LPAREN);
    expression();
    match(TOKEN_RPAREN);
    match(TOKEN_LBRACE);
    while (tokens[current_token].type != TOKEN_RBRACE)
    {
        statement();
    }
    match(TOKEN_RBRACE);
}

void expression_statement()
{
    expression();
    match(TOKEN_SEMICOLON);
}

void return_statement()
{
    match(TOKEN_KEYWORD);
    expression();
    match(TOKEN_SEMICOLON);
}

void expression()
{
    term();
    while (tokens[current_token].type == TOKEN_OP)
    {
        match(TOKEN_OP);
        term();
    }
}

void term()
{
    printf("From term: \n");
    factor();
    while (tokens[current_token].type == TOKEN_OP && (strcmp(tokens[current_token].value, "*") == 0 || strcmp(tokens[current_token].value, "/") == 0))
    {
        match(TOKEN_OP);
        factor();
    }
}

void factor()
{
    printf("From factor: \n");
    printf("<%s, %s> \n", token_type_strings[tokens[current_token].type], tokens[current_token].value ? tokens[current_token].value : "null");
    if (tokens[current_token].type == TOKEN_KEYWORD)
    {
        current_token++;
    }
    if (tokens[current_token].type == TOKEN_NUMBER)
    {
        match(TOKEN_NUMBER);
    }
    else if (tokens[current_token].type == TOKEN_ID)
    {
        match(TOKEN_ID);
    }
    else if (tokens[current_token].type == TOKEN_LPAREN)
    {
        match(TOKEN_LPAREN);
        expression();
        match(TOKEN_RPAREN);
    }
    else
    {
        fprintf(stderr, "Syntax error at line %d: Unexpected token %s\n", tokens[current_token].line_no, token_type_strings[tokens[current_token].type]);
        exit(EXIT_FAILURE);
    }
}

int main()
{
    const char *filename = "input.c";
    char *code = read_file(filename);

    // Use the global tokens array
    tokens = tokenize(code, &token_count);

    print_tokens(tokens, token_count);
    parse();

    // Free allocated memory
    for (int i = 0; i < token_count; i++)
    {
        free(tokens[i].value);
    }
    free(tokens);
    free(code);

    return 0;
}

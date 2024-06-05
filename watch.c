#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

typedef enum
{
    DATA_TYPES,
    ID,
    OP,
    KEYWORDS,
    NUM,
    STRING,
    CHAR,
    COMMENT,
    PUNCTUATORS,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    SEMICOLON,
    COMMA,
    COLON,
    DOT,
    PREPROCESSOR,
    T_EOF, // Changed from EOF to T_EOF to avoid conflict with standard EOF
    UNKNOWN,
    LOOP,
    CONDITIONAL,
    FUNCTION,
    INPUTS,
    OUTPUTS,
    ASSIGNMENT
} TokenType;

typedef struct
{
    TokenType type;
    char *value;
    int line_no;
} Token;

Token *tokens;
int tokenCount = 0;
int currentToken = 0;

const char *token_type_strings[] = {
    "DATA_TYPES",
    "ID",
    "OP",
    "KEYWORDS",
    "NUM",
    "STRING",
    "CHAR",
    "COMMENT",
    "PUNCTUATORS",
    "LPAREN",
    "RPAREN",
    "LBRACE",
    "RBRACE",
    "LBRACKET",
    "RBRACKET",
    "SEMICOLON",
    "COMMA",
    "COLON",
    "DOT",
    "PREPROCESSOR",
    "T_EOF",
    "UNKNOWN",
    "LOOP",
    "CONDITIONAL",
    "FUNCTION",
    "INPUTS",
    "OUTPUTS",
    "ASSIGNMENT"};

const char *keywords[] = {
    "auto", "break", "case", "const", "continue", "default", "do", "enum", "goto", "return"};

const char *data_types[] = {
    "int", "char", "float", "double", "void", "long long", "long", "short", "unsigned", "signed"};


const char *loops[] = {
    "for", "while", "do"};

const char *conditionals[] = {
    "if", "else", "switch", "case", "default"};

const char *inputs[] = {
    "scanf", "getchar", "getch", "getche"};

const char *outputs[] = {
    "printf", "putchar", "puts"};

const char *functions[] = {
    "strcmp", "strcpy"};


// Token next();
// Token peek();
// void match(TokenType expected);
// void program();
// void external_declaration();
// void function_definition();
// void parameter_list();
// void parameter();
// void declaration();
// void statement();
// void expression_statement();
// void return_statement();
// void expression();
// void term();
// void factor();
// void parse();
// void for_statement();
// void while_statement();
// void do_while_statement();
// void if_statement();
// void else_statement();
// void switch_statement();
// void case_statement();
// void default_statement();

bool isKeyword(char *str)
{
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
    {
        if (strcmp(str, keywords[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

bool isDataType(char *str)
{
    for (int i = 0; i < sizeof(data_types) / sizeof(data_types[0]); i++)
    {
        if (strcmp(str, data_types[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

int isOperator(const char *str)
{
    const char *operators[] = {
        "+", "-", "*", "/", "%", "++", "--", "==", "!=", ">", "<", ">=", "<=", "&&", "||", "!", "&", "|", "^", "~", "<<", ">>", "sizeof", "?:"};
    size_t n = sizeof(operators) / sizeof(operators[0]);
    for (size_t i = 0; i < n; i++)
    {
        if (strncmp(str, operators[i], strlen(operators[i])) == 0)
        {
            return 1;
        }
    }
    return 0;
}

bool isLoop(char *str)
{
    for (int i = 0; i < sizeof(loops) / sizeof(loops[0]); i++)
    {
        if (strcmp(str, loops[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

bool isConditional(char *str)
{
    for (int i = 0; i < sizeof(conditionals) / sizeof(conditionals[0]); i++)
    {
        if (strcmp(str, conditionals[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

bool isFunction(char *str)
{
    for (int i = 0; i < sizeof(functions) / sizeof(functions[0]); i++)
    {
        if (strcmp(str, functions[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

bool isInput(char *str)
{
    for (int i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++)
    {
        if (strcmp(str, inputs[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

bool isOutput(char *str)
{
    for (int i = 0; i < sizeof(outputs) / sizeof(outputs[0]); i++)
    {
        if (strcmp(str, outputs[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

int isAssignment(const char *str)
{
    const char *assignments[] = {
        "=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>="};
    size_t n = sizeof(assignments) / sizeof(assignments[0]);
    for (size_t i = 0; i < n; i++)
    {
        if (strncmp(str, assignments[i], strlen(assignments[i])) == 0)
        {
            return 1;
        }
    }
    return 0;
}

Token *tokenize(const char *code, int *tokenCount)
{
    Token *tokens = malloc(sizeof(Token) * 2048);
    *tokenCount = 0;
    int lineNo = 1;

    for (int i = 0; i < strlen(code); i++)
    {
        char c = code[i];
        if (c == '\n')
        {
            lineNo++;
            continue;
        }
        if (c == ' ')
            continue;

        if (isdigit(c))
        {
            int start = i;
            while (isdigit(code[i]))
            {
                i++;
            }
            char *num = strndup(code + start, i - start);
            tokens[(*tokenCount)++] = (Token){NUM, num, lineNo};
            i--;
        }
        else if (isalpha(c) || c == '_')
        {
            int start = i;
            while (isalnum(code[i]) || code[i] == '_')
            {
                i++;
            }
            char *id = strndup(code + start, i - start);
            if (isKeyword(id))
            {
                tokens[(*tokenCount)++] = (Token){KEYWORDS, id, lineNo};
            }
            else if (isDataType(id))
            {
                tokens[(*tokenCount)++] = (Token){DATA_TYPES, id, lineNo};
            }
            else if (isLoop(id))
            {
                tokens[(*tokenCount)++] = (Token){LOOP, id, lineNo};
            }
            else if (isConditional(id))
            {
                tokens[(*tokenCount)++] = (Token){CONDITIONAL, id, lineNo};
            }
            else if (isFunction(id))
            {
                tokens[(*tokenCount)++] = (Token){FUNCTION, id, lineNo};
            }
            else if (isInput(id))
            {
                tokens[(*tokenCount)++] = (Token){INPUTS, id, lineNo};
            }
            else if (isOutput(id))
            {
                tokens[(*tokenCount)++] = (Token){OUTPUTS, id, lineNo};
            }
            else if (isAssignment(id))
            {
                tokens[(*tokenCount)++] = (Token){ASSIGNMENT, id, lineNo};
            }
            else
            {
                tokens[(*tokenCount)++] = (Token){ID, id, lineNo};
            }
            i--;
        }
        else if (isAssignment(code + i))
        {
            int start = i;
            while (isAssignment(code + i))
            {
                i++;
            }
            char *op = strndup(code + start, i - start);
            tokens[(*tokenCount)++] = (Token){ASSIGNMENT, op, lineNo};
            i--;
        }
        else if (isOperator(code + i))
        {
            int start = i;
            while (isOperator(code + i))
            {
                i++;
            }
            char *op = strndup(code + start, i - start);
            tokens[(*tokenCount)++] = (Token){OP, op, lineNo};
            i--;
        }
        else if (c == '"')
        {
            int start = i + 1;
            while (code[i + 1] != '"' && code[i + 1] != '\0')
            {
                i++;
            }
            char *str = strndup(code + start, i - start + 1);
            tokens[(*tokenCount)++] = (Token){STRING, str, lineNo};
            i++;
        }
        else if (c == '\'')
        {
            int start = ++i;
            while (code[i] != '\'' && code[i] != '\0')
            {
                i++;
            }
            char *str = strndup(code + start, i - start);
            tokens[(*tokenCount)++] = (Token){CHAR, str, lineNo};
        }
        else if (c == '#')
        {
            int start = i;
            while (code[i] != '\n' && code[i] != '\0')
            {
                i++;
            }
            char *directive = strndup(code + start, i - start);
            tokens[(*tokenCount)++] = (Token){PREPROCESSOR, directive, lineNo};
        }
        else if (c == '(')
        {
            tokens[(*tokenCount)++] = (Token){LPAREN, strdup("("), lineNo};
        }
        else if (c == ')')
        {
            tokens[(*tokenCount)++] = (Token){RPAREN, strdup(")"), lineNo};
        }
        else if (c == '{')
        {
            tokens[(*tokenCount)++] = (Token){LBRACE, strdup("{"), lineNo};
        }
        else if (c == '}')
        {
            tokens[(*tokenCount)++] = (Token){RBRACE, strdup("}"), lineNo};
        }
        else if (c == '[')
        {
            tokens[(*tokenCount)++] = (Token){LBRACKET, strdup("["), lineNo};
        }
        else if (c == ']')
        {
            tokens[(*tokenCount)++] = (Token){RBRACKET, strdup("]"), lineNo};
        }
        else if (c == ';')
        {
            tokens[(*tokenCount)++] = (Token){SEMICOLON, strdup(";"), lineNo};
        }
        else if (c == ',')
        {
            tokens[(*tokenCount)++] = (Token){COMMA, strdup(","), lineNo};
        }
        else if (c == ':')
        {
            tokens[(*tokenCount)++] = (Token){COLON, strdup(":"), lineNo};
        }
        else if (c == '.')
        {
            tokens[(*tokenCount)++] = (Token){DOT, strdup("."), lineNo};
        }
        else if (isOperator(&c))
        {
            int start = i;
            while (isOperator(&code[i]))
            {
                i++;
            }
            char *op = strndup(code + start, i - start);
            tokens[(*tokenCount)++] = (Token){OP, op, lineNo};
            i--;
        }
        else if (c == '/')
        {
            if (code[i + 1] == '/')
            {
                while (code[i] != '\n' && code[i] != '\0')
                {
                    i++;
                }
                // Skip the comment and do not add to token stream
            }
            else if (code[i + 1] == '*')
            {
                while (!(code[i] == '*' && code[i + 1] == '/'))
                {
                    i++;
                }
                i += 2;
                // Skip the comment and do not add to token stream
            }
            else
            {
                printf("Unknown token: %c\n", c);
                char *unknown = malloc(2);
                unknown[0] = c;
                unknown[1] = '\0';
                tokens[(*tokenCount)++] = (Token){UNKNOWN, unknown, lineNo};
            }
        }
        else
        {
            if(isAssignment(&c)){
                int start = i;
                while (isAssignment(code + i))
                {
                    i++;
                }
                char *op = strndup(code + start, i - start);
                tokens[(*tokenCount)++] = (Token){ASSIGNMENT, op, lineNo};
                i--;
            }else if(isOperator(&c)){
                int start = i;
                while (isOperator(&code[i]))
                {
                    i++;
                }
                char *op = strndup(code + start, i - start);
                tokens[(*tokenCount)++] = (Token){OP, op, lineNo};
                i--;
            }else{
            char *unknown = malloc(2);
            unknown[0] = c;
            unknown[1] = '\0';
            tokens[(*tokenCount)++] = (Token){UNKNOWN, unknown, lineNo};
            }
        }
    }

    tokens[(*tokenCount)++] = (Token){T_EOF, strdup("EOF"), lineNo};
    return tokens;
}

void print_tokens(Token *tokens, int tokenCount)
{
    for (int i = 0; i < tokenCount; i++)
    {
        printf("<%s, %s, %d>\n", token_type_strings[tokens[i].type], tokens[i].value, tokens[i].line_no);
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

// Syntax Analysis
// Token next()
// {
//     return tokens[currentToken++];
// }

// Token peek()
// {
//     return tokens[currentToken];
// }

// void parse()
// {
//     currentToken = 0;
//     program();
//     if (tokens[currentToken].type == T_EOF)
//     {
//         printf("Parsing successful\n");
//     }
//     else
//     {
//         printf("Parsing failed\n");
//     }
// }

// void program()
// {
//     while (tokens[currentToken].type != T_EOF)
//     {
//         external_declaration();
//     }
// }

// void external_declaration()
// {
//     if (tokens[currentToken].type == DATA_TYPES)
//     {
//         if (tokens[currentToken + 2].type == LPAREN)
//         {
//             function_definition();
//         }
//         else
//         {
//             declaration();
//         }
//     }
//     else if (tokens[currentToken].type == PREPROCESSOR)
//     {
//         match(PREPROCESSOR);
//     }
// }

// void function_definition()
// {
//     match(DATA_TYPES);
//     match(ID);
//     match(LPAREN);
//     while (tokens[currentToken].type != RBRACE)
//     {
//         parameter_list();
//     }
//     match(RPAREN);
//     match(LBRACE);
//     while (tokens[currentToken].type != RBRACE)
//     {
//         statement();
//     }
//     match(RBRACE);
// }

// void parameter_list()
// {
//     parameter();
//     while (tokens[currentToken].type == COMMA)
//     {
//         match(COMMA);
//         parameter();
//     }
// }

// void parameter()
// {
//     match(DATA_TYPES);
//     match(ID);
// }

// void declaration()
// {
//     int type = tokens[currentToken].type;
//     if (type == DATA_TYPES)
//     {
//         match(DATA_TYPES);
//         match(ID);
//         while (tokens[currentToken].type == COMMA)
//         {
//             match(COMMA);
//             match(ID);
//         }
//     }
// }

int main()
{
    const char *filename = "input.c";
    char *code = read_file(filename);

    // Use the global tokens array
    tokens = tokenize(code, &tokenCount);

    print_tokens(tokens, tokenCount);
    // parse();

    // Free allocated memory
    for (int i = 0; i < tokenCount; i++)
    {
        free(tokens[i].value);
    }
    free(tokens);
    free(code);

    return 0;
}

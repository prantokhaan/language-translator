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
    T_EOF, 
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

typedef struct Symbol{
    char *name;
    char *type;
    struct Symbol *next;
} Symbol;

Symbol *symbolTable = NULL;




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
    "ASSIGNMENT"
};

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
    "strcmp", "strcpy"
};


Token next();
Token peek();
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
void do_while_statement();
void if_statement();
void else_statement();
void switch_statement();
void case_statement();
void default_statement();
void outputStatement();
void inputStatement();
void dataTypeDeclaration();
void functionCall();
void loopStatement();
void conditionalStatement();

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

bool isComment(char *str){
    if(str[0] == '/' && str[1] == '/'){
        return true;
    }
    if(str[0] == '/' && str[1] == '*'){
        while(str[0] != '*' && str[1] != '/'){
            str++;
        }
        return true;
    }
    return false;
}
bool multiComment = false;
Token *tokenize(const char *code, int *tokenCount)
{
    Token *tokens = malloc(sizeof(Token) * 2048);
    *tokenCount = 0;
    int lineNo = 1;

    for (int i = 0; i < strlen(code); i++)
    {
        char c = code[i];
        if(c=='/'){
            if(code[i+1] == '/'){
                while(code[i] != '\n'){
                    i++;
                }
                continue;
            }
            if(code[i+1] == '*'){
                int j=i+2;
                while(code[j] != '*' && code[j+1] != '/'){
                    j++;
                }
                i = j+2;
                continue;
            }
        }
        
        if (c == '\n')
        {
            lineNo++;
            continue;
        }
        if (c == ' ')
            continue;

        if(c=='&'){
            char b[1000];
            int ptr = 0;
            if((code[i+1]>='a' && code[i+1]<='z') || (code[i+1]>='A' && code[i+1]<='Z')){
                int j= i+1;
                while(code[j] != ')'){
                    b[ptr++] = code[j];
                    j++;
                }
                i = j-1;
            }
            tokens[(*tokenCount)++] = (Token){ID, b, lineNo};
            continue;
            

        }

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
                printf("Comment: %s\n", code + i);
                while (code[i] != '\n' && code[i] != '\0')
                {
                    i++;
                }
                printf("Comment: %s\n", code + i);
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
            if (isAssignment(&c))
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
            else
            {
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
Token next(){
    return tokens[currentToken++];
}

Token peek(){
    return tokens[currentToken];
}

void parse(){
    currentToken = 0;
    program();
    if(tokens[currentToken].type == T_EOF){
        printf("Parsing successful\n");
    } else {
        printf("Parsing failed\n");
    }
}

void program(){
    while(tokens[currentToken].type != T_EOF){
        external_declaration();
    }
}

void external_declaration(){
    if(tokens[currentToken].type == DATA_TYPES){
        if(tokens[currentToken+2].type == LPAREN){
            function_definition();
        } else {
            declaration();
        }
    }else if(tokens[currentToken].type == PREPROCESSOR){
        match(PREPROCESSOR);
    }
}

void function_definition(){
    printf("Function definition\n");
    printf("<%s, %s, %d>\n", token_type_strings[tokens[currentToken].type], tokens[currentToken].value, tokens[currentToken].line_no);
    match(DATA_TYPES);
    match(ID);
    match(LPAREN);
    printf("Current: %s\n", tokens[currentToken].value);

    while(tokens[currentToken].type != RPAREN){
        parameter_list();
    }
    printf("Current: %s\n", tokens[currentToken].value);
    match(RPAREN);
    match(LBRACE);
    while(tokens[currentToken].type != RBRACE){
        statement();
    }
    match(RBRACE);

}

void expression_statement(){
    expression();
    match(SEMICOLON);
}

void return_statement(){
    match(KEYWORDS);
    expression();
    match(SEMICOLON);
}

void statement(){
    int type = tokens[currentToken].type;
    if(type == DATA_TYPES){
        dataTypeDeclaration();
    }else if(type == INPUTS){
        inputStatement();
    }else if(type == OUTPUTS){
        outputStatement();
    }else if(type == LOOP){
        loopStatement();
    }else if(type == CONDITIONAL){
        conditionalStatement();
    }else if(type == FUNCTION){
        functionCall();
    }else if(type == ID){
        if(tokens[currentToken+1].type == LPAREN){
            match(ID);
            match(LPAREN);
            while(tokens[currentToken].type != RPAREN){
                match(tokens[currentToken].type);
                if(tokens[currentToken].type == OP){
                    match(OP);
                }
                if(tokens[currentToken].type == COMMA){
                    match(COMMA);
                }
            }
            match(RPAREN);
            match(SEMICOLON);
        }else{
        expression_statement();
        }
    }else if(type == KEYWORDS){
        if(strcmp(tokens[currentToken].value, "return") == 0){
            return_statement();
        }else if(strcmp(tokens[currentToken].value, "case") == 0){
            case_statement();
        }else if(strcmp(tokens[currentToken].value, "default") == 0){
            default_statement();
        }
    }else if(type == COMMENT){
        currentToken++;
    }else{
        printf("Syntax error at line %d\n", tokens[currentToken].line_no);
        exit(1);
    }
}

void parameter_list(){
    parameter();
    while(tokens[currentToken].type == COMMA){
        match(COMMA);
        parameter();
    }
}

void parameter(){
    match(DATA_TYPES);
    match(ID);
}

void declaration(){
    int type = tokens[currentToken].type;
    if (tokens[currentToken].type == DATA_TYPES)
    {
        dataTypeDeclaration();
    }
    else if (tokens[currentToken].type == INPUTS)
    {
        inputStatement();
    }
    else if (tokens[currentToken].type == OUTPUTS)
    {
        outputStatement();
    }
    else if (tokens[currentToken].type == LOOP)
    {
        printf("Loop\n");
        loopStatement();
    }
    else if (tokens[currentToken].type == CONDITIONAL)
    {
        conditionalStatement();
    }
    else if (tokens[currentToken].type == FUNCTION)
    {
        functionCall();
    }else if(tokens[currentToken].type == COMMENT){
        currentToken++;
    }else if(tokens[currentToken].type == ID){
        printf("i am here");
        match(ID);
        if(tokens[currentToken].type == LPAREN){
            match(LPAREN);
            while(tokens[currentToken].type != RPAREN){
                match(tokens[currentToken].type);
                match(COMMA);
            }
            match(RPAREN);
            match(SEMICOLON);
        }
    }
}

void expression(){
    term();
    while(tokens[currentToken].type == OP || tokens[currentToken].type == ASSIGNMENT){
        if(tokens[currentToken].type == ASSIGNMENT){
            match(ASSIGNMENT);
        }else{
            match(OP);
        }
        term();
    }
}

void term(){
    factor();
    while(tokens[currentToken].type == OP || tokens[currentToken].type == ASSIGNMENT){
        if(tokens[currentToken].type == ASSIGNMENT){
            match(ASSIGNMENT);
        }else{
            match(OP);
        }
        factor();
    }
}

void factor(){
    if(tokens[currentToken].type == LPAREN){
        match(LPAREN);
        expression();
        match(RPAREN);
    }else if(tokens[currentToken].type == ID){
        match(ID);
    }else if(tokens[currentToken].type == NUM){
        match(NUM);
    }
}

void functionCall(){
    match(FUNCTION);
    match(LPAREN);
    while(tokens[currentToken].type != RPAREN){
        expression();
    }
    match(RPAREN);
    match(SEMICOLON);
}

void conditionalStatement(){
    if(strcmp(tokens[currentToken].value, "if") == 0){
        if_statement();
    }else if(strcmp(tokens[currentToken].value, "switch") == 0){
        switch_statement();
    }
}

void if_statement(){
    match(CONDITIONAL);
    match(LPAREN);
    expression();
    match(RPAREN);
    match(LBRACE);
    while(tokens[currentToken].type != RBRACE){
        statement();
    }
    match(RBRACE);
    else_statement();
}

void else_statement(){
    if(strcmp(tokens[currentToken].value, "else") == 0){
        match(CONDITIONAL);
        match(LBRACE);
        while(tokens[currentToken].type != RBRACE){
            statement();
        }
        match(RBRACE);
    }
}

void switch_statement(){
    match(CONDITIONAL);
    match(LPAREN);
    expression();
    match(RPAREN);
    match(LBRACE);
    while(tokens[currentToken].type != RBRACE){
        case_statement();
    }
    match(RBRACE);
}

void case_statement(){
    match(KEYWORDS);
    match(NUM);
    match(COLON);
    while(tokens[currentToken].type != RBRACE){
        if(strcmp(tokens[currentToken].value, "break")==0){
            match(KEYWORDS);
            match(SEMICOLON);
        }else{
            statement();
        }
    }
}

void default_statement(){
    if(strcmp(tokens[currentToken].value, "default") == 0){
        match(KEYWORDS);
        match(COLON);
        while(tokens[currentToken].type != RBRACE){
            if(strcmp(tokens[currentToken].value, "break")==0){
                match(KEYWORDS);
                match(SEMICOLON);
            }else{
                statement();
            }
        }
    }
}

void loopStatement()
{
    printf("Value: %s\n", tokens[currentToken].value);
    if (strcmp(tokens[currentToken].value, "for") == 0)
    {
        for_statement();
    }
    else if (strcmp(tokens[currentToken].value, "while") == 0)
    {
        while_statement();
    }
    else if (strcmp(tokens[currentToken].value, "do") == 0)
    {
        do_while_statement();
    }
}

void do_while_statement(){
    match(LOOP);
    match(LBRACE);
    while(tokens[currentToken].type != RBRACE){
        statement();
    }
    match(RBRACE);
    match(LOOP);
    match(LPAREN);
    expression();
    match(RPAREN);
    match(SEMICOLON);
}

void while_statement(){
    match(LOOP);
    match(LPAREN);
    expression();
    match(RPAREN);
    match(LBRACE);
    while(tokens[currentToken].type != RBRACE){
        statement();
    }
    match(RBRACE);
}

void for_statement(){
    match(LOOP);
    match(LPAREN);
    declaration();
    match(ID);
    match(OP);
    match(NUM);
    match(SEMICOLON);
    match(ID);
    match(OP);
    match(RPAREN);
    match(LBRACE);
    while(tokens[currentToken].type != RBRACE){
        statement();
    }
    match(RBRACE);
}

void outputStatement(){
    match(OUTPUTS);
    match(LPAREN);
    match(STRING);
    while(tokens[currentToken].type != RPAREN){
        match(COMMA);
        match(ID);
    }
    match(RPAREN);
    match(SEMICOLON);
}

void inputStatement(){
    match(INPUTS);
    match(LPAREN);
    match(STRING);
    match(COMMA);
    match(ID);
    while(tokens[currentToken].type != RPAREN){
        match(ID);
        if(tokens[currentToken].type == COMMA){
            match(COMMA);
        }
    }
    match(RPAREN);
    match(SEMICOLON);
}

void dataTypeDeclaration(){
    printf("\n From data type: \n");
    int type = tokens[currentToken].type;
    if(strcmp(tokens[currentToken].value, "char")==0){
        match(DATA_TYPES);
        match(ID);
        match(LBRACKET);
        while(tokens[currentToken].type != RBRACKET){
            match(NUM);
        }
        match(RBRACKET);
    }else{
        match(DATA_TYPES);
        match(ID);
    }
    type = tokens[currentToken].type;
    printf("Current: %s\n", tokens[currentToken].value);
    if (type == ASSIGNMENT)
    {
        match(ASSIGNMENT);
        expression();
    }
    while (tokens[currentToken].type == COMMA)
    {
        match(COMMA);
        match(ID);
        if (type == ASSIGNMENT)
        {
            match(ASSIGNMENT);
            expression();
        }
    }
    match(SEMICOLON);
}

void match(TokenType expected){
    printf("From Match: \n");
    printf("<%s, %s, %d>\n", token_type_strings[tokens[currentToken].type], tokens[currentToken].value, tokens[currentToken].line_no);
    printf("expected: %s\n", token_type_strings[expected]);
    if(tokens[currentToken].type == expected){
        currentToken++;
    } else {
        printf("Syntax error at lines %d\n", tokens[currentToken].line_no);
        exit(1);
    }
}

// Semantic Analysis
void insert_symbol(char *name, char *type)
{
    Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
    symbol->name = strdup(name);
    symbol->type = strdup(type);
    symbol->next = symbolTable;
    symbolTable = symbol;
}

Symbol *find_symbol(char *name)
{
    Symbol *current = symbolTable;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void printAllSymbols(){
    Symbol *current = symbolTable;
    while(current != NULL){
        printf("Name: %s, Type: %s\n", current->name, current->type);
        current = current->next;
    }
}

int semantic_analysis()
{
    int scope = 0;
    int semanticErrors = 0;

    for (int i = 0; i < tokenCount; i++)
    {
        Token token = tokens[i];
        
        if (token.type == DATA_TYPES)
        {
            // Next token should be an identifier
            if (tokens[i + 1].type != ID)
            {
                printf("Semantic Error: Expected identifier after data type at line %d\n", token.line_no);
                semanticErrors++;
            }
            else
            {
                while(tokens[i].type != SEMICOLON){
                    if(tokens[i].type == ID){
                        insert_symbol(tokens[i].value, token.value);
                    }
                    i++;
                }
            }
        }
        else if (token.type == ID)
        {
            // Check if the identifier is declared
            Symbol *symbol = find_symbol(token.value);
            if (symbol == NULL)
            {
                
                bool declared = false;
                for (int j = i - 1; j >= 0; j--)
                {
                    if (tokens[j].type == SEMICOLON)
                    {
                        break;
                    }
                    else if (tokens[j].type == DATA_TYPES)
                    {
                        declared = true;
                        break;
                    }
                }
                if (!declared)
                {
                    printf("Semantic Error: Undeclared variable %s at line %d\n", token.value, token.line_no);
                    semanticErrors++;
                }
            }
        }
        else if (token.type == LBRACE)
        {
            scope++;
        }
        else if (token.type == RBRACE)
        {
            scope--;
        }
    }

    return semanticErrors;
}


void convert_to_javascript(FILE *outputFile)
{
    for (int i = 0; i < tokenCount; i++)
    {
        Token token = tokens[i];

        if (token.type == DATA_TYPES)
        {
            if (tokens[i + 1].type == ID && tokens[i + 2].type == LPAREN)
            {
                fprintf(outputFile, "function ");
                int j = i + 1;
                while (tokens[j].type != RPAREN)
                {
                    if (tokens[j].type == DATA_TYPES)
                    {
                        j++;
                        continue;
                    }
                    fprintf(outputFile, "%s", tokens[j].value);
                    j++;
                }
                i = j - 1;
            }
            else
            {
                fprintf(outputFile, "let ");
            }
        }
        else if(token.type==LOOP){
            if(strcmp(token.value, "for") == 0){
                fprintf(outputFile, "for");
                i++;
                while(tokens[i].type != LBRACE){
                    if(tokens[i].type == SEMICOLON){
                        fprintf(outputFile, "; ");
                    }else if(tokens[i].type == DATA_TYPES){
                        fprintf(outputFile, "let ");
                    }else{
                        fprintf(outputFile, "%s", tokens[i].value);
                    }
                    i++;
                }
                i--;
            }else{
                fprintf(outputFile, "%s", token.value);
            }
        }
        else if(strcmp(tokens[i].value, "break")==0){
            fprintf(outputFile, "\t\tbreak");
        }
        else if (token.type == ID)
        {
            fprintf(outputFile, "%s", token.value);
            if (tokens[i + 1].type == LBRACKET)
            {
                int j = i + 1;
                while (tokens[j].type != RBRACKET)
                {
                    j++;
                }
                i = j;
            }
        }
        else if (token.type == KEYWORDS)
        {
            if (strcmp(token.value, "case") == 0)
            {
                fprintf(outputFile, "case ");
            }
            else
            {
                fprintf(outputFile, "%s", token.value);
            }
        }
        else if (token.type == OP || token.type == NUM)
        {
            fprintf(outputFile, "%s", token.value);
        }
        else if (token.type == STRING || token.type == CHAR)
        {
            fprintf(outputFile, "\"%s\"", token.value);
        }
        else if (token.type == COMMENT)
        {
            fprintf(outputFile, "// %s\n", token.value);
        }
        else if (token.type == COLON)
        {
            fprintf(outputFile, ": ");
        }
        else if(strcmp(tokens[i].value, "default")==0){
            fprintf(outputFile, "default");
        }
        else if (token.type == PUNCTUATORS || token.type == LPAREN || token.type == RPAREN ||
                 token.type == LBRACE || token.type == RBRACE || token.type == LBRACKET ||
                 token.type == RBRACKET || token.type == SEMICOLON || token.type == COMMA || token.type == DOT)
        {
            fprintf(outputFile, "%s", token.value);
            if (token.type == SEMICOLON || token.type == LBRACE || token.type == RBRACE)
            {
                fprintf(outputFile, "\n");
            }
        }
        else if (token.type == PREPROCESSOR)
        {
            // Skip preprocessor directives
            continue;
        }
        else if (token.type == INPUTS)
        {
            fprintf(outputFile, "prompt(");
        }
        else if (token.type == OUTPUTS)
        {
            fprintf(outputFile, "console.log(");
            for (int j = i; j < tokenCount; j++)
            {
                if (tokens[j].type == STRING && tokens[j + 1].type == COMMA)
                {
                    i = j;
                    break;
                }
            }
            i++;
        }
        else if (token.type == ASSIGNMENT)
        {
            fprintf(outputFile, " = ");
        }
        else if (token.type == T_EOF)
        {
            // Handle end of file if necessary
        }
        else if (token.type == LOOP || token.type == CONDITIONAL || token.type == FUNCTION)
        {
            fprintf(outputFile, "%s", token.value);
        }
        else if (token.type == UNKNOWN)
        {
            printf("/* Unhandled token type: %s */", token.value);
        }
        else
        {
            printf("/* Unhandled token type: %s */", token.value);
        }
    }
    printf("\n");
}

// Call the main function at the end of the JavaScript code
void generate_main_function_call(FILE *outputFile){
    fprintf(outputFile, "main();");
}

void convert_to_javascript_with_main_call(FILE *outputFile)
{
    convert_to_javascript(outputFile);
    generate_main_function_call(outputFile);
}

int main()
{
    char *source_code = read_file("input.c");
    tokens = tokenize(source_code, &tokenCount);
    print_tokens(tokens, tokenCount);

    parse();
    int semanticErrors = semantic_analysis();
    // printAllSymbols();
    if (semanticErrors == 0)
    {
        printf("Semantic Analysis: No errors found. Success!\n");
    }
    else
    {
        printf("Semantic Analysis: %d errors found.\n", semanticErrors);
        return 0;
    }

    FILE *outputFile = fopen("output.js", "w");

    convert_to_javascript_with_main_call(outputFile);

    // Clean up
    free(source_code);
    return 0;
}

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
                fprintf(outputFile, "%s ", token.value);
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
            fprintf(outputFile, "%s ", token.value);
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
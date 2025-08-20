#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_INPUTDEFS (10)
#define MAX_OUTPUTDEFS (10)
#define MAX_CHIPDEFS (10)

#define TODO(WHAT)                                                                 \
    do                                                                             \
    {                                                                              \
        printf("TODO: %s %s() %s:%d\n", (WHAT), __FUNCTION__, __FILE__, __LINE__); \
        abort();                                                                   \
    } while (0);

char *ReadEntireFile(const char *fpath, long *len)
{
    char *buffer = 0;
    long length;
    FILE *f = fopen(fpath, "rb");

    if (f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length + 1);
        if (buffer)
        {
            fread(buffer, 1, length, f);
        }
        fclose(f);

        buffer[length] = 0;
        if (len)
            *len = length;
    }

    return buffer;
}

typedef struct Slice
{
    char *base;
    int len;
} Slice;

typedef struct ChipIODef
{
    Slice name;
    // todo: input_array_size?
} ChipIODef;

// chip name_of_chip (inputs) -> (outputs)
typedef struct ChipDef
{
    Slice name;
    ChipIODef inputs[MAX_INPUTDEFS];
    ChipIODef outputs[MAX_OUTPUTDEFS];
} ChipDef;

typedef enum TGTokKind
{
    // name of something
    TOK_ident,

    TOK_number,

    TOK_open_paren,
    TOK_close_paren,
    TOK_open_bracket,
    TOK_close_bracket,
    TOK_open_square,
    TOK_close_square,
    // ,
    TOK_cama,
    // ;
    TOK_semi,
    // =
    TOK_equ,
    // "\n"
    TOK_end_of_line,
    // "//"
    TOK_single_line_comment,
    // "/*"
    TOK_multi_line_comment_start,
    // "*/"
    TOK_multi_line_comment_end,

    // the string "->"
    TOK_arrow,

    TOK_end_of_input

} TGTokKind;

typedef struct TGToken
{
    TGTokKind kind;
    union
    {
        Slice ident;
        unsigned int number;
    };
} TGToken;

typedef struct TGLexer
{
    char *base, *now, *eof;
    long line_number;
    bool in_comment;

} TGLexer;

// true if there are atleast count chars left in Lexer
bool AtleastChars(TGLexer l, size_t count) { return l.eof - l.now > count; }
bool IsNumber(char c) { return c >= '0' && c < '9'; }

#include <math.h>

unsigned int power(int digits)
{
    switch (digits)
    {
        // clang-format off
    case  0: return 0; 
    case  1: return 1; 
    case  2: return 10; 
    case  3: return 100; 
    case  4: return 1000; 
    case  5: return 10000; 
    case  6: return 1000000; 
    case  7: return 10000000; 
    case  8: return 100000000; 
    case  9: return 1000000000;
        // clang-format on
    }

    TODO("THAT NUMBER HAS TOO MANY DIGITS APOLOGIZE !!");
}

unsigned int ParseNumber(char *start, size_t length)
{
    unsigned int res = 0;

    for (size_t i = 0; i < length; i++)
    {
        char c = start[i];
        int v = c - '0';
        unsigned int pow = power(length - i);
        res += v * pow;
    }

    return res;
}

TGToken NextToken(TGLexer *lex)
{

GIVE_READING_THE_TOKEN_ANOTHER_GO:;

    if (lex->now >= lex->eof)
        return (TGToken){.kind = TOK_end_of_input};

    TGToken ret = {0};

    int read = 1;

#define CHNOW (lex->now[0])
#define CHNXT (lex->now[1])

    if (CHNOW == ' ' || CHNOW == '\t')
    {
        lex->now += 1;
        goto GIVE_READING_THE_TOKEN_ANOTHER_GO;
    }
    // clang-format off
    else if (CHNOW == '(')  ret.kind = TOK_open_paren;
    else if (CHNOW == ')')  ret.kind = TOK_close_paren;
    else if (CHNOW == '[')  ret.kind = TOK_open_square;
    else if (CHNOW == ']')  ret.kind = TOK_close_square;
    else if (CHNOW == '{')  ret.kind = TOK_open_bracket;
    else if (CHNOW == '}')  ret.kind = TOK_close_bracket;
    else if (CHNOW == ',')  ret.kind = TOK_cama;
    else if (CHNOW == '=')  ret.kind = TOK_equ;
    else if (CHNOW == ';')  ret.kind = TOK_semi;
    else if (CHNOW == '\n') ret.kind = TOK_end_of_line;
    // clang-format on
    else if (IsNumber(CHNOW))
    {
        char *now = lex->now;

        while (now != lex->eof)
        {
            if (!IsNumber(now[0]))
                goto FOUND_END_OF_NUMBER;

            now += 1;
        }

        TODO("ERROR: Unexpected end of file!");

    FOUND_END_OF_NUMBER:;

        read = now - lex->now;

        unsigned int parsed_number = ParseNumber(lex->now, read);

        ret.kind = TOK_number;
        ret.number = parsed_number;
    }
    else if (AtleastChars(*lex, 2) && CHNOW == '/' && CHNXT == '/')
    {
        ret.kind = TOK_single_line_comment;
        read = 2;
    }
    else if (AtleastChars(*lex, 2) && CHNOW == '/' && CHNXT == '*')
    {
        ret.kind = TOK_multi_line_comment_start;
        read = 2;
    }
    else if (AtleastChars(*lex, 2) && CHNOW == '*' && CHNXT == '/')
    {
        ret.kind = TOK_multi_line_comment_end;
        read = 2;
    }
    else if (AtleastChars(*lex, 2) && CHNOW == '-' && CHNXT == '>')
    {
        ret.kind = TOK_arrow;
        read = 2;
    }
    else
    {
        ret.kind = TOK_ident;
        ret.ident.base = lex->now;

        const char *ident_enders = " \t(){}[]->;,\n*/";
        size_t ident_len = strlen(ident_enders);

        char *now = lex->now;
        while (now != lex->eof)
        {
            for (size_t i = 0; i < ident_len; i++)
            {
                char ender = ident_enders[i];
                if (now[0] == ender)
                {
                    goto END_OF_TOKEN_FOUND;
                }
            }

            now += 1;
        }

        TODO("ERROR: Unexpected end of file");

    END_OF_TOKEN_FOUND:;
        ret.ident.len = read = now - lex->now;
    }

    lex->now += read;

    return ret;
    
#undef CHNOW
#undef CHNXT
}

void PrintToken(TGToken tok)
{
    // clang-format off
    switch(tok.kind) {
    case TOK_ident:  printf("ident '%.*s'\n", tok.ident.len, tok.ident.base); break;
    case TOK_open_paren: puts("open paren '('"); break;
    case TOK_close_paren: puts("close paren ')'"); break;
    case TOK_open_bracket: puts("open bracket '{'"); break;
    case TOK_close_bracket: puts("close bracket '}'"); break;
    case TOK_open_square: puts("open square '['"); break;
    case TOK_close_square: puts("close square ']'"); break;
    case TOK_cama: puts("cama ','"); break;
    case TOK_equ: puts("equ '='"); break;
    case TOK_semi: puts("semi ';'"); break;
    case TOK_end_of_line: puts("end of line '\\n'"); break;
    case TOK_single_line_comment: puts("start of 1 line comment '//'"); break;
    case TOK_multi_line_comment_start: puts("start of multiline comment '/*'"); break;
    case TOK_multi_line_comment_end: puts("end of multiline comment '*/'"); break;
    case TOK_arrow: puts("arrow '->'"); break;
    case TOK_number: printf("number '%u'\n", tok.number); break;
    case TOK_end_of_input: puts("[end of input]"); break;
    }
    // clang-format on
}

// static ChipDef g_chips[MAX_CHIPDEFS] = {0};

int main(int argc, char *argv[])
{
    long flen;
    char *file = ReadEntireFile("one.tg", &flen);

    if (file == NULL)
    {
        perror("fopen");
        return 1;
    }

    TGLexer p = {
        .base = file,
        .now = file,
        .eof = file + flen,
    };

    TGToken tk;
    while ((tk = NextToken(&p)).kind != TOK_end_of_input)
    {
        PrintToken(tk);
    }

    PrintToken(tk);

    return 0;
}

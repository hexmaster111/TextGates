#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_CHIP_IO_DEFS (10)
#define MAX_LVALUE_ASSIGNMENTS (10)

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

typedef struct ChipIODefList
{
    ChipIODef items[MAX_CHIP_IO_DEFS];
} ChipIODefList;

/*
Q =
    AND (   -- EXP_use_chip
        A,     -- EXP_use_wire
        B      -- EXP_use_wire
    )
    ;
*/

/*
C_out =  OR(AND(A,B), AND(C_in, XOR(A,B)))

C_out =
    OR --use chip
    (
        AND --use chip
        (
            A, -- use wire
            B  -- use wire
        )
        ,
        AND -- use chip
        (
            C_in -- use wire
            XOR  -- use chip
            (
                A -- use wire
                B -- use wire
            )
        )
    )

*/

/*
    O = BUFF -- use chip
        (
            I -- use wire
        )
*/

struct Expr
{
    enum
    {
        EXP_use_chip = 1,
        EXP_use_wire
    } kind;

    Slice name;

    size_t sub_expressions_count;
    struct Expr *sub_expressions;
};

typedef struct Expr Expr;

typedef struct ChipStructure
{
    Slice assign_to;
    Expr expression;
} ChipStructure;

// chip name_of_chip (inputs) -> (outputs)
typedef struct ChipDef
{
    Slice name;
    ChipIODefList inputs;
    ChipIODefList outputs;

    ChipStructure structure;
} ChipDef;

typedef enum TGTokKind
{
    // name of something
    TOK_ident = 1,

    TOK_number,
    // "chip"
    TOK_chip,

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
    bool in_single_line_comment;
    bool in_multi_line_comment;
} TGLexer;

// true if there are atleast count chars left in Lexer
bool AtleastChars(TGLexer l, size_t count) { return l.eof - l.now > count; }
bool IsNumber(char c) { return c >= '0' && c < '9'; }

unsigned int Power(int digits)
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
        unsigned int pow = Power(length - i);
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
    else if (CHNOW == '\n') {
        ret.kind = TOK_end_of_line;
        lex->line_number += 1;
    }
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
    else if (AtleastChars(*lex, 4) && (memcmp(lex->now, "chip", 4) == 0))
    {
        ret.kind = TOK_chip;
        read = 4;
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

        const char *ident_enders = " \t(){}[]->=;,\n*/";
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

#undef CHNOW
#undef CHNXT

    // comment skipping
    if (lex->in_single_line_comment && ret.kind == TOK_end_of_line)
    {
        lex->in_single_line_comment = false;
    }
    else if (lex->in_multi_line_comment && ret.kind == TOK_multi_line_comment_end)
    {
        lex->in_multi_line_comment = false;
    }
    else if (ret.kind == TOK_single_line_comment)
    {
        lex->in_single_line_comment = true;
    }
    else if (ret.kind == TOK_multi_line_comment_start)
    {
        lex->in_multi_line_comment = true;
    }

    if (lex->in_single_line_comment ||
        lex->in_multi_line_comment ||
        ret.kind == TOK_multi_line_comment_end ||
        ret.kind == TOK_end_of_line)
    {
        goto GIVE_READING_THE_TOKEN_ANOTHER_GO;
    }

    return ret;
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
    case TOK_chip: puts("chip def 'chip'"); break;
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

void Usage(char *name)
{
    printf("usage: %s <filename.tg>\n", name);
}

TGToken ExpectTokenOrFail(TGLexer *l, TGTokKind tok)
{
    TGToken t = NextToken(l);
    if (t.kind != tok)
    {
        TODO("ERROR: Unexpected token!");
    }

    return t;
}

// ... ( <ident / ident cama ident / none> ) ...
ChipIODefList ParseIoDefList(TGLexer *l)
{
    ExpectTokenOrFail(l, TOK_open_paren);

    ChipIODefList ret = {0};
    int listidx = 0;
    TGToken tk;
    bool expect_cama = false;

    while ((tk = NextToken(l)).kind != TOK_close_paren)
    {

        if (!expect_cama)
        {
            if (tk.kind != TOK_ident)
                TODO("ERROR: Expected Identifer");
            ret.items[listidx].name = tk.ident;
            listidx += 1;
            expect_cama = true;
            // PrintToken(tk);
        }
        else
        {
            if (tk.kind != TOK_cama)
                TODO("ERROR: Expect Cama");

            expect_cama = false;
        }
    }

    return ret;
}

// everything after the = Something(something, something)
Expr ParseRhsExpression(TGLexer *l)
{
    Expr ret = {0};
    ret.name = ExpectTokenOrFail(l, TOK_ident).ident;
    ExpectTokenOrFail(l, TOK_open_paren);
    ret.kind = EXP_use_chip;

    ret.sub_expressions_count = 1;
    ret.sub_expressions = malloc(sizeof(Expr) * ret.sub_expressions_count);
    memset(ret.sub_expressions, 0, sizeof(Expr) * ret.sub_expressions_count);

    ret.sub_expressions[0].kind = EXP_use_wire;
    ret.sub_expressions[0].name = ExpectTokenOrFail(l, TOK_ident).ident;

    ExpectTokenOrFail(l, TOK_close_paren);
    ExpectTokenOrFail(l, TOK_semi);

    TODO("im working here yet")
    return ret;
}

ChipStructure ParseChipBody(TGLexer *l)
{
    ExpectTokenOrFail(l, TOK_open_bracket);

    ChipStructure ret = {0};

    ret.assign_to = ExpectTokenOrFail(l, TOK_ident).ident;
    ExpectTokenOrFail(l, TOK_equ);

    ret.expression = ParseRhsExpression(l);

    ExpectTokenOrFail(l, TOK_close_bracket);

    return ret;
}

// ... IDENT, OPEN_PAREN, <ident / ident cama ident>, CLOSE_PAREN, ARROW, OPEN_PAREN, <ident / ident cama ident>, CLOSE PAREN ...
ChipDef ParseChip(TGLexer *l)
{
    ChipDef ret = {0};

    ret.name = ExpectTokenOrFail(l, TOK_ident).ident;
    ret.inputs = ParseIoDefList(l);
    ExpectTokenOrFail(l, TOK_arrow);
    ret.outputs = ParseIoDefList(l);
    ret.structure = ParseChipBody(l);
    return ret;
}

int main(int argc, char *argv[])
{

    if (2 > argc)
    {
        Usage(argv[0]);
        return 1;
    }

    long flen;
    char *file = ReadEntireFile(argv[1], &flen);

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

        if (tk.kind == TOK_chip)
        {
            ChipDef c = ParseChip(&p);
            printf("chip: %.*s\n", c.name.len, c.name.base);
        }
        else if (tk.kind == TOK_end_of_line)
        {
        }
        else
        {
            printf("unhandled token: ");
            PrintToken(tk);
            TODO("Unexpected token!");
        }
    }

    PrintToken(tk);

    return 0;
}

#include <stdio.h>
#include <ctype.h>
#include "str.h"
#include "error.h"
#include "scanner.h"
#include "token.h"

enum e_scanner_state {SS_DEFAULT,
                     SS_COMMENT,
                     SS_SLASH,
                     SS_BLOCK_COMMENT,
                     SS_BLOCK_COMMENT_ASTERISK,
                     SS_IDENTIFICATOR,
                     SS_INT_PART_1,
                     SS_INT_PART_2,
                     SS_FRACTIONAL_PART,
                     SS_EXPONENCIAL_PART,
                     SS_STRING,
                     SS_STRING_BACKSLASH,
                     SS_STRING_ESCAPE_HEX_1,
                     SS_STRING_ESCAPE_HEX_2,
                     SS_LESS_THEN,
                     SS_GREATER_THEN,
                     SS_OPERATOR_OF_ASSIGNEMENT,
                     SS_EXCLAMATION,
                    };



enum e_token_t scanner_control_type(str_t *s);
enum e_token_t scanner_check_keyword(str_t *s);

scanner_t * scanner_init(FILE* fp)
{
    scanner_t * s = ifj15_malloc(SCANNER, sizeof(scanner_t));

    s->file = fp;
    s->str = str_init();
    s->line = 1;

    return s;
}

void _scanner_free(scanner_t* s) {
    free(s);
}

#define next_state(s)   ( prev_state = state, state = s )

#define is_whitespace(c)    ( c == '\n' || c == ' ' || c == '\t' )
#define is_digit(c)         ( c >= '0' && c < '9' )

#define is_identificator_start(c)   ( c == '_' || ( c >= 'A' && c <= 'Z') || ( c >= 'a' && c < 'z') )
#define is_identificator(c)         ( is_identificator_start(c) || is_digit(c) )

#define is_hex(c)   ( (c >= '0' && c <= '9') || (c >= 'A' && c >= 'F') || (c >= 'a' && c >= 'f')  )

#define hex_decode(c) ( ( c >= '0' && c <= '9') ? (c-'0') : ( \
                        ( c >= 'A' && c >= 'F') ? (c-'A' + 10 ) : ( \
                                                  (c-'a' + 10 ) ) ) )


token_t get_next_token(scanner_t * s)
{
    token_t tok;
    int c = 0;

    char hex_char;

    enum e_scanner_state state = SS_DEFAULT;

    str_clear(s->str);

    while(1)
    {
        c = getc(s->file);
        //printf("%c", c);

        if (c == EOF)
        {
            tok.type = TT_EOF;
            return tok;
        }

        else if (c == '\n')
            s->line++;


        switch(state)
        {
        case SS_DEFAULT:

            //default state
            //Implement SWITCH

            //eats whitespace
            if ( is_whitespace(c) )
                state = SS_DEFAULT;

            else if(c== '/')
                state = SS_SLASH;

            //start of identificator
            else if( is_identificator_start(c) )
            {
                ungetc(c, s->file);
                state = SS_IDENTIFICATOR;
            }

            //start of non negative number
            else if ( is_digit(c) )
            {
                ungetc(c, s->file);
                state = SS_INT_PART_1;
            }

            else if(c == '"')
            {
                state = SS_STRING;
            }

            else if (c == '=')
            {
                state = SS_OPERATOR_OF_ASSIGNEMENT;
            }

            else if(c == '*')
            {
                tok.type = TT_OP_ARITHMETIC;
                tok.op_arith = OP_ARITH_MULTIPLY;
                return tok;
            }

            else if (c == '{')
            {
                tok.type = TT_BLOCK_START;
                return tok;

            }

            else if (c == '}')
            {
                tok.type = TT_BLOCK_END;
                return tok;
            }

            else if (c == ';')
            {
                tok.type = TT_SEMICOLON;
                return tok;
            }

            else if (c == ',')
            {
                tok.type = TT_COMMA;
                return tok;
            }

            else if (c == '-')
            {
                tok.type = TT_OP_ARITHMETIC;
                tok.op_arith = OP_ARITH_SUBTRACT;
                return tok;
            }

            else if (c == '+')
            {
                tok.type = TT_OP_ARITHMETIC;
                tok.op_arith = OP_ARITH_ADD;
                return tok;
            }

            else if(c == '(')
            {
                tok.type = TT_PARENTHESES_OPEN;
                return tok;
            }

            else if(c == ')')
            {
                tok.type = TT_PARENTHESES_CLOSE;
                return tok;
            }

            else if(c == '<')
                state = SS_LESS_THEN;

            else if(c == '>')
                state = SS_GREATER_THEN;

            else if(c == '!')
                state = SS_EXCLAMATION;

            else
                error("KOKOTINA, ktoru jazyk nezvlada.", ERROR_LEX);

            break;


        case SS_SLASH:

            if ( c == '/')
                state = SS_COMMENT;

            else if ( c == '*')
                state = SS_BLOCK_COMMENT;

            else if( c == '_' || ( c >= 'A' && c <= 'Z') || ( c > 'a' && c < 'z')
                     || is_whitespace(c)
                     || ( c >= '0' && c < '9' ) )
            {
                ungetc(c, s->file);

                tok.type = TT_OP_ARITHMETIC;
                tok.op_arith = OP_ARITH_DIVIDE;
                return tok;

            }

            else
            {
                ungetc(c, s->file);
                state = SS_DEFAULT;
            }

            break;


        case SS_COMMENT:

            if (c == '\n')
                state = SS_DEFAULT;

            else
                state = SS_COMMENT;

            break;

        case SS_BLOCK_COMMENT:

            if (c == '*')
                state = SS_BLOCK_COMMENT_ASTERISK;

            else
                state = SS_BLOCK_COMMENT;

            break;

        case SS_BLOCK_COMMENT_ASTERISK:

            if (c == '/')
                state = SS_DEFAULT;

            else
                state = SS_BLOCK_COMMENT;

            break;

        case SS_IDENTIFICATOR:

            if ( is_identificator(c) )
            {
                str_append_char(s->str, c);

                state = SS_IDENTIFICATOR;
            }

            else
            {
                ungetc(c, s->file);

                tok.type = scanner_check_keyword(s->str);

                if (tok.type == TT_NONE)
                {
                    tok.type = TT_IDENTIFICATOR;
                    tok.str = s->str;

                    s->str = str_init(); //initialize new string
                }

                return tok;
            }

            break;

        case SS_INT_PART_1:

            if( str_last_char(s->str) == '0' && c == '.')
                state = SS_FRACTIONAL_PART;

            else if( str_last_char(s->str) == '0' && is_digit(c))
                error("Incorrect representation of number", ERROR_LEX);

            else if (c >= '1' && c <= '9')
            {
                state = SS_INT_PART_2;
            }

            else if (c == '0')
            {
                state = SS_INT_PART_1;
            }

            else if (c == '.')
            {

                state = SS_FRACTIONAL_PART;
            }

            else
            {
                ungetc(c, s->file);

                tok.type = TT_LIT_INT;
                tok.int_val = str_to_int(s->str);
                return tok;
            }

            str_append_char(s->str, c);
            break;

        case SS_INT_PART_2:

            if (c >= '0' && c <= '9')
            {
                str_append_char(s->str, c);
                state = SS_INT_PART_2;
            }

            else if (c == '.')
            {
                str_append_char(s->str, c);
                state = SS_FRACTIONAL_PART;
            }

            else if (c == 'E' || c == 'e')
            {
                str_append_char(s->str, c);
                state = SS_EXPONENCIAL_PART;
            }

            else
            {
                ungetc(c, s->file);

                tok.type = TT_LIT_INT;
                tok.int_val = str_to_int(s->str);
                return tok;
            }

            break;

        case SS_FRACTIONAL_PART:

            if( c >= '0' && c <= '9' )
            {
                str_append_char(s->str, c);

                state = SS_FRACTIONAL_PART;
            }

            else if (c == 'E' || c == 'e')
            {
                str_append_char(s->str, c);
                state = SS_EXPONENCIAL_PART;
            }

            else if (c == '.')
                error("Incorrect representation of number", ERROR_LEX);

            else
            {
                ungetc(c, s->file);

                tok.type = TT_LIT_DOUBLE;
                tok.double_val = str_to_double(s->str);
                return tok;
            }

            break;

        case SS_EXPONENCIAL_PART:

            if (str_last_char(s->str) == 'E' || str_last_char(s->str) == 'e' )
                //first time in this state
            {
                if(c == '+' || c == '-')
                {
                    str_append_char(s->str, c);
                    state = SS_EXPONENCIAL_PART;
                }

                else if(c >= '0' && c <= '9')
                    str_append_char(s->str, c);

                else
                    error("Incorect representation of number", ERROR_LEX);
            }

            else if(c >= '0' && c <= '9')
            {
                str_append_char(s->str, c);
                state = SS_EXPONENCIAL_PART;
            }

            else if(c == '.')
                error("Incorrect representation of number", ERROR_LEX);

            else
            {
                ungetc(c, s->file);

                tok.type = TT_LIT_DOUBLE;
                tok.double_val = str_to_double(s->str);
                return tok;
            }

            break;

        case SS_STRING:

            if(c == '\\' )
                state = SS_STRING_BACKSLASH;

            else if(c == '"')
            {
                tok.type = TT_LIT_STRING;
                tok.str = s->str;
                s->str = str_init(); //initialize new string
                return tok;

            }

            else if (c > 31)
                str_append_char(s->str, c);

            break;

        case SS_STRING_BACKSLASH:

            if (c == '"' )
                str_append_char(s->str, '\"');

            else if(c == 'n')
                str_append_char(s->str, '\n');

            else if(c == 't')
                str_append_char(s->str, '\t');

            else if( c == '\\')
                str_append_char(s->str, '\\');

            else if(c == 'x')
            {
                state = SS_STRING_ESCAPE_HEX_1;
                break;
            }

            state = SS_STRING;
            break;

        case SS_STRING_ESCAPE_HEX_1:

            if( is_hex(c) )
            {
                hex_char = hex_decode(c) << 4;
                state = SS_STRING_ESCAPE_HEX_2;
            }

            else
                error("Invalid representation of char in escape sequence.", ERROR_LEX);

            break;

        case SS_STRING_ESCAPE_HEX_2:

            if( is_hex(c) )
            {
                hex_char |= hex_decode(c);

                str_append_char(s->str, hex_char);

                state = SS_STRING;
            }
            else
                error("Invalid representation of char in escape sequence.", ERROR_LEX);

            break;

        case SS_LESS_THEN:

            if(c == '<')
            {
                tok.type = TT_OP_STREAM_OUT;
                return tok;
            }

            else if( c == '=' )
            {
                tok.type = TT_OP_RELATIONAL;
                tok.op_rel = OP_REL_LESS_OR_EQ;
                return tok;
            }

            else
            {
                ungetc(c, s->file);

                tok.type = TT_OP_RELATIONAL;
                tok.op_rel = OP_REL_LESS;
                return tok;
            }

            break;

        case SS_GREATER_THEN:

            if(c == '>')
            {
                tok.type = TT_OP_STREAM_IN;
                return tok;

            }

            else if(c == '=')
            {
                tok.type = TT_OP_RELATIONAL;
                tok.op_rel = OP_REL_GREATER_OR_EQ;
                return tok;
            }

            else
            {
                ungetc(c, s->file);

                tok.type = TT_OP_RELATIONAL;
                tok.op_rel = OP_REL_GREATER;
                return tok;
            }

            break;

        case SS_EXCLAMATION:

            if(c == '=')
            {
                tok.type = TT_OP_RELATIONAL;
                tok.op_arith = OP_REL_NOT_EQUAL;
                return tok;
            }

            else
                error("Uncomplete non-equal operator.", ERROR_LEX);

            break;

        case SS_OPERATOR_OF_ASSIGNEMENT:

            if(c == '=')
            {
                tok.type = TT_OP_RELATIONAL;
                tok.op_rel = OP_REL_EQUAL;
                return tok;
            }

            else
            {
                ungetc(c, s->file);
                tok.type = TT_OP_ASSIGNMENT;
                return tok;
            }

            break;
        }
    }
}


/************************************
 * check for keywords of language
 ************************************/
enum e_token_t scanner_check_keyword(str_t *s)
{

    if (str_equals(s, "double"))
        return TT_TYPE_DOUBLE;

    else if(str_equals(s, "string"))
        return TT_TYPE_STRING;

    else if(str_equals(s, "auto"))
        return TT_TYPE_AUTO;

    else if(str_equals(s, "int"))
        return TT_TYPE_INT;

    else if (str_equals(s, "cin"))
        return TT_KW_CIN;

    else if (str_equals(s, "cout"))
        return TT_KW_COUT;

    else if (str_equals(s, "for"))
        return TT_KW_FOR;

    else if (str_equals(s, "if"))
        return TT_KW_IF;

    else if (str_equals(s, "return"))
        return TT_KW_RETURN;

    else if (str_equals(s, "else"))
        return TT_KW_ELSE;

    else
        return TT_NONE;
}

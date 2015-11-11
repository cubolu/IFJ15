#include <stdio.h>
#include <ctype.h>
#include "str.h"
#include "error.h"
#include "parser.h"
#include "token.h"

enum e_parser_state {PS_DEFAULT,
                     PS_COMMENT,
                     PS_SLASH,
                     PS_BLOCK_COMMENT,
                     PS_IDENTIFICATOR,
                     PS_INT_PART_1,
                     PS_INT_PART_2,
                     PS_FRACTIONAL_PART,
                     PS_EXPONENCIAL_PART,
                     PS_STRING,
                     PS_STRING_BACKSLASH,
                     PS_STRING_ESCAPE_HEX_1,
                     PS_STRING_ESCAPE_HEX_2,
                     PS_LESS_THEN,
                     PS_GREATER_THEN,
                     PS_OPERATOR_OF_ASSIGNEMENT
                    };



enum e_token_type parser_control_type(str *s);
enum e_token_type parser_check_keyword(str *s);

parser * parser_init(char * filename)
{
    // TODO alokovat systematicky
    parser * p = malloc(sizeof(parser));

    p->file = fopen(filename, "r");
    //TODO overit otevreni

    p->s = str_init();

    return p;
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


token_t parser_next_token(parser * p)
{
    token_t tok;
    int c = 0, c_prev = 0;

    char hex_char;

    enum e_parser_state state = PS_DEFAULT;

    str_clear(p->s);

    while(1)
    {
        c_prev = c;
        c = getc(p->file);
        printf("%c", c);

        if (c == EOF)
        {
            tok.type = TT_EOF;
            return tok;
        }

        switch(state)
        {
        case PS_DEFAULT:

            //default state
            //Implement SWITCH

            //eats whitespace
            if ( is_whitespace(c) )
                state = PS_DEFAULT;

            else if(c== '/')
                state = PS_SLASH;

            //start of identificator
            else if( is_identificator_start(c) )
            {
                ungetc(c, p->file);
                state = PS_IDENTIFICATOR;
            }

            //start of non negative number
            else if ( is_digit(c) )
            {
                ungetc(c, p->file);
                state = PS_INT_PART_1;
            }

            else if(c == '"')
            {
                state = PS_STRING;
            }

            else if (c == '=')
            {
                state = PS_OPERATOR_OF_ASSIGNEMENT;
            }

            else if(c == '*')
            {
                tok.type = TT_OP_ARITHEMTIC;
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
                tok.type = TT_OP_ARITHEMTIC;
                tok.op_arith = OP_ARITH_SUBTRACT;
                return tok;
            }

            else if (c == '+')
            {
                tok.type = TT_OP_ARITHEMTIC;
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
                state = PS_LESS_THEN;

            else if(c == '>')
                state = PS_GREATER_THEN;

            else
                error("KOKOTINA, ktoru jazyk nezvlada.", ERROR_LEX);

            break;


        case PS_SLASH:

            if ( c == '/')
                state = PS_COMMENT;

            else if ( c == '*')
                state = PS_BLOCK_COMMENT;

            else if( c == '_' || ( c >= 'A' && c <= 'Z') || ( c > 'a' && c < 'z')
                     || is_whitespace(c)
                     || ( c >= '0' && c < '9' ) )
            {
                ungetc(c, p->file);

                tok.type = TT_OP_ARITHEMTIC;
                tok.op_arith = OP_ARITH_DIVIDE;
                return tok;

            }

            else
            {
                ungetc(c, p->file);
                state = PS_DEFAULT;
            }

            break;


        case PS_COMMENT:

            if (c == '\n')
                state = PS_DEFAULT;

            else
                state = PS_COMMENT;

            break;

        case PS_BLOCK_COMMENT:

            if (c == '/')
            {
                if (c_prev == '*')
                    state = PS_DEFAULT;
            }

            else
                state = PS_BLOCK_COMMENT;

            break;

        case PS_IDENTIFICATOR:

            if ( is_identificator(c) )
            {
                str_append_char(p->s, c);

                state = PS_IDENTIFICATOR;
            }

            else
            {
                ungetc(c, p->file);

                tok.type = parser_check_keyword(p->s);

                if (tok.type == TT_NONE)
                {
                    tok.type = TT_IDENTIFICATOR;
                    tok.s = p->s;

                    p->s = str_init(); //initialize new string
                }

                return tok;
            }

            break;

        case PS_INT_PART_1:

            if(c_prev == '0')
            {
                if (c == '.')
                {
                    str_append_char(p->s, '0');
                    str_append_char(p->s, c);
                    state = PS_FRACTIONAL_PART;
                }
                else
                    error("Incorrect representation of number", ERROR_LEX);
            }

            else if (c >= '1' && c <= '9')
            {
                str_append_char(p->s, c);
                state = PS_INT_PART_2;
            }

            else if (c == '.')
            {
                str_append_char(p->s, c);
                state = PS_FRACTIONAL_PART;
            }

            else
            {
                ungetc(c, p->file);

                tok.type = TT_LIT_INT;
                tok.int_val = str_to_int(p->s);
                return tok;
            }
            break;

        case PS_INT_PART_2:

            if (c >= '0' && c <= '9')
            {
                str_append_char(p->s, c);
                state = PS_INT_PART_2;
            }

            else if (c == '.')
            {
                str_append_char(p->s, c);
                state = PS_FRACTIONAL_PART;
            }

            else if (c == 'E' || c == 'e')
            {
                str_append_char(p->s, c);
                state = PS_EXPONENCIAL_PART;
            }

            else
            {
                ungetc(c, p->file);

                tok.type = TT_LIT_INT;
                tok.int_val = str_to_int(p->s);
                return tok;
            }

            break;

        case PS_FRACTIONAL_PART:

            if( c >= '0' && c <= '9' )
            {
                str_append_char(p->s, c);

                state = PS_FRACTIONAL_PART;
            }

            else if (c == 'E' || c == 'e')
            {
                str_append_char(p->s, c);
                state = PS_EXPONENCIAL_PART;
            }

            else if (c == '.')
                error("Incorrect representation of number", ERROR_LEX);

            else
            {
                ungetc(c, p->file);

                tok.type = TT_LIT_DOUBLE;
                tok.double_val = str_to_double(p->s);
                return tok;
            }

            break;

        case PS_EXPONENCIAL_PART:

            if (c_prev == 'E' || c_prev == 'e' )
                //first time in this state
            {
                if(c == '+' || c == '-')
                {
                    str_append_char(p->s, c);
                    state = PS_EXPONENCIAL_PART;
                }

                else if(c >= '0' && c <= '9')
                    str_append_char(p->s, c);

                else
                    error("Incorect representation of number", ERROR_LEX);
            }

            else if(c >= '0' && c <= '9')
            {
                str_append_char(p->s, c);
                state = PS_EXPONENCIAL_PART;
            }

            else if(c == '.')
                error("Incorrect representation of number", ERROR_LEX);

            else
            {
                ungetc(c, p->file);

                tok.type = TT_LIT_DOUBLE;
                tok.double_val = str_to_double(p->s);
                return tok;
            }

            break;

        case PS_STRING:

            if(c == '\\' )
                state = PS_STRING_BACKSLASH;

            else if(c == '"')
            {
                tok.type = TT_LIT_STRING;
                tok.s = p->s;
                p->s = str_init(); //initialize new string
                return tok;

            }

            else if (c > 31)
                str_append_char(p->s, c);

            break;

        case PS_STRING_BACKSLASH:

            if (c == '"' )
                str_append_char(p->s, '\"');

            else if(c == 'n')
                str_append_char(p->s, '\n');

            else if(c == 't')
                str_append_char(p->s, '\t');

            else if( c == '\\')
                str_append_char(p->s, '\\');

            else if(c == 'x')
            {
                state = PS_STRING_ESCAPE_HEX_1;
                break;
            }

            state = PS_STRING;
            break;

        case PS_STRING_ESCAPE_HEX_1:

            if( is_hex(c) )
            {
                hex_char = hex_decode(c) << 4;
                state = PS_STRING_ESCAPE_HEX_2;
            }

            else
                error("Invalid representation of char in escape sequence.", ERROR_LEX);

            break;

        case PS_STRING_ESCAPE_HEX_2:

            if( is_hex(c) )
            {
                hex_char |= hex_decode(c);

                str_append_char(p->s, hex_char);

                state = PS_STRING;
            }
            else
                error("Invalid representation of char in escape sequence.", ERROR_LEX);

            break;

        case PS_LESS_THEN:

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
                ungetc(c, p->file);

                tok.type = TT_OP_RELATIONAL;
                tok.op_rel = OP_REL_LESS;
                return tok;
            }

            break;

        case PS_GREATER_THEN:

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
                ungetc(c, p->file);

                tok.type = TT_OP_RELATIONAL;
                tok.op_rel = OP_REL_GREATER;
                return tok;
            }

            break;

        case PS_OPERATOR_OF_ASSIGNEMENT:

            if(c == '=')
            {
                tok.type = TT_OP_RELATIONAL;
                tok.op_rel = OP_REL_EQUAL;
                return tok;
            }

            else
            {
                ungetc(c, p->file);
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
enum e_token_type parser_check_keyword(str *s)
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








#include <stdio.h>
#include <ctype.h>
#include "str.h"
#include "error.h"
#include "scanner.h"
#include "token.h"

enum e_scanner_state {PS_DEFAULT,
                     PS_COMMENT,
                     PS_SLASH,
                     PS_BLOCK_COMMENT,
                     PS_BLOCK_COMMENT_ASTERISK,
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
                     PS_OPERATOR_OF_ASSIGNEMENT,
                     PS_EXCLAMATION,
                    };



enum e_token_type scanner_control_type(str_t *s);
enum e_token_type scanner_check_keyword(str_t *s);

scanner_t * scanner_init(char * filename)
{
    // TODO alokovat systematicky
    scanner_t * s = malloc(sizeof(scanner_t));

    s->file = fopen(filename, "r");
    //TODO overit otevreni

    s->str = str_init();

    s->line = 1;

    return s;
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


token_t next_token(scanner_t * s)
{
    token_t tok;
    int c = 0;

    char hex_char;

    enum e_scanner_state state = PS_DEFAULT;

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
                ungetc(c, s->file);
                state = PS_IDENTIFICATOR;
            }

            //start of non negative number
            else if ( is_digit(c) )
            {
                ungetc(c, s->file);
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

            else if(c == '!')
                state = PS_EXCLAMATION;

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
                ungetc(c, s->file);

                tok.type = TT_OP_ARITHEMTIC;
                tok.op_arith = OP_ARITH_DIVIDE;
                return tok;

            }

            else
            {
                ungetc(c, s->file);
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

            if (c == '*')
                state = PS_BLOCK_COMMENT_ASTERISK;

            else
                state = PS_BLOCK_COMMENT;

            break;

        case PS_BLOCK_COMMENT_ASTERISK:

            if (c == '/')
                state = PS_DEFAULT;

            else
                state = PS_BLOCK_COMMENT;

            break;

        case PS_IDENTIFICATOR:

            if ( is_identificator(c) )
            {
                str_append_char(s->str, c);

                state = PS_IDENTIFICATOR;
            }

            else
            {
                ungetc(c, s->file);

                tok.type = scanner_check_keyword(s->str);

                if (tok.type == TT_NONE)
                {
                    tok.type = TT_IDENTIFICATOR;
                    tok.s = s->str;

                    s->str = str_init(); //initialize new string
                }

                return tok;
            }

            break;

        case PS_INT_PART_1:

            if( str_last_char(s->str) == '0' && c == '.')
                state = PS_FRACTIONAL_PART;

            else if( str_last_char(s->str) == '0' && is_digit(c))
                error("Incorrect representation of number", ERROR_LEX);

            else if (c >= '1' && c <= '9')
            {
                state = PS_INT_PART_2;
            }

            else if (c == '0')
            {
                state = PS_INT_PART_1;
            }

            else if (c == '.')
            {

                state = PS_FRACTIONAL_PART;
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

        case PS_INT_PART_2:

            if (c >= '0' && c <= '9')
            {
                str_append_char(s->str, c);
                state = PS_INT_PART_2;
            }

            else if (c == '.')
            {
                str_append_char(s->str, c);
                state = PS_FRACTIONAL_PART;
            }

            else if (c == 'E' || c == 'e')
            {
                str_append_char(s->str, c);
                state = PS_EXPONENCIAL_PART;
            }

            else
            {
                ungetc(c, s->file);

                tok.type = TT_LIT_INT;
                tok.int_val = str_to_int(s->str);
                return tok;
            }

            break;

        case PS_FRACTIONAL_PART:

            if( c >= '0' && c <= '9' )
            {
                str_append_char(s->str, c);

                state = PS_FRACTIONAL_PART;
            }

            else if (c == 'E' || c == 'e')
            {
                str_append_char(s->str, c);
                state = PS_EXPONENCIAL_PART;
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

        case PS_EXPONENCIAL_PART:

            if (str_last_char(s->str) == 'E' || str_last_char(s->str) == 'e' )
                //first time in this state
            {
                if(c == '+' || c == '-')
                {
                    str_append_char(s->str, c);
                    state = PS_EXPONENCIAL_PART;
                }

                else if(c >= '0' && c <= '9')
                    str_append_char(s->str, c);

                else
                    error("Incorect representation of number", ERROR_LEX);
            }

            else if(c >= '0' && c <= '9')
            {
                str_append_char(s->str, c);
                state = PS_EXPONENCIAL_PART;
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

        case PS_STRING:

            if(c == '\\' )
                state = PS_STRING_BACKSLASH;

            else if(c == '"')
            {
                tok.type = TT_LIT_STRING;
                tok.s = s->str;
                s->str = str_init(); //initialize new string
                return tok;

            }

            else if (c > 31)
                str_append_char(s->str, c);

            break;

        case PS_STRING_BACKSLASH:

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

                str_append_char(s->str, hex_char);

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
                ungetc(c, s->file);

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
                ungetc(c, s->file);

                tok.type = TT_OP_RELATIONAL;
                tok.op_rel = OP_REL_GREATER;
                return tok;
            }

            break;

        case PS_EXCLAMATION:

            if(c == '=')
            {
                tok.type = TT_OP_RELATIONAL;
                tok.op_arith = OP_REL_NOT_EQUAL;
                return tok;
            }

            else
                error("Uncomplete non-equal operator.", ERROR_LEX);

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
enum e_token_type scanner_check_keyword(str_t *s)
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

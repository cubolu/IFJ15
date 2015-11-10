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
                     PS_SYMBOL_OF_SUBTRACTION,
                     PS_INT_PART_1,
                     PS_INT_PART_2,
                     PS_FRACTIONAL_PART,
                     PS_EXPONENCIAL_PART,
                     PS_WHITESPACE,
                     PS_FIRST_WHITESPACE,
                     PS_IDENTIFICATOR_OF_DECLARATION,
                     PS_ARGUMENTS_OF_DECLARATION_START,
                     PS_ARGUMENTS_OF_DECLARATION_INSIDE_1,
                     PS_ARGUMENTS_OF_DECLARATION_TYPE,
                     PS_WHITESPACE_1,
                     PS_ARGUMENTS_OF_DECLARATION_IDENTIFICATOR,
                     PS_ARGUMENTS_OF_DECLARATION_END,
                     PS_CHECK_KEYWORD,
                     PS_FOR_LOOP_START,
                     PS_WHITESPACE_2,
                     PS_FOR_LOOP_INICIALISATION_TYPE,
                     PS_WHITESPACE_3,
                     PS_FOR_LOOP_INICIALISATION_IDENTIFICATOR,
                     PS_WHITESPACE_4,
                     PS_FOR_LOOP_INICIALISATION_VALUE_START,
                     PS_FOR_LOOP_INICIALISATION_VALUE_VARIABLE,
                     PS_WHITESPACE_5,
                     PS_FOR_LOOP_INICIALISATION_END,
                     PS_FOR_LOOP_INICIALISATION_VALUE_NUMBER,
                     PS_CALL_FUNCTION_START,
                     PS_CALL_FUNCTION_PARAM_IDENTIFICATOR,
                     PS_CALL_FUNCTION_PARAM_NUMBER_INT_PART,
                     PS_CALL_FUNCTION_PARAM_NUMBER_FRACTIONAL_PART,
                     PS_CALL_FUNCTION_PARAM_STRING,
                     PS_WHITESPACE_6,
                     PS_CALL_FUNCTION_NEXT_PARAM,
                     PS_CALL_FUNCTION_END,
                     PS_IF_START,
                     PS_IF_VARIABLE_1_START,
                     PS_IF_VARIABLE_1_CONTINUE_IDENTIFICATOR,
                     PS_IF_AFTER_FIRST_VARIABLE,
                     PS_IF_COMPARAISON,
                     PS_IF_VARIABLE_2_START,
                     PS_IF_VARIABLE_2_CONTINUE_IDENTIFICATOR,
                     PS_IF_AFTER_SECOND_VARIABLE,
                     PS_STRING,
                     PS_STRING_BACKSLASH,
                     PS_STRING_EXCAPE_SEQUENCE_1,
                     PS_STRING_EXCAPE_SEQUENCE_2
                    };



enum e_variable_type parser_control_type(str *s);
int parser_control_keyword(str *s);

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



token_t parser_next_token(parser * p)
{
    token_t tok;
    int c = 0;
    enum e_parser_state state = PS_DEFAULT, prev_state = PS_DEFAULT;

    int c_before = 0;

    str_clear(p->s);

    while(1)
    {

        c_before = c;
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
                //send operator

                tok.type = TT_ASSIGNMENT;
                return tok;

                state = PS_DEFAULT;
            }
            else if(c == '*')
            {
                //send operator

                tok.type = TT_OPERATOR;
                tok.op.type = OP_MULTIPLY;
                return tok;

                state = PS_DEFAULT;
            }
            else if (c == '{')
            {
                tok.type = TT_BLOCK_START;
                return tok;

                //send start of block
                state = PS_DEFAULT;
            }
            else if (c == '}')
            {
                tok.type = TT_BLOCK_END;
                return tok;

                //send end of block
                state = PS_DEFAULT;
            }
            else if (c == ';')
            {
                tok.type = TT_SEMICOLON;
                return tok;

                //send semicolon
                state = PS_DEFAULT;
            }
            else if (c == '-')
            {
                //TOKEN SEND
                tok.type = TT_OPERATOR;
                tok.op.type = OP_SUBTRACT;
                return tok;
            }
            else
                error("KOKOTINA NEVYRIESENA ESTE.", ERROR_LEX);

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
                //send operator
                ungetc(c, p->file);

                tok.type = TT_OPERATOR;
                tok.op.type = OP_DIVIDE;
                return tok;



                state = PS_DEFAULT;
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
                if (c_before == '*')
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
                if(parser_control_type(p->s) != VT_NOT_A_TYPE)
                {
                    tok.var_or_func_declaration.t = parser_control_type(p->s);
                    str_clear(p->s);
                    //TOKEN TYPE KEYWORD

                    break;

                }
                else if(parser_control_keyword(p->s) !=  KW_NOT_A_KEYWORD){

                    //TOKEN SEND KEYWORD

                }
                else
                {
                    //TOKEN SEND IDENTIFICATOR

                }

            }


            break;



        case PS_INT_PART_1:

            if(c_before == '0')
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

                tok.type = TT_INTEGER;
                tok.integer.value = str_to_int(p->s);
                str_clear(p->s);
                return tok;
                //send token
                state = PS_DEFAULT;
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

                tok.type = TT_INTEGER;

                tok.integer.value = str_to_int(p->s);

                state = PS_DEFAULT;

                return tok;



            }
            break;


        case PS_FRACTIONAL_PART:

            if( c >= '0' && c <= '9' )
            {
                str_append_char(p->s, c);
                //pokracujem v citani znakov
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
                //send token
                ungetc(c, p->file);

                tok.type = TT_DOUBLE;
                tok.double_num.value = str_to_double(p->s);

                return tok;

                state = PS_DEFAULT;
            }

            break;

        case PS_EXPONENCIAL_PART:

            if (c_before == 'E' || c_before == 'e' )
                //first time in this state
            {
                if(c == '+' || c == '-')
                {
                    str_append_char(p->s, c);
                    state = PS_EXPONENCIAL_PART;
                }


                else if(c >= '0' && c <= '9')

                    //TODO
                    ;
                else if(c == '.')
                    error("Incorect representation of number", ERROR_LEX);
            }

            else if(c >= '0' && c <= '9'){

                str_append_char(p->s, c);
                state = PS_EXPONENCIAL_PART;
            }

            else if(c == '.')
                error("Incorrect representation of number", ERROR_LEX);

            else
            {
                ungetc(c, p->file);
                //send number to token

                tok.type = TT_DOUBLE;
                tok.double_num.value = str_to_double(p->s);

                return tok;

                state = PS_DEFAULT;
            }


            break;









        case PS_STRING:

            if(c == '\\' )
            {
                state = PS_STRING_BACKSLASH;
            }
            else if(c == '"')
            {
                //TOKEN SEND, end of string
            }
            else if (c > '31')
            {
                str_append_char(p->s, c);
                //save char
            }
            break;


        case PS_STRING_BACKSLASH:

            if (c == '"' )
            {

                str_append_char(p->s, '\"');
            }
            else if(c == 'n')
            {
                str_append_char(p->s, '\n');
            }
            else if(c == 't')
            {
                str_append_char(p->s, '\t');
            }
            else if( c == '\\')
            {
                str_append_char(p->s, '\\');
            }
            else if(c == 'x')
            {
                state = PS_STRING_EXCAPE_SEQUENCE_1;
            }


            break;

        case PS_STRING_EXCAPE_SEQUENCE_1:

            //TODO finish
            if((c >= '0' && c <= '9') || (c >= 'A' && c >= 'F') )
            {
                state = PS_STRING_EXCAPE_SEQUENCE_2;
            }
            else
                error("Invalid representation of char in escape sequence.", ERROR_LEX);

            break;


        case PS_STRING_EXCAPE_SEQUENCE_2:

            if((c >= '0' && c <= '9') || (c >= 'A' && c >= 'F') )
            {
                //TODO SAVE FOUND CHAR
                state = PS_STRING;
            }
            else
                error("Invalid representation of char in escape sequence.", ERROR_LEX);


            break;
        }

    }
}




/************************************
 * check for type keywords of language
 ************************************/
enum e_variable_type parser_control_type(str *s)
{

    if (str_equals(s, "double"))
    {
        //save to token as type

        return VT_DOUBLE;
    }
    else if(str_equals(s, "string"))
    {

        return VT_STRING;
    }
    else if(str_equals(s, "auto"))
    {

        return VT_AUTO;
    }
    else if(str_equals(s, "int"))
    {

        return VT_INT;
    }
    else
        return VT_NOT_A_TYPE;
}


/************************************
 * check for keywords of language
 ************************************/
int parser_control_keyword(str *s)
{
    if (str_equals(s, "cin"))
        return KW_CIN;

    else if (str_equals(s, "cout"))
        return KW_COUT;

    else if (str_equals(s, "for"))
        return KW_FOR;

    else if (str_equals(s, "if"))
        return KW_IF;

    else if (str_equals(s, "return"))
        return KW_RETURN;

    else if (str_equals(s, "else"))
        return KW_ELSE;
    else
        return KW_NOT_A_KEYWORD;
}








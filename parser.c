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

            //eats whitespace
            if ( is_whitespace(c) )
                state = PS_DEFAULT;

            //start od commentar
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
                state = PS_SYMBOL_OF_SUBTRACTION;
            }

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
                    //We found keyword type
                    //do something with keyword
                    state = PS_FIRST_WHITESPACE;
                    break;

                }
                else if(parser_control_keyword(p->s) == 0){

                    //we found keyword (except type)
                    state = PS_CHECK_KEYWORD;
                }

                //do something with identificator
                state = PS_DEFAULT;
            }


            break;

        case PS_SYMBOL_OF_SUBTRACTION:

            //decision between operation SUBTRACTION or start of Negative number
            if( is_whitespace(c) )
            {
                tok.type = TT_OPERATOR;
                tok.op.type = OP_SUBTRACT;

                return tok;
            }

            else if( is_digit(c) )
            {
                str_append_char(p->s, '-');
                str_append_char(p->s, c);
                state = PS_INT_PART_1;
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

            if (c >= '1' && c <= '9')
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

                return tok;

                state = PS_DEFAULT;

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

        case PS_FIRST_WHITESPACE:

            if ( ! is_whitespace(c) )
            {
                error("You can't use type without name",ERROR_LEX);
            }
            else
            {
                state = PS_WHITESPACE;
            }
            break;

        case PS_WHITESPACE:

            if( is_whitespace(c) )
                state = PS_WHITESPACE;

            else if ( is_identificator_start(c) )
            {
                ungetc(c, p->file);
                state = PS_IDENTIFICATOR_OF_DECLARATION;
            }

            else
            {
                error("Type without following identificator", ERROR_LEX);
            }

            break;

        case PS_ARGUMENTS_OF_DECLARATION_START:

            //variable is declared, state control next char and decide between declaration of function, declaration of variable and inicialisation
            if ( is_whitespace(c) )
                state = PS_ARGUMENTS_OF_DECLARATION_START;
            else if( c == '(' ){
                state = PS_ARGUMENTS_OF_DECLARATION_INSIDE_1;

            }
            else if (c == ';' || c == '=')
            {
                ungetc(c, p->file);
                //send token as declaration of variable

                tok.type = TT_VARIABLE_DECLARATION;
                tok.var_or_func_declaration.name = p->s;
                //tok.var_or_func_declaration.t = VT_AUTO; // TODO str_to_type()

                return tok;

                state = PS_DEFAULT;
            }
            else
                error("Incorrect declaration.", ERROR_LEX);

            break;


        case PS_ARGUMENTS_OF_DECLARATION_INSIDE_1:

            //Inside parameters of declaration of function
            if( is_whitespace(c) )
                state = PS_ARGUMENTS_OF_DECLARATION_INSIDE_1;
            else if( is_identificator_start(c) )
            {
                //save char
                state = PS_ARGUMENTS_OF_DECLARATION_TYPE;
            }
            else if(c == ')')
            {
                //send token
                next_state(PS_DEFAULT);
            }
            else
            {
                error("Nonvalid declaration of function.", ERROR_LEX);
            }

            break;

        case PS_ARGUMENTS_OF_DECLARATION_TYPE:

            //loading the type of variable inside of declaration of function
            if ( is_identificator(c) )
            {
                //save char
                state = PS_ARGUMENTS_OF_DECLARATION_TYPE;
            }
            else if ( is_whitespace(c) )
            {
                if(parser_control_type(p->s) == VT_NOT_A_TYPE)
                {
                    state = PS_WHITESPACE_1;

                }
                else
                {
                    error("Type of variable is not correct.", ERROR_LEX);
                }

            }
            else
            {
                error("Type of variable is not correct.", ERROR_LEX);
            }
            break;


        case PS_WHITESPACE_1:

            //Interstate, eats whitespaces
            if( is_whitespace(c) )
                state = PS_WHITESPACE_1;
            else if( is_identificator_start(c) )
            {
                //save char
                state = PS_ARGUMENTS_OF_DECLARATION_IDENTIFICATOR;

            }
            else
                error("Nonvalid name of variable.", ERROR_LEX);

            break;

        case PS_ARGUMENTS_OF_DECLARATION_IDENTIFICATOR:

            //identificator of variable inside of declaration of function
            if( is_identificator(c) )
            {
                //save char
                state = PS_ARGUMENTS_OF_DECLARATION_IDENTIFICATOR;
            }
            else if( is_whitespace(c) || (c == ',') || (c == ')'))
            {
                ungetc(c, p->file);
                if(parser_control_keyword(p->s) != 0)
                    state = PS_ARGUMENTS_OF_DECLARATION_END;
                else
                    error("Identificator of variable can't be keyword.", ERROR_LEX);

            }
            else
                error("Nonvalid name of variable.", ERROR_LEX);


            break;

        case PS_ARGUMENTS_OF_DECLARATION_END:

            //possible end of declartaion of function... decide between end, or continuing in loading params
            if( is_whitespace(c)  )
                state = PS_ARGUMENTS_OF_DECLARATION_END;
            else if (c == ')')
            {
                tok.type = TT_FUNCTION_DECLARATION;

                return tok;
                state = PS_DEFAULT;
            }
            else if(c == ',')
            {
                //save / create new token for next declaration of variable inside declaration of function
                // TODO wtf
                state = PS_ARGUMENTS_OF_DECLARATION_TYPE;
            }
            else
                error("Nonvalid declaration of function.", ERROR_LEX);

            break;

        case PS_IDENTIFICATOR_OF_DECLARATION:

            if ( is_identificator(c) )
            {
                state = PS_IDENTIFICATOR_OF_DECLARATION;
                str_append_char(p->s, c);
                //save char in string
            }


            else
            {
                ungetc(c, p->file);
                if(parser_control_keyword(p->s) == 0)
                {
                    //Identificator is keyword

                    error("Type keyword can't be followed by keyword.", ERROR_LEX);

                }
                else
                {

                    state = PS_ARGUMENTS_OF_DECLARATION_START;
                }

            }

            break;

        case PS_CHECK_KEYWORD:

            //keyword is not type, we decide between others keywords
            if (str_equals(p->s, "for"))
            {
                state = PS_FOR_LOOP_START;
            }
            else if(str_equals(p->s, "return"))
            {
                //send token
                tok.type = TT_RETURN;

                return tok;
                state = PS_DEFAULT;
            }
            else if(str_equals(p->s, "if"))
            {
                next_state(PS_IF_START);
            }

            break;


        case PS_FOR_LOOP_START:

            //start of for loop
            if( is_whitespace(c) )
                state = PS_FOR_LOOP_START;
            else if (c == '(')
            {
                //save char?
                state = PS_WHITESPACE_2;
            }
            else
                error("non valid for cycle.", ERROR_LEX);


            break;

        case PS_WHITESPACE_2:

            //inside of declaration of for loop, eating whitespace
            if( is_whitespace(c) )
                state = PS_WHITESPACE_2;
            else if ( ( c >= 'A' && c <= 'Z') || ( c > 'a' && c < 'z') )
            {
                //save char
                state = PS_FOR_LOOP_INICIALISATION_TYPE;
            }
            else
                error("In inicialisation of for loop, type of variable is necessary.", ERROR_LEX);

            break;

        case PS_FOR_LOOP_INICIALISATION_TYPE:

            //loading type of varaible in inicialisation of for loop
            if ( ( c >= 'A' && c <= 'Z') || ( c > 'a' && c < 'z') )
            {
                //save char
                state = PS_FOR_LOOP_INICIALISATION_TYPE;
            }
            else if( is_whitespace(c) )
            {
                if(parser_control_type(p->s) != VT_NOT_A_TYPE)
                {
                    //keyword type is OK, save to token
                    state = PS_WHITESPACE_3;
                }
                else
                    error("Inicialisation of expression in for loop is invalid.", ERROR_LEX);
            }

            else
                error("Inicialisation of expression in for loop is invalid.", ERROR_LEX);

            break;

        case PS_WHITESPACE_3:

            //eats whitespace, looking for start of identificator of variable
            if( is_whitespace(c) )
                state = PS_WHITESPACE_3;
            if ( is_identificator_start(c) )
            {
                //save char
                state = PS_FOR_LOOP_INICIALISATION_IDENTIFICATOR;
            }

            break;


        case PS_FOR_LOOP_INICIALISATION_IDENTIFICATOR:

            //loading identificator of variable
            if ( is_identificator(c) )
            {
                //save char
                state = PS_FOR_LOOP_INICIALISATION_IDENTIFICATOR;
            }
            else if(c == '=')
            {
                if(parser_control_keyword(p->s) != 0)
                {
                    state = PS_FOR_LOOP_INICIALISATION_VALUE_START;
                }
                else
                    error("Identificator can't be represented by keyword.", ERROR_LEX);
            }
            else if ( is_whitespace(c) )
                state = PS_WHITESPACE_4;
            else
                error("Invalid identificator.", ERROR_LEX);

            break;

        case PS_WHITESPACE_4:

            //after identificator, looking for '='
            if( is_whitespace(c) )
                state = PS_WHITESPACE_4;
            else if (c == '=')
            {
                if(parser_control_keyword(p->s) != 0)
                {
                    state = PS_FOR_LOOP_INICIALISATION_VALUE_START;
                }
                else
                    error("Identificator can't be represented by keyword.", ERROR_LEX);
            }
            else
                error("Invalid identificator.", ERROR_LEX);

            break;


        case PS_FOR_LOOP_INICIALISATION_VALUE_START:

            //'='found, waiting for start of variable or number. Variable can later become calling of function. Now I don' t see that.
            if( is_whitespace(c) )
                state = PS_FOR_LOOP_INICIALISATION_VALUE_START;
            else if ( is_identificator_start(c) )
            {
                //save char
                state = PS_FOR_LOOP_INICIALISATION_VALUE_VARIABLE;
            }
            else if( (c >= '1' && c <= '9') || c == '-' || c == '+')
            {
                //save char
                state = PS_FOR_LOOP_INICIALISATION_VALUE_NUMBER;
            }
            else
                error("Invalid inicialisation of variable.", ERROR_LEX);

            break;


        case PS_FOR_LOOP_INICIALISATION_VALUE_VARIABLE:

            //loading of identificator of variable in inicialisation of for loop
            if ( is_identificator(c) )
            {
                //save char
                state = PS_FOR_LOOP_INICIALISATION_VALUE_VARIABLE;
            }
            else if( is_whitespace(c) )
                state = PS_WHITESPACE_5;
            else if(c == ';')
            {
                //save to token
                state = PS_FOR_LOOP_INICIALISATION_END;
            }
            else
                error("Invalid inicialisation part of for loop.", ERROR_LEX);

            break;


        case PS_WHITESPACE_5:

            //eats whitespace and waits on ';'
            if( is_whitespace(c) )
                state = PS_WHITESPACE_4;

            else if (c == ';')
            {
                //save to token
                state = PS_FOR_LOOP_INICIALISATION_END;
            }
            else
                error("Invalid inicialisation part of for loop.", ERROR_LEX);

            break;

        case PS_FOR_LOOP_INICIALISATION_VALUE_NUMBER:

            //in case when value is in forme of number
            if (c >= '0' && c <= '9')
            {
                //SAVE CHAR
                state = PS_FOR_LOOP_INICIALISATION_VALUE_NUMBER;
            }
            else if( is_whitespace(c) )
                state = PS_WHITESPACE_5;
            else if (c == ';')
            {
                //save char
                state = PS_FOR_LOOP_INICIALISATION_END;
            }
            else
                error("Invalid inicialisation part of for loop.", ERROR_LEX);
            break;

        case PS_FOR_LOOP_INICIALISATION_END:

            //inicialisation part ended, looking for first char of condition
            if( is_whitespace(c))
                state = PS_FOR_LOOP_INICIALISATION_END;



            break;

        case PS_CALL_FUNCTION_START:

            //possible start of parameters of function, depends on last state

            //control of last state --> IF
            if( is_whitespace(c) );
            //dont change state

            else if ( is_identificator_start(c) )
            {
                str_append_char(p->s, c);
                next_state(PS_CALL_FUNCTION_PARAM_IDENTIFICATOR);
            }
            else if( (c >= '1' && c <= '9') || c == '-' || c == '+')
            {
                str_append_char(p->s, c);
                next_state(PS_CALL_FUNCTION_PARAM_NUMBER_INT_PART);
            }
            else if(c == '"')
            {
                str_append_char(p->s, c);
                next_state(PS_CALL_FUNCTION_PARAM_STRING);
            }

            else
                error("Invalid parameter of function.", ERROR_LEX);

            break;

        case PS_CALL_FUNCTION_PARAM_IDENTIFICATOR:


            if ( is_identificator(c) )
            {
                str_append_char(p->s, c);
            }
            else if ( is_whitespace(c) )
            {
                next_state(PS_WHITESPACE_6);
            }
            else if(c == ',')
            {
                next_state(PS_CALL_FUNCTION_NEXT_PARAM);
            }
            else if(c == ')')
            {
                next_state(PS_CALL_FUNCTION_END);
            }
            else
                error("Invalid parameter of function.", ERROR_LEX);


            break;

        case PS_WHITESPACE_6:

            if ( is_whitespace(c) );

            else if(c == ',')
            {
                next_state(PS_CALL_FUNCTION_NEXT_PARAM);
            }
            else if(c == ')')
            {
                next_state(PS_CALL_FUNCTION_END);
            }
            else
                error("Invalid parameter of function.", ERROR_LEX);

            break;

        case PS_CALL_FUNCTION_NEXT_PARAM:

            if( is_whitespace(c) );
            //dont change state

            else if ( is_identificator_start(c) )
            {
                str_append_char(p->s, c);
                next_state(PS_CALL_FUNCTION_PARAM_IDENTIFICATOR);
            }
            else if( (c >= '1' && c <= '9') || c == '-' || c == '+')
            {
                str_append_char(p->s, c);
                next_state(PS_CALL_FUNCTION_PARAM_NUMBER_INT_PART);
            }
            else if(c == '"')
            {
                str_append_char(p->s, c);
                next_state(PS_CALL_FUNCTION_PARAM_STRING);
            }

            else
                error("Invalid parameter of function.", ERROR_LEX);

            break;

        case PS_CALL_FUNCTION_PARAM_NUMBER_INT_PART:

            //just in case of first time in this case
            if(prev_state == PS_CALL_FUNCTION_START || prev_state == PS_CALL_FUNCTION_NEXT_PARAM)
            {
                if(c_before == '0')
                {
                    if (c == '.')
                        next_state(PS_CALL_FUNCTION_PARAM_NUMBER_FRACTIONAL_PART);
                    else
                        error("Incorrect representation parameter of function.", ERROR_LEX);
                }
                else if (c >= '1' && c <= '9')
                {
                    str_append_char(p->s, c);
                    next_state(PS_CALL_FUNCTION_PARAM_NUMBER_INT_PART);
                }

                else if (c == '.' && (c_before != '+' || c_before != '-'))
                    next_state(PS_CALL_FUNCTION_PARAM_NUMBER_FRACTIONAL_PART);



                else if( is_whitespace(c) && (c_before != '+' || c_before != '-') )
                {
                    next_state(PS_WHITESPACE_6);
                }
                else if(c == ')' && (c_before != '+' || c_before != '-') )
                {
                    next_state(PS_CALL_FUNCTION_END);
                }
                else
                    error("Invalid parameter of function.", ERROR_LEX);
            }

            else if (c >= '1' && c <= '9')
            {
                //spracuj
                str_append_char(p->s, c);

            }
            else if (c == '.')
            {
                //spracuj
                str_append_char(p->s, c);
                state = PS_CALL_FUNCTION_PARAM_NUMBER_FRACTIONAL_PART;
            }

            else if( is_whitespace(c) )
            {
                next_state(PS_WHITESPACE_6);
            }
            else if(c == ')' )
            {
                next_state(PS_CALL_FUNCTION_END);
            }
            else
                error("Invalid parameter of function.", ERROR_LEX);



            break;

        case PS_CALL_FUNCTION_PARAM_NUMBER_FRACTIONAL_PART:

            if (c >= '1' && c <= '9')
            {
                str_append_char(p->s, c);

            }
            else if (c == ')')
            {
                next_state(PS_CALL_FUNCTION_END);
            }
            else if ( is_whitespace(c) )
            {
                next_state(PS_WHITESPACE_6);
            }
            else if (c == ',')
            {
                next_state(PS_CALL_FUNCTION_NEXT_PARAM);
            }
            else
                error("Invalid parameter of function.", ERROR_LEX);

            break;

        case PS_CALL_FUNCTION_PARAM_STRING:

            if (c != '"')
                str_append_char(p->s, c);

            else
                next_state(PS_WHITESPACE_6);

            break;


        case PS_CALL_FUNCTION_END:

            //send token of calling function
            //TODO structure of tokens for declarations of function works how?
            tok.type = TT_FUNCTION_CALL;

            return tok;

            break;

        case PS_IF_START:

            if ( is_whitespace(c) );

            else if( c == '(')
            {
                //save char?
                next_state(PS_IF_VARIABLE_1_START);
            }
            else
                error("If has to be followed by '(", ERROR_LEX);


            break;

        case PS_IF_VARIABLE_1_START:

            if ( is_whitespace(c) );

            else if ( is_digit(c) || c == '+' || c == '-')
            {
                //save char
                //TODO state for numbers
            }
            else if ( is_identificator_start(c))
            {
                //save char
                next_state(PS_IF_VARIABLE_1_CONTINUE_IDENTIFICATOR);
            }
            else
                error("Expected identificator of variable or number.", ERROR_LEX);

            break;

        case PS_IF_VARIABLE_1_CONTINUE_IDENTIFICATOR:

            if (is_identificator(c))
            {
                //save char
            }
            else
            {
                ungetc(c, p->file);
                next_state(PS_IF_AFTER_FIRST_VARIABLE);

            }

            break;

        case PS_IF_AFTER_FIRST_VARIABLE:

            if(is_whitespace(c));

            else if(c == ')')
            {
                //send token
                next_state(PS_DEFAULT);
            }
            else if(c == '<' || c == '>' || c == '!' || c == '=')
            {
                //save char
                next_state(PS_IF_COMPARAISON);
            }
            else
                error("Expected comparaison", ERROR_LEX);

            break;

        case PS_IF_COMPARAISON:

            if (c == '=')
            {
               //save char
                next_state(PS_IF_VARIABLE_2_START);
            }
            else
            {
                ungetc(c, p->file);
                next_state(PS_IF_VARIABLE_2_START);
            }
            break;

        case PS_IF_VARIABLE_2_START:

            if ( is_whitespace(c) );

            else if ( is_digit(c) || c == '+' || c == '-')
            {
                //save char
                //TODO state for numbers
            }
            else if ( is_identificator_start(c))
            {
                //save char
                next_state(PS_IF_VARIABLE_2_CONTINUE_IDENTIFICATOR);
            }
            else
                error("Expected identificator of variable or number.", ERROR_LEX);

            break;

        case PS_IF_VARIABLE_2_CONTINUE_IDENTIFICATOR:

            if (is_identificator(c))
            {
                //save char
            }

            else
            {
                ungetc(c, p->file);
                next_state(PS_IF_AFTER_SECOND_VARIABLE);
            }

            break;


        case PS_IF_AFTER_SECOND_VARIABLE:

            if(is_whitespace(c));

            else if (')')
            {
                //send token
                next_state(PS_DEFAULT);
            }
            else
                error("Expected ')'.", ERROR_LEX);

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
    if (str_equals(s, "double") || str_equals(s, "string") || str_equals(s, "auto") || str_equals(s, "int") || str_equals(s, "cin") || str_equals(s, "cout") ||
            str_equals(s, "for") || str_equals(s, "if") || str_equals(s, "return") || str_equals(s, "else"))
        return 0;
    else
        return 1;
}








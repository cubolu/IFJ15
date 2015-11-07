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
                    PS_CALL_FUNCTION_STRING};



int parser_control_type(str *s);
int parser_control_keyword(str *s);

parser * parser_init(char * filename)
{
    // TODO alokovat systematicky
    parser * p = malloc(sizeof(parser));

    p->file = fopen(filename, "r");
    //TODO overit otevreni

    return p;
}

#define next_state(s)   ( prev_state = state, state = s )

int parser_next_token(parser * p)
{
    //datovy typ token
    int c = NULL;
    enum e_parser_state state, prev_state = PS_DEFAULT;

    int c_before = NULL;

    str *s = str_init();

    while(1)
    {

        c_before = c;
        c = getc(p->file);

        switch(state)
        {
        case PS_DEFAULT:

            //default state

            //eats whitespace
            if (c == '\n' || c == ' ' || c == '\t')
                state = PS_DEFAULT;

            //start od commentar
            else if(c== '/')
                state = PS_SLASH;

            //start of identificator
            else if( c == '_' || ( c >= 'A' && c <= 'Z') || ( c >= 'a' && c < 'z') )
            {
                ungetc(c, p->file);
                state = PS_IDENTIFICATOR;
            }

            //start of non negative number
            else if ( c >= '0' && c < '9')
                state = PS_INT_PART_1;
            else if (c == '=')
            {
                //send operator
                state = PS_DEFAULT;
            }
            else if(c == '*')
            {
                //send operator
                state = PS_DEFAULT;
            }
            else if (c == '{')
            {
                //send start of block
                state = PS_DEFAULT;
            }
            else if (c == '}')
            {
                //send end of block
                state = PS_DEFAULT;
            }
            else if (c == ';')
            {
                //send semicolon
                state = PS_DEFAULT;
            }
            else if (c == '-')
            {
                //save char
                state = PS_SYMBOL_OF_SUBTRACTION;
            }



            break;


        case PS_SLASH:

            if ( c == '/')
                state = PS_COMMENT;
            else if ( c == '*')
                state = PS_BLOCK_COMMENT;
            else if( c == '_' || ( c >= 'A' && c <= 'Z') || ( c > 'a' && c < 'z') )
            {
                //send operator
                ungetc(c, p->file);

                state = PS_DEFAULT;
            }
            else if (c == '\n' || c == ' ' || c == '\t')
            {
                //send operator
                ungetc(c, p->file);
                state = PS_DEFAULT;
            }
            else if ( c >= '0' && c < '9')
            {
                //send operator
                ungetc(c, p->file);
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


            if ( c == '_' || ( c >= 'A' && c <= 'Z') || ( c > 'a' && c < 'z') || ( c >= '0' && c <= '9') )
            {
                str_append_char(s, c);

                state = PS_IDENTIFICATOR;
            }

            else
            {
                    ungetc(c, p->file);
                    if(parser_control_type(s) == 0)
                    {
                        //We found keyword type
                        //do something with keyword
                        state = PS_FIRST_WHITESPACE;

                    }
                    else if(parser_control_keyword(s) == 0){

                        //we found keyword (except type)
                        state = PS_CHECK_KEYWORD;
                    }

                    //do something with identificator
                    state = PS_DEFAULT;
            }


            break;

        case PS_SYMBOL_OF_SUBTRACTION:

            //decision between operation SUBTRACTION or start of Negative number
            if(c == '\n' || c == ' ' || c == '\t')
            {
                //send token with operation SUBTRACTION
                state = PS_DEFAULT;
            }
            else if( c >= '0' && c < '9')
                state = PS_INT_PART_1;

            break;

        case PS_INT_PART_1:

            if(c_before == '0')
            {
                if (c == '.')
                {
                    state = PS_FRACTIONAL_PART;
                }
                else
                    error("Incorrect representation of number", ERROR_LEX);
            }
            else if (c >= '1' && c <= '9')
                {
                    //spracuj znak
                    state = PS_INT_PART_2;
                }

            else if (c == '.')
                state = PS_FRACTIONAL_PART;



            else{
                ungetc(c, p->file);
                //send token
                state = PS_DEFAULT;
            }
            break;

        case PS_INT_PART_2:

            if (c >= '1' && c <= '9')
            {
                //spracuj
                state = PS_INT_PART_2;
            }
            else if (c == '.')
            {
                //spracuj
                state = PS_FRACTIONAL_PART;
            }
            else if (c == 'E' || c == 'e')
            {
                //spracuj
                state = PS_EXPONENCIAL_PART;
            }
            else
            {
                //send token
                ungetc(c, p->file);
                state = PS_DEFAULT;

            }
            break;


        case PS_FRACTIONAL_PART:

            if( c >= '0' && c <= '9' )
                //spracuj
                //pokracujem v citani znakov
                state = PS_FRACTIONAL_PART;

            else if (c == 'E' || c == 'e')
            {
                //spracuj
                state = PS_EXPONENCIAL_PART;
            }
            else if (c == '.')
                error("Incorrect representation of number", ERROR_LEX);

            else
            {
                //send token
                ungetc(c, p->file);
                state = PS_DEFAULT;
            }



            break;

        case PS_EXPONENCIAL_PART:


            if (c_before == 'E' || c_before == 'e' )
            //first time in this state
            {
                if(c == '+' || c == '-')
                {
                    //spracuj
                    state = PS_EXPONENCIAL_PART;
                }


                else if(c >= '0' && c <= '9')

                    //TODO
                    ;
                else if(c == '.')
                    error("Incorect representation of number", ERROR_LEX);
            }

            else if(c >= '0' && c <= '9'){

                //spracuj
                state = PS_EXPONENCIAL_PART;
            }

            else if(c == '.')
                error("Incorrect representation of number", ERROR_LEX);

            else
            {
                ungetc(c, p->file);
                //send number to token
                state = PS_DEFAULT;
            }


            break;

        case PS_FIRST_WHITESPACE:

            if (c != ' ' || c != '\t' || c != '\n' )
            {
                error("You can't use type without name",ERROR_LEX);
            }
            else
            {
                state = PS_WHITESPACE;
            }
            break;

        case PS_WHITESPACE:

            if(c == '\n' || c == ' ' || c == '\t')
                state = PS_WHITESPACE;

            else
            {
                ungetc(c, p->file);
                state = PS_IDENTIFICATOR_OF_DECLARATION;
            }
            break;

        case PS_ARGUMENTS_OF_DECLARATION_START:

            //variable is declared, state control next char and decide between declaration of function, declaration of variable and inicialisation
            if (c != ' ' || c != '\t' || c != '\n' )
                state = PS_ARGUMENTS_OF_DECLARATION_START;
            else if( c == '(' ){
                state = PS_ARGUMENTS_OF_DECLARATION_INSIDE_1;

            }
            else if (c == ';')
            {
                //send token as declaration of variable
                state = PS_DEFAULT;

            }
            else if(c == '=')
            {
                ungetc(c, p->file);
                state = PS_DEFAULT;
                //send token as declaration of variable

            }
            else
                error("Incorrect declaration.", ERROR_LEX);

            break;


        case PS_ARGUMENTS_OF_DECLARATION_INSIDE_1:

            //Inside parameters of declaration of function
            if(c == '\n' || c == ' ' || c == '\t')
                state = PS_ARGUMENTS_OF_DECLARATION_INSIDE_1;
            else if( c == '_' || ( c >= 'A' && c <= 'Z') || ( c > 'a' && c < 'z') )
            {
                //save char
                state = PS_ARGUMENTS_OF_DECLARATION_TYPE;
            }
            else
            {
                error("Nonvalid declaration of function.", ERROR_LEX);
            }

            break;

        case PS_ARGUMENTS_OF_DECLARATION_TYPE:

            //loading the type of variable inside of declaration of function
            if ( c == '_' || ( c >= 'A' && c <= 'Z') || ( c > 'a' && c < 'z') )
            {
                //save char
                state = PS_ARGUMENTS_OF_DECLARATION_TYPE;
            }
            else if (c == '\n' || c == ' ' || c == '\t')
            {
                if(parser_control_type(s) == 0)
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
            if(c == '\n' || c == ' ' || c == '\t')
                state = PS_WHITESPACE_1;
            else if( c == '_' || ( c >= 'A' && c <= 'Z') || ( c > 'a' && c < 'z') )
            {
                //save char
                state = PS_ARGUMENTS_OF_DECLARATION_IDENTIFICATOR;

            }
            else
                error("Nonvalid name of variable.", ERROR_LEX);

            break;

        case PS_ARGUMENTS_OF_DECLARATION_IDENTIFICATOR:

            //identificator of variable inside of declaration of function
            if( c == '_' || ( c >= 'A' && c <= 'Z') || ( c > 'a' && c < 'z') || ( c >= '0' && c <= '9') )
            {
                //save char
                state = PS_ARGUMENTS_OF_DECLARATION_IDENTIFICATOR;
            }
            else if((c == '\n' || c == ' ' || c == '\t') || (c == ',') || (c == ')'))
            {
                ungetc(c, p->file);
                if(parser_control_keyword(s) != 0)
                    state = PS_ARGUMENTS_OF_DECLARATION_END;
                else
                    error("Identificator of variable can't be keyword.", ERROR_LEX);

            }
            else
                error("Nonvalid name of variable.", ERROR_LEX);


            break;

        case PS_ARGUMENTS_OF_DECLARATION_END:

            //possible end of declartaion of function... decide between end, or continuing in loading params
            if(c == '\n' || c == ' ' || c == '\t')
                state = PS_ARGUMENTS_OF_DECLARATION_END;
            else if (c == ')')
            {
                //save and send token
                state = PS_DEFAULT;
            }
            else if(c == ',')
            {
                //save / create new token for next declaration of variable inside declaration of function
                state = PS_ARGUMENTS_OF_DECLARATION_TYPE;
            }
            else
                error("Nonvalid declaration of function.", ERROR_LEX);

            break;

        case PS_IDENTIFICATOR_OF_DECLARATION:

            if ( c == '_' || ( c >= 'A' && c <= 'Z') || ( c > 'a' && c < 'z') || ( c >= '0' && c <= '9') )
            {
                state = PS_IDENTIFICATOR_OF_DECLARATION;
                //save char in string
            }


            else
            {
                ungetc(c, p->file);
                if(parser_control_keyword(s) == 0)
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
            if (str_equals(s, "for"))
            {
                state = PS_FOR_LOOP_START;
            }
            else if(str_equals(s, "return"))
            {
                //send token
                state = PS_DEFAULT;
            }

            break;


        case PS_FOR_LOOP_START:

            //start of for loop
            if(c == '\n' || c == ' ' || c == '\t')
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
            if(c == '\n' || c == ' ' || c == '\t')
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
            else if(c == '\n' || c == ' ' || c == '\t')
            {
                if(parser_control_type(s) == 0)
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
            if(c == '\n' || c == ' ' || c == '\t')
                state = PS_WHITESPACE_3;
            if ( c == '_' || ( c >= 'A' && c <= 'Z') || ( c > 'a' && c < 'z') )
            {
                //save char
                state = PS_FOR_LOOP_INICIALISATION_IDENTIFICATOR;
            }

            break;


        case PS_FOR_LOOP_INICIALISATION_IDENTIFICATOR:

            //loading identificator of variable
            if ( c == '_' || ( c >= 'A' && c <= 'Z') || ( c > 'a' && c < 'z') || ( c >= '0' && c <= '9') )
            {
                //save char
                state = PS_FOR_LOOP_INICIALISATION_IDENTIFICATOR;
            }
            else if(c == '=')
            {
                if(parser_control_keyword(s) != 0)
                {
                    state = PS_FOR_LOOP_INICIALISATION_VALUE_START;
                }
                else
                    error("Identificator can't be represented by keyword.", ERROR_LEX);
            }
            else if (c == '\n' || c == ' ' || c == '\t')
                state = PS_WHITESPACE_4;
            else
                error("Invalid identificator.", ERROR_LEX);

            break;

        case PS_WHITESPACE_4:

            //after identificator, looking for '='
            if(c == '\n' || c == ' ' || c == '\t')
                state = PS_WHITESPACE_4;
            else if (c == '=')
            {
                if(parser_control_keyword(s) != 0)
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
            if(c == '\n' || c == ' ' || c == '\t')
                state = PS_FOR_LOOP_INICIALISATION_VALUE_START;
            else if ( c == '_' || ( c >= 'A' && c <= 'Z') || ( c > 'a' && c < 'z') )
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
            if ( c == '_' || ( c >= 'A' && c <= 'Z') || ( c > 'a' && c < 'z') || ( c >= '0' && c <= '9') )
            {
                //save char
                state = PS_FOR_LOOP_INICIALISATION_VALUE_VARIABLE;
            }
            else if(c == '\n' || c == ' ' || c == '\t')
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
            if(c == '\n' || c == ' ' || c == '\t')
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
            else if(c == '\n' || c == ' ' || c == '\t')
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
            if(c == '\n' || c == ' ' || c == '\t')
                state = PS_FOR_LOOP_INICIALISATION_END;



            break;

        case PS_CALL_FUNCTION_START:

            //possible start of parameters of function, depends on last state

            //control of last state --> IF
            if(c == '\n' || c == ' ' || c == '\t');
               //dont change state

            else if ( c == '_' || ( c >= 'A' && c <= 'Z') || ( c > 'a' && c < 'z') )
            {
                //save char
                next_state(PS_CALL_FUNCTION_PARAM_IDENTIFICATOR);
            }
            else if( (c >= '1' && c <= '9') || c == '-' || c == '+')
            {
                //save char
                next_state(PS_CALL_FUNCTION_PARAM_NUMBER_INT_PART);
            }
            else if(c == '"')
            {
                //save char
                next_state(PS_CALL_FUNCTION_STRING);
            }

            else
                error("Invalid parameter of function.", ERROR_LEX);

            break;

        case PS_CALL_FUNCTION_PARAM_IDENTIFICATOR:


            if ( c == '_' || ( c >= 'A' && c <= 'Z') || ( c > 'a' && c < 'z') || ( c >= '0' && c <= '9') )
            {
                //save char
            }
            else if (c == '\n' || c == ' ' || c == '\t')
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

            if (c == '\n' || c == ' ' || c == '\t');

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

        case PS_CALL_FUNCTION_END:

            if(c == '\n' || c == ' ' || c == '\t');
               //dont change state

            else if ( c == '_' || ( c >= 'A' && c <= 'Z') || ( c > 'a' && c < 'z') )
            {
                //save char
                next_state(PS_CALL_FUNCTION_PARAM_IDENTIFICATOR);
            }
            else if( (c >= '1' && c <= '9') || c == '-' || c == '+')
            {
                //save char
                next_state(PS_CALL_FUNCTION_PARAM_NUMBER);
            }
            else if(c == '"')
            {
                //save char
                next_state(PS_CALL_FUNCTION_STRING);
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
                    if (c == '.');
                        next_state(PS_CALL_FUNCTION_PARAM_NUMBER_FRACTIONAL_PART);
                    else
                        error("Incorrect representation parameter of function.", ERROR_LEX);
                }
                else if (c >= '1' && c <= '9')
                    {
                        //spracuj znak
                        next_state(PS_CALL_FUNCTION_PARAM_NUMBER_INT_PART;
                    }

                else if (c == '.' && (c_before != '+' || c_before != '-'))
                    next_state(PS_CALL_FUNCTION_PARAM_NUMBER_FRACTIONAL_PART);



                else if((c == '\n' || c == ' ' || c == '\t') && (c_before != '+' || c_before != '-') )
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

            }
            else if (c == '.')
            {
                //spracuj
                state = PS_CALL_FUNCTION_PARAM_NUMBER_FRACTIONAL_PART;
            }

            else if(c == '\n' || c == ' ' || c == '\t')
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
                //spracuj znak

            }
            else if (c == ')')
            {
                next_state(PS_CALL_FUNCTION_END);
            }
            else if (c == '\n' || c == ' ' || c == '\t')
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


        }

    }
}




/************************************
 * check for type keywords of language
 ************************************/
int parser_control_type(str *s){

    if (str_equals(s, "double"))
    {
        //save to token as type

        return 0;
    }
    else if(str_equals(s, "string"))
    {

        return 0;
    }
    else if(str_equals(s, "auto"))
    {

        return 0;
    }
    else if(str_equals(s, "int"))
    {

        return 0;
    }
    else
        return 1;
}


/************************************
 * check for keywords of language
 ************************************/
int parser_control_keyword(str *s){


    if (str_equals(s, "double") || str_equals(s, "string") || str_equals(s, "auto") || str_equals(s, "int") || str_equals(s, "cin") || str_equals(s, "cout") ||
            str_equals(s, "for") || str_equals(s, "if") || str_equals(s, "return") || str_equals(s, "else"))
        return 0;
    else
        return 1;
}








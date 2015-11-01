#include <stdio.h>
#include <ctype.h>
#include "str.h"
#include "error.h"

enum e_parser_state {PS_DEFAULT,
                     PS_COMMENT,
                    PS_SLASH,
                    PS_BLOCK_COMMENT,
                    PS_IDENTIFICATOR,
                    PS_INT_PART_1,
                    PS_INT_PART_2,
                    PS_FRACTIONAL_PART,
                    PS_EXPONENCIAL_PART,
                    PS_WHITESPACE,
                    PS_FIRST_WHITESPACE,
                    PS_IDENTIFICATOR_OF_DECLARATION,
                    PS_ARGUMENTS_OF_DECLARATIONS_START,
                    PS_ARGUMENTS_OF_DECLARATIONS_INSIDE};

enum e_parser_type {INT,
                    STRING,
                    DOUBLE,
                    AUTO};



int parser_next_token()
{
    //datovy typ token
    int c = NULL;
    bool type_read = false;
    e_parser_state state = PS_DEFAULT;

    int c_before = NULL;

    str *s = str_init();

    while(1)
    {
        c_before = c;
        c = getc(source);

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
            else if( c == '_' || ( c > 64 && c < 91) || ( c > 96 && c < 123) )
            {
                ungetc(c, source);
                state = PS_IDENTIFICATOR;
            }

            //start of non negative number
            else if ( c > 47 && c < 58)
                state = PS_INT_PART_1;


            break;


        case PS_SLASH:

            if ( c == '/')
                state = PS_COMMENT;
            else if ( c == '*')
                state = PS_BLOCK_COMMENT;
            else{

                // operation divide send to token

                ungetc(c, source);
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


            if ( c == '_' || ( c > 64 && c < 91) || ( c > 96 && c < 123) || ( c > 47 && c < 58) )
                state = PS_IDENTIFICATOR;

            else
            {
                    ungetc(c, source);
                    if(parser_control_type(s) == 0)
                    {
                        //We found keyword
                        //do something wit keyword
                        state = PS_FIRST_WHITESPACE;

                    }

                    //do something with identificator
                    state = PS_DEFAULT;
            }


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

                ungetc(c, source);
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
                //spracuj
                ungetc(c, soubor);
                state = PS_DEFAULT;

            }
            break;


        case PS_FRACTIONAL_PART:

            if( c > 47 && c < 58 )
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

                    TODO;
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
                ungetc(c, soubor);
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
                state = PS_IDENTIFICATOR_OF_DECLARATION;
            }
            break;

        case PS_ARGUMENTS_OF_DECLARATIONS_START:

            if (c != ' ' || c != '\t' || c != '\n' )
                state = PS_ARGUMENTS_OF_DECLARATIONS_START;
            else if( c == '(' ){
                state = PS_ARGUMENTS_OF_DECLARATIONS_INSIDE;

            }
            else if (c == ';')
            {
                //send token as declaration of variable

            }
            else if(c == '=')
            {
                ungetc(c, source);
                state = PS_DEFAULT;
                //send token as declaration of variable

            }
            else
                error("Incorrect declaration.", ERROR_LEX);

            type_read = false;
            break;


        case PS_ARGUMENTS_OF_DECLARATIONS_INSIDE:

           //TUTO SOM SKONCIL, PRACUJEM NA DEKLAROVANEJ FUNKCII

            break;

        case PS_IDENTIFICATOR_OF_DECLARATION:

            if ( c == '_' || ( c > 64 && c < 91) || ( c > 96 && c < 123) || ( c > 47 && c < 58) )
            {
                state = PS_IDENTIFICATOR_OF_DECLARATION;
                //save char in string
            }
            else if(c_before == '_' || ( c_before > 64 && c_before < 91) || ( c_before > 96 && c_before < 123) )
            {   //first time in this state, keyword of type isn't followed by identificator
                error("Keyword type without identificator of variable.");

            }
            else
            {
                ungetc(c, source);
                if(parser_control_type(s) == 0)
                {
                    //Identificator is keyword

                    error("Type keyword can't be followed by another type keyword.", ERROR_LEX);

                }
                else
                {

                    state = PS_ARGUMENTS_OF_DECLARATIONS_START;
                }

                }
            }



        }

    }


}

/************************************
 * check for keywords of language
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








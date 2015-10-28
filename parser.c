#include <stdio.h>
#include <ctype.h>
#include "str.h"
#include "error.h"

enum e_parser_state {PS_DEFAULT,
                     PS_COMMENT,
                    PS_SLASH,
                    PS_BLOCK_COMMENT,
                    PS_IDENTIFICATOR,
                    PS_INT_PART,
                    PS_FRACTIONAL_PART,
                    PS_EXPONENCIAL_PART,
                    PS_WHITESPACE,
                    PS_FIRST_WHITESPACE};

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

            //zakladny stav, zerie whitespace
            if (c == '\n' || c == ' ' || c == '\t')
                state = PS_DEFAULT;

            else if(c== '/')
                state = PS_SLASH;

            else if( c == '_' || ( c > 64 && c < 91) || ( c > 96 && c < 123) )
            {
                str_append_char(s, c);
                state = PS_IDENTIFICATOR;
            }


            else if ( c > 47 && c < 58)
                state = PS_INT_PART;


            break;


        case PS_SLASH:

            if ( c == '/')
                state = PS_COMMENT;
            else if ( c == '*')
                state = PS_BLOCK_COMMENT;
            else{

                // do tokenu poslem operator delenie

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
                if (c_before == '*')
                    state = PS_DEFAULT;

            break;

        case PS_IDENTIFICATOR:

            if ( c == '_' || ( c > 64 && c < 91) || ( c > 96 && c < 123) || ( c > 47 && c < 58) )
                state = PS_IDENTIFICATOR;

            else
            {
                    ungetc(c, source);
                    if(parser_control_type(s) != 0)
                    {
                        //send token
                        state = PS_DEFAULT;
                    }
                    else
                    {
                        if (type_read == true)
                        {
                            error("Keyword can't be followed by another keyword.");

                        }
                        type_read = true;
                        state = PS_FIRST_WHITESPACE;
                    }
            }


            break;

        case PS_INT_PART:

            if(c_before == '0' && c != '.')
                //chyba

            if (c == '.')
                state = PS_FRACTIONAL_PART;
            else if (c > 47 && c < 58)
                state = PS_INT_PART;


            else{

                ungetc(c, source);
            }

        case PS_FRACTIONAL_PART:

            if( c > 47 && c < 58 )
                //pokracujem v citani znakov
                state = PS_FRACTIONAL_PART;

            else if (c == 'E' || c == 'e'){
                TODO;
                state = PS_EXPONENCIAL_PART;
            }
            else if (c == '.')
                //ERROR



            break;

        case PS_EXPONENCIAL_PART:

            //iam in this state for the first time
            if (c_before == 'E' || c_before == 'e' )
            {
                if(c == '+' || c == '-')

                    TODO;

                else if(c > 47 && c < 58)

                    TODO;
                else if(c == '.')
                    //ERROR
                    TODO;
            }

            else if(c > 47 && c < 58){

                TODO;
            }

            else if(c == '.')
                //ERROR
                TODO;


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

        case PS_WHITESPACE:

            if(c == '\n' || c == ' ' || c == '\t')
                state = PS_WHITESPACE;

            else
            {
                state = PS_IDENTIFICATOR;
            }



        }

    }


}

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








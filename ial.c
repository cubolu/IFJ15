
#include "ial.h"
#include "str.h"

int length(str_t * s)
{
    return s->length;
}

str_t * substr(str_t * s, int i, int n)
{
    if (i > s->length)
        error("Parameters of function substr: Out of range.", ERROR_RUNTIME);

    str_t * ret = str_init();

    if (n+i > s->length)
        n = s->length-i;

    _str_resize(ret, n+1);
    memcpy(ret->c_str, s->c_str+i, n);
    ret->c_str[n] = '\0';
    ret->length = n;

    return ret;
}

str_t * concat(str_t * s1, str_t * s2)
{
    str_t * ret = str_init();

    _str_resize(ret, s1->length+s2->length+1);

    memcpy(ret->c_str           , s1->c_str, s1->length);
    memcpy(ret->c_str+s1->length, s2->c_str, s2->length);

    ret->c_str[s1->length+s2->length] = '\0';
    ret->length = s1->length + s2->length;

    return ret;
}

//Boyer-Moore with Bad Character Rule (could be imporved by adding Good Suffix Rule)
int find(str_t * s, str_t * search)
{
    //Empty string is at position 0 in any string
    if (search->length == 0)
        return 0;

    //Bad match table
    int bmt[256];

    int pattlen = search->length;

    //Fill BMT with default shift (length of pattern)
    for(int i=0; i< 256; i++)
    {
        bmt[i] = pattlen;
    }

    //Make match table - omit last character, it's lenth of pattern if not already defined lower
    for(int i=0; i < pattlen-1; i++)
    {
        unsigned char c = search->c_str[i];

        bmt[c] = pattlen - i - 1;
    }



    //Go through the string
    int i = pattlen-1;

    while (i < s->length)
    {
        int j;
        for(j = 0; j < pattlen; j++)
        {
            if ( s->c_str[i-j] != search->c_str[pattlen-j-1] )
                break;
        }

        //Matched
        if (j == pattlen)
            return i-pattlen+1;


        //Shift index
        i += bmt[ (unsigned char) s->c_str[i] ];
    }


    //Didn't find
    return -1;
}

void _swap(char array[], int left, int right)
{
    int tmp = array[right];
    array[right] = array[left];
    array[left] = tmp;
}


void _quicksort(char array[], int left, int right)
{
    if(left < right)
    {
        int boundary = left;

        for(int i = left + 1; i < right; i++)
        {
            if(array[i] < array[left])
                _swap(array, i, ++boundary);
        }

        _swap(array, left, boundary);

        _quicksort(array, left, boundary);

        _quicksort(array, boundary + 1, right);
    }
}

//Quicksort
str_t * sort(str_t * orig)
{
    str_t *s = str_init();

    str_copy(s, orig->c_str);

    _quicksort(s->c_str, 0, s->length);

    return s;
}


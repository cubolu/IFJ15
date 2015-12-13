
#include "ial.h"
#include "str.h"

int length(str_t * s)
{
    return s->length;
}

str_t * substr(str_t * s, int i, int n)
{
    if ( (unsigned int) i > s->length || n < 0)
        error("Parameters of function substr: Out of range.", ERROR_RUNTIME);

    str_t * ret = str_init();

    if ( (size_t) n+i > s->length)
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
    size_t i = pattlen-1;

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

void _quicksort(char array[], int left_begin, int right_begin)
{
    char pivot = array[(left_begin + right_begin) / 2];
    char tmp;
    int left_index, right_index;
    left_index = left_begin;
    right_index = right_begin;

    do {
        while (array[left_index] < pivot && left_index < right_begin)
            left_index++;
        while (array[right_index] > pivot && right_index > left_begin)
            right_index--;

        if (left_index <= right_index) {
            tmp = array[left_index];
            array[left_index++] = array[right_index];
            array[right_index--] = tmp;
        }
    } while (left_index < right_index);

    if (right_index > left_begin) _quicksort(array, left_begin, right_index);
    if (left_index < right_begin) _quicksort(array, left_index, right_begin);
}

//Quicksort
str_t * sort(str_t * orig)
{
    str_t *s = str_init();

    str_copy(s, orig->c_str);

    _quicksort(s->c_str, 0, s->length-1);

    return s;
}


/*
   scanner.c

   test scanner with all defined keywords
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
	static struct keyword {
	    char *word;
	    int code;
	} keywords[] = {
	    { "like",		1000 },
	    ...
	    { 0, 0 }
*/
#include "../tools/keywords.h"

extern int matchword (const char *word);

int
main (int argc, char **argv)
{
    int i;

    if (argc < 2)
    {
	i = 0;
	while (keywords[i].word != 0)
	{
	    int code = matchword (keywords[i].word);
	    if (code != keywords[i].code)
	    {
		printf ("%s: got %d, expect %d\n", keywords[i].word, code, keywords[i].code);
		return -1;
	    }
	    i++;
	}
	return 0;
    }

    for (i = 1; i < argc; i++)
	printf ("%d ", matchword (argv[i]));
    printf ("\n");

    return 0;
}

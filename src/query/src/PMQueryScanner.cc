/*
   PMQueryScanner.cc

   match keywords by traversing table

   returns id-code (see tools/keywords.h file) if word matches
   else returns -1

   table : list of (short match, short next-index) pairs, chained
	   by next-index
   First value is character (if > 0), or final result (if < 0)
     if (value & 0x80) several matches are possible and consecutive
     in table
     if a minimal non-ambiguous prefix is found, (value & 0x100)
     is set (no more alternatives in remaining chain)
   Second value is list index to jump to if character matches 

   see tools/maketable.c for details
*/

#include <string.h>
#include <ctype.h>

#include "lookuptable.h"


/*
  find match for keyword in lookuptable
  @param word	pointer to word
  @returns id-code if match, -1 else

  tries to match a minimal prefix of word in lookup table.
  take all letter characters of word into account. if we have
  a non-ambiguous minimal prefix at the first non-letter
  character (or \0) of word, it is a match.
 */

int
matchword (const char *word)
{
    short match = *word;
    int ret = -1;
    int idx;

    if (isupper (match))		// case-insensitive
	match = tolower (match);

    if (!islower (match))		// must start with letter
	return -1;

    idx = (match - 'a') * 2;		// start index in lookup table

    for (;;)
    {
	short expect;

	expect = lookuptable[idx];	// get character code from table

	if (expect < 0)				// complete match
	{
	    if (!islower (match))		// end of word reached
		ret = -expect;
	    break;
	}
	else if (match == (expect & 0x7f))	// character match
	{
	    idx = lookuptable[idx+1];		// get next index (linked list)
	    word++;
	    match = *word;
	    if (isupper (match))
		match = tolower (match);
	}
	else if ((expect & 0x80) != 0)		// try next
	{
	    idx += 2;				// try next match in table
	}
	else if ((expect & 0x100) != 0)		// minimal prefix found
	{					// match rest of word
	    if (islower (match))		// word does not match
	    {
		ret = -1;
		break;
	    }
	    idx = lookuptable[idx+1];		// go on in table
	}
	else					// no match
	{
	    break;
	}
    }

    return ret;
}

/*
   maketable.c

   generate lookup table for query keywords

   Strategy

   Create tree-like structure with information about
   all keywords
   The nodes contain

   - pointer to 'next' node
   - character to expect
   - pointer to 'match' node

   A node is reached after a matches character is found.
   For the initial characters A..Z (upper case for readability only,
   keywords are not cases sensitive), no nodes are created.

   Each node represents a specific character position (from the
   start of the keyword). A node is reached after matching a character.
   Then the next character should match the expected character
   stored in the node. If it does, the 'match' node is reached.
   If it doesn't, 'next' points to a (linked) list of alternative
   matches (nodes).

   Example: Keywords ABC and ACD
   For 'A', no node is created since the initial character has fixed
   start nodes instead of a linked list of alternative matches.
   The first node in the 'A' branch represents the second character ('B')
   and has a link to the alternative second character ('C')
   The 'B' node only expects 'C' (at the third position), the 'C' node
   only expects 'D'.

   The node additionally contain a 'pos' field used later to lay
   out all node information in a flat (1-dim) list.

   Each keyword has an integer value associated for later reference.
   The final match nodes point to a further node containing this
   integer value (stored as a negative 'expect' value).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "keywords.h"

/*
  character node definition

 */
struct node {
    struct node *next;		// next try if no match
    int expect;			// character to expect (>0), final code (<0)
    int pos;			// used in 'flatten' algorithm later
    struct node *match;		// next character node if match
};

/* lower-case character  */
#define NORMALIZE(c) do { if (isupper (c)) c = tolower (c); } while (0)


/*
   fill_node (struct node *prev, char *word, int code)

   recursively fill node structures for whole word

   prev points to the expect node for the character at *word (!)
   so we create here either the node to be reached when the
   expected character is found
   or the node for an alternative character

   Initial nodes for the second position of a keyword don't
   need to be created, they're already existant in a fixed table
*/

static int
fill_node (struct node *prev, char *word, int code)
{
    int nextchar;
//fprintf (stderr, "fill_node ('%c', '%c', %d)\n", (prev?prev->expect:'.'), *word, code);
    /*
       create new node for the _next_ character
     */
    struct node *current = (struct node *)malloc (sizeof (struct node));
    if (current == 0)
    {
	fprintf (stderr, "OOM\n");
	abort ();
    }
    memset (current, 0, sizeof (struct node));

    nextchar = *word;
    NORMALIZE (nextchar);

    if (*word == 0)				// end-of-word
    {
//fprintf (stderr, "Final node\n");
	free (current);
	prev->expect = -code;			// final node has negative keyword value in expect
    }
    else if (prev->expect == 0)			// initial node, prev == table
    {
//fprintf (stderr, "Initial node\n");
	prev->expect = nextchar;		// create linked 'match' nodes
	prev->match = current;
	fill_node (current, word+1, code);	// for all characters of word
    }
    else if (prev->expect == nextchar)
    {
//fprintf (stderr, "Match node\n");
	free (current);
	fill_node (prev->match, word+1, code);	// match !
    }
    else		// prev->expect != nextchar
    {
//fprintf (stderr, "Next node\n");
	while (prev->next != 0)			// find end of 'next' chain
	{
	    prev = prev->next;
	    if (prev->expect == nextchar)
	    {
		free (current);
		fill_node (prev, word, code);
		return 0;
	    }
	}
	prev->next = current;			// link in new alternative
	fill_node (current, word, code);	// next !
    }
    return 0;
}


static void
print_node (struct node *node, char *prefix)
{
    if (node->expect < 0)
    {
	printf ("%d !\n", -node->expect);
    }
    else
    {
	if (node->match == 0)
	{
	    printf ("\n");
	    fprintf (stderr, "no match!\n");
	    return;
	}
	if (node->next != 0)
	{
    	    print_node (node->next, "+ ");
	}
	printf ("%s[%c] ", prefix, node->expect);
	print_node (node->match, "");
    }
    return;
}


/*
   collapse link of "match" nodes to single node

   -> only save minimal prefix for each keyword

 */

static int
collapse_node (struct node *node)
{
    int ret = 0;
//printf ("collapse ('%c'/%d -> %p, %p)\n", node->expect, node->expect, node->match, node->next);
    if (node->expect < 0)
    {
	return node->expect;
    }

    ret = collapse_node (node->match);		// pure match linke ?
    if ((ret < 0)
	&& (node->next == 0))
    {
//printf ("pure ('%c'/%d)!\n", node->expect, node->expect);
	node->expect |= 0x100;			// collapse !
    }
    else
    {
	while (node->next)
	{
	    collapse_node (node->next->match);
	    node = node->next;
	}
	ret = 0;
    }

    return ret;
}

/*
  position next/match nodes of 'node'
  idx == next free slot

*/

static int
position_node (struct node *node, int idx, int pass, FILE *output)
{
    struct node *start = node;
//fprintf (stderr, "position_node(%c(%d)@%d[%p/%p], %d)\n", node->expect, node->expect, node->pos, node->match, node->next, idx);
    if (pass == 1)
    {
#if 0
	// 'this' comes first
	if (node->expect > 0)
	    printf ("! [%c @ %d]\n", node->expect, idx);
	else
	    printf ("! [%d @ %d]\n", node->expect, idx);
#endif
    }
    else	// pass != 1
    {
	if (node->pos != idx)
	{
	    fprintf (stderr, "\n**** Bad node pos (%d <-> %d)\n", node->pos, idx);
	    return -1;
	}
	if (output != 0)
	{
	    if (node->match)
		fprintf (output, ", '%c'%s, %d", node->expect & 0x7f, (node->next?"+128":((node->expect & 0x100)?"+0x100":"")), node->match->pos * 2);
	    else
		fprintf (output, ", %d, 0\n// -- %d --\n", node->expect, idx*2 + 2);
	}
    }

    // position 'next' nodes together

    while (node->next != 0)
    {
	node = node->next;
	idx++;
	if (pass == 1)
	{
#if 0
	    if (node->expect > 0)
		printf ("+ [%c @ %d]\n", node->expect, idx);
	    else
		printf ("+ [%d @ %d]\n", node->expect, idx);
#endif
	    node->pos = idx;
	}
	else	// pass != 1
	{
	    if (node->pos != idx)
	    {
		fprintf (stderr, "\n**** Bad next pos ('%c' %d <-> %d)\n", node->expect, node->pos, idx);
		return -1;
	    }
	    if (output != 0)
		fprintf (output, ", '%c'%s, %d", node->expect & 0x7f, (node->next?"+128":((node->expect & 0x100)?"+0x100":"")), node->match->pos * 2);
	}
    }

    // now re-start and descend the tree

    node = start;

    // position match if present (else we're at end of string)

    idx++;			// either for match or for final code
    if (node->match)
    {
//fprintf (stderr, "position match '%c'\n", node->expect);
	node = node->match;
	node->pos = idx;
	idx = position_node (node, idx, pass, output);
    }
    
    // positions all nodes 'behind' next
    node = start;

    while (node->next != 0)
    {
	node = node->next;
//fprintf (stderr, "position next '%c'\n", node->expect);
	if (pass == 1)
	    node->match->pos = idx;
	idx = position_node (node->match, idx, pass, output);
	if (idx < 0)
	    break;
    }

    return idx;
}

static void
usage (char *name)
{
    fprintf (stderr, "Usage: %s [-o <output>]\n", name);
    return;
}


int
main (int argc, char **argv)
{
    struct keyword *kptr;

#define TABLESIZE ('Z'-'A'+1)
    struct node table[TABLESIZE];

    int i, j;

    FILE *output = 0;

    if (argc > 1)
    {
	i = 1;
	while (i < argc)
	{
	    if (strcmp (argv[i], "-o") == 0)
	    {
		i++;
		if (i >= argc)
		{
		    usage (argv[0]);
		    exit (1);
		}
		output = fopen (argv[i], "w+");
		if (output == 0)
		{
		    fprintf (stderr, "Can't open %s for writing\n", argv[i]);
		    exit (1);
		}
	    }
	    if (strcmp (argv[i], "-h") == 0)
	    {
		usage (argv[0]);
		exit (0);
	    }
	    i++;
	}
    }

    // FIXME: check keywords for isalpha() and code > 0

    // initialize table

    memset (table, 0, sizeof (struct node) * TABLESIZE);

    // loop over all initial table slots (a..z)

    for (i = 0; i < TABLESIZE; i++)
    {
	// loop over all keywords, finding matching first characters
	// for current slot

	kptr = keywords;
	while (kptr->word != 0)
	{
	    char *word = kptr->word;
	    int firstchar = *word;
	    NORMALIZE (firstchar);

	    // match current table slot 
	    if ((firstchar - 'a') == i)
	    {
		fill_node (&(table[i]), word+1, kptr->code);
	    }

	    kptr++;
	}
    }

    // collapse tree to minimal prefixes

    for (i = 0; i < TABLESIZE; i++)
    {
	if (table[i].expect != 0)
	{
	    collapse_node (&table[i]);
	}
    }

    // print tree
    if (output == 0)
    {
    for (i = 0; i < TABLESIZE; i++)
    {
	if (table[i].expect != 0)
	{
	    printf ("[%c] ", i + 'A');
	    print_node (&table[i], "");
	}
    }
    }

    // generate table
    j = TABLESIZE;
    for (i = 0; i < TABLESIZE; i++)
    {
	if (table[i].expect != 0)
	{
	    table[i].pos = j;			// initial start pos
	    j = position_node (&(table[i]), j, 1, output);
	}
	if (j < 0)
	    break;
    }

    if (output != 0)
    {
	fprintf (output, "short lookuptable[%d] = {\n", j*2);
	for (i = 0; i < TABLESIZE; i++)
	{
	    if (i > 0)
		fprintf (output, ", ");
	    if (table[i].expect != 0)
	    {
		fprintf (output, "'%c', %d", i+'a', table[i].pos * 2);
	    }
	    else
	    {
		fprintf (output, "0, 0");
	    }
	}

	j = TABLESIZE;
	fprintf (output, "\n//--- %d ---\n", j * 2);
	for (i = 0; i < TABLESIZE; i++)
	{
	    if (table[i].expect != 0)
		j = position_node (&(table[i]), j, 2, output);
	    if (j < 0)
		break;
	}
	fprintf (output, "};\n");

	fclose (output);
    }

    return 0;
}

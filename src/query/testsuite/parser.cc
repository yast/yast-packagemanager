/*
    test_query.cc

    test frontend for query interface

    Author: kkaempf@suse.de
    Maintainer: kkaempf@suse.de

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <y2pm/Query.h>

extern struct qnode *parse_query (char **query, QueryError *error);

static void print_node (struct qnode *node);

static char *
attr2string (int attr)
{
#include "../tools/keywords.h"

    int i = 0;

    while (keywords[i].code != 0)
    {
	if (keywords[i].code == attr)
	    return keywords[i].word;
	i++;
    }
    return "?key?";
}

static char *
op2string (enum operation op)
{
    switch (op & ~OP_ATOMIC)
    {
	case OP_AND:
	return "&&";
	break;
	case OP_OR:
	return "||";
	break;
	case OP_NOT:
	return "!";
	break;
	case OP_EQ:
	return "==";
	break;
	case OP_NEQ:
	return "!=";
	break;
	case OP_GT:
	return ">";
	break;
	case OP_GE:
	return ">=";
	break;
	case OP_LT:
	return "<";
	break;
	case OP_LE:
	return "<=";
	break;
	case OP_CONTAINS:
	return "contains";
	break;
	case OP_LIKE:
	return "like";
	break;
	case OP_CONST:
	case OP_FLAG:
	return "";
	break;
	default:
	break;
    }
    fprintf (stderr, "Unknown op %0x\n", (int)op);
    return "?op?";
}



static void
print_branch (struct qvalue *branch)
{
    switch (branch->type)
    {
	case QTYPE_FLAG:
	case QTYPE_ATTR:
	    printf ("<%d>%s", branch->v.attr, attr2string(branch->v.attr));
	break;
	case QTYPE_CONST:
	    printf ("\"%s\"", branch->v.constant);
	break;
	case QTYPE_NODE:
	    print_node (branch->v.node);
	break;
	default:
	    printf ("branch->type (%d) ??\n", branch->type);
	break;
    }
    return;
}

static void
print_node (struct qnode *node)
{
    enum operation op;

    if (node == 0)
    {
	printf ("<nil>");
	return;
    }

    printf ("[");
    op = (enum operation)(node->op & ~OP_ATOMIC);
    if (op == OP_NOT)
    {
	printf ("!");
    }
    if (node->op & OP_ATOMIC) printf ("(");
    print_branch (&(node->left));
    if (op > OP_UNARY)
    {
	printf (" %s ", op2string (op));
	print_branch (&(node->right));
    }
    if (node->op & OP_ATOMIC) printf (")");
    printf ("]");
    return;
}


static void
do_query (const std::string query)
{
    PMError error;
    QueryParser parser;
    struct qnode *node = 0;
    int errpos;
    error = parser.parseQuery (query, errpos, &node);
    if (node != 0)
    {
	if ((node->op & ~OP_ATOMIC) == OP_CONST)
	{
	    printf ("*** Not a query:\n");
	}
	print_node (node);
    }
    else
    {
	int j;
//	printf ("*** %s:\n", error.errstr().c_str());
	printf ("*** Fail\n");
	printf ("%s\n", query.c_str());
	for (j = 0; j < errpos; j++)
	{
	    printf (" ");
	}
	printf ("^");
    }
    printf ("\n");
    return;
}

int
main (int argc, char **argv)
{
    if (argc < 2)
    {
#define BUFLEN 1024
	static char buf[BUFLEN];
	while (fgets (buf, BUFLEN-1, stdin) == buf)
	    do_query (buf);
    }
    else
    {
	int i;

	for (i = 1; i < argc; i++)
	{
	    do_query (argv[i]);
	}
    }
    return 0;
}


/*
   QueryParseer.cc

   parse query

   Author: kkaempf@suse.de
   Maintainer: kkaempf@suse.de
*/

#ifdef PARSER_DEBUG
#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <y2pm/QueryParser.h>

extern int matchword (const char *word);

/*
  free node structure

 */
void
QueryParser::free_node (struct qnode *node)
{
    if (node->left.type == QTYPE_NODE)
	free_node (node->left.v.node);
    if (node->right.type == QTYPE_NODE)
	free_node (node->right.v.node);

#ifdef PARSER_DEBUG
fprintf (stderr, "free(%p)\n", node);
#endif
    free (node);

    return;
}

/*
  return binding priority for operation

  && binds higher than ||
  comparison op binds higher than boolean op

 */
int
QueryParser::op_binding (enum operation op)
{
    switch (op)
    {
//	case OP_NOT:		// highest prio
//	    return 4;
//	break;
	case OP_AND:		// middle prio
	    return 2;
	break;
	case OP_OR:		// low prio
	    return 1;
	break;
	default:
	break;
    }
    return 3;			// all others: higher prio
}

/*
   parse operatation at *query

   return operation code
*/

enum operation
QueryParser::parse_op (const char **query)
{
    char qchar;
#ifdef PARSER_DEBUG
fprintf (stderr, "parse_op(%s)", *query);
#endif

    while (isspace (**query))
	(*query)++;

    qchar = **query;
    (*query)++;
    switch (qchar)
    {
	case '!':
	{
	   if (**query == '=')
	   {
		(*query)++;
		return OP_NEQ;
	   }
	}
	break;
	case '=':
	{
	   if (**query == '=')
	   {
		(*query)++;
		return OP_EQ;
	   }
	}
	break;
	case '>':
	{
	   if (**query == '=')
	   {
		(*query)++;
		return OP_GE;
	   }
	   else
	   {
		return OP_GT;
	   }
	}
	break;
	case '<':
	{
	   if (**query == '=')
	   {
		(*query)++;
		return OP_LE;
	   }
	   else
	   {
		return OP_LT;
	   }
	}
	break;
	case '&':
	{
	    if (**query == '&')
	    {
		(*query)++;
		return OP_AND;
	    }
	}
	break;
	case '|':
	{
	    if (**query == '|')
	    {
		(*query)++;
		return OP_OR;
	    }
	}
	break;
	default:
	{
	    if (isalpha (qchar))
	    {
		int word = matchword (*query - 1);
		if ((word == 1000)
		    || (word == 1001))
		{
		    while (isalpha (**query))
			(*query)++;
		    return (word==1000)?OP_LIKE:OP_CONTAINS;
		}
	    }
	}
	break;
    }
    (*query)--;
    return OP_NONE;     
}


/*
  check op against value

  disallow compare ops with flags
  disallow boolean ops with values

  return QueryError::E_ok if ok
  return QueryError::E_compare_flag if compare op with flag
  return QueryError::E_andor_value if boolean op with value
 */

PMError
QueryParser::check_node (struct qnode *node)
{
    enum operation op = (enum operation)(node->op & ~OP_ATOMIC);
#ifdef PARSER_DEBUG
fprintf (stderr, "check_node(%d, %d, %d)\n", (int)node->left.type, (int)node->op, (int)node->right.type);
#endif
    if (op < OP_UNARY)		// unary op
    {
	if ((node->right.type != QTYPE_NONE)
	    || (node->left.type != QTYPE_FLAG))
	{
	    return QueryError::E_bad_expr;
	}
    }
    else if (op < OP_BOOLEAN)	// boolean op
    {
	if ((node->left.type == QTYPE_CONST)
	    || (node->right.type == QTYPE_CONST)
	    || (node->left.type == QTYPE_ATTR)
	    || (node->right.type == QTYPE_ATTR))
	{
	    return QueryError::E_andor_const;
	}
    }
    else			// compare op
    {
	if ((node->left.type != QTYPE_CONST)
	    && (node->right.type != QTYPE_CONST)
	    && (node->left.type != QTYPE_ATTR)
	    && (node->right.type != QTYPE_ATTR))
	{
	    return QueryError::E_compare_flag;
	}
    }
    return QueryError::E_ok;
}

/*
  parse value at *query

  return
  0: ok, value points to filled qvalue, *query to next char
  -1: error, value is untouched, *query points to error

  A value is
  - an expression in parantheses
  - '!' + expression
  - a string constant
  - a keyword

 */

int
QueryParser::parse_value (struct qvalue *value, const char **query, PMError *error)
{
    while (isspace (**query))
	(*query)++;
#ifdef PARSER_DEBUG
fprintf (stderr, "parse_value(%s)\n", *query);
#endif

    if (**query == '(')
    {
	struct qnode *node;
	(*query)++;
	node = parse_expr (query, error);
	if (node == 0)
	{
	    return -1;
	}
	if (**query != ')')
	{
	    *error = QueryError::E_open_paranthesis;
	    return -1;
	}
	(*query)++;

	node->op = (enum operation)(node->op | OP_ATOMIC);
	value->type = QTYPE_NODE;
	value->v.node = node;

	return 0;
    }
    else if (**query == '!')
    {
	struct qnode *not_node;
	const char *start;

	(*query)++;
	while (isspace (**query))
	    (*query)++;
	not_node = (struct qnode *)malloc (sizeof (struct qnode));
	if (not_node == 0)
	{
	    *error = QueryError::E_memory;
	    return -1;
	}
	memset (not_node, 0, sizeof (struct qnode));
#ifdef PARSER_DEBUG
fprintf (stderr, "not_node at %p\n", not_node);
#endif
	not_node->op = OP_NOT;
	start = *query;
	if (parse_value (&(not_node->left), query, error) != 0)
	{
	    free_node (not_node);
	    return -1;
	}
	*error = check_node (not_node);
	if (*error != QueryError::E_ok)
	{
	    free_node (not_node);
	    return -1;
	}

	value->type = QTYPE_NODE;
	value->v.node = not_node;

	return 0;
    }
    else if (**query == '"')		// string constant
    {
	const char *qptr;
	(*query)++;
	qptr = *query;
	while (**query)
	{
	    if (**query == '"')
	    {
		int len = *query - qptr;
		(*query)++;
		value->type = QTYPE_CONST;
		value->v.constant = (char *)malloc (len + 1);
		memcpy (value->v.constant, qptr, len);
		value->v.constant[len] = 0;
		return 0;
	    }
	    else if (**query == '\\')
	    {
		(*query)++;
	    }
	    (*query)++;
	}
	*error = QueryError::E_unterminated_string;
	*query = qptr;
	return -1;
    }
    else if (isalpha (**query))
    {
	int word = matchword (*query);
	if (word > 0)
	{
#ifdef PARSER_DEBUG
fprintf (stderr, "%s -> %d\n", *query, word);
#endif
	    if (word < QKEY_MAX_FLAG)
		value->type = QTYPE_FLAG;
	    else if (word < QKEY_MAX_VALUE)
		value->type = QTYPE_ATTR;
	    else
	    {
		*error = QueryError::E_bad_value;
		return -1;
	    }
	    value->v.attr = word;
	    while (isalpha (**query))
		(*query)++;
	}
	else
	{
	    const char *qptr = *query;
	    int len;

	    while (isalpha (**query))
		(*query)++;
	    len = *query - qptr;
	    value->type = QTYPE_CONST;
	    value->v.constant = (char *)malloc (len + 1);
	    memcpy (value->v.constant, qptr, len);
	    value->v.constant[len] = 0;
	}
	return 0;
    }
    *error = QueryError::E_unknown_operand;
    return -1;
}


/*
  check for node expression at *query

  return node
 */

struct qnode *
QueryParser::parse_expr (const char **query, PMError *error)
{
    struct qnode *node;

#ifdef PARSER_DEBUG
fprintf (stderr, "parse_expr (%s)\n", *query);
#endif

    node = (struct qnode *)malloc (sizeof (struct qnode));
    if (node == 0)
    {
	*error = QueryError::E_memory;
	return 0;
    }
    memset (node, 0, sizeof (struct qnode));
#ifdef PARSER_DEBUG
fprintf (stderr, "node at %p\n", node);
#endif

    while (isspace (**query))
	(*query)++;

    if (parse_value (&(node->left), query, error) != 0)
    {
	free_node (node);
	return 0;
    }

    // node->left == first operand

    for (;;)
    {
	struct qnode *rnode;
	const char *start;

	while (isspace (**query))
	    (*query)++;

	/*
	   end of query ?
	 */

	if ((**query == ')')
	    || (**query == 0))
	{
	    switch (node->left.type)
	    {
		case QTYPE_FLAG:
		    node->op = OP_FLAG;
		break;
		case QTYPE_CONST:
		case QTYPE_ATTR:
		    node->op = OP_CONST;
		break;
		case QTYPE_NODE:
		{
		    struct qnode *obsolete = node;
		    node = node->left.v.node;
		    obsolete->left.type = QTYPE_NONE;	// invalidate left side
#ifdef PARSER_DEBUG
fprintf (stderr, "obsolete(%p), node(%p)\n", obsolete, node);
#endif
		    free_node (obsolete);
		}
		break;
		default:
		    *error = QueryError::E_no_op;
		    free_node (node);
		    node = 0;
		break;
	    }
	    break;			// break for(;;)
	}

	/* check for operation  */

	start = *query;
	node->op = parse_op (query);
	if (node->op == OP_NONE)
	{
	    *error = QueryError::E_unknown_operation;
	    *query = start;
	    free_node (node);
	    node = 0;
	    break;
	}

	// ** recursion **

	start = *query;    		// remember start for check_node later
	rnode = parse_expr (query, error);
	if (rnode == 0)
	{
	    free_node (node);
	    node = 0;
	    break;
	}

	// final value/expr found

	if ((rnode->op == OP_FLAG)	// only left value filled
	    || (rnode->op == OP_CONST))
	{
#ifdef PARSER_DEBUG
fprintf (stderr, "only left\n");
#endif
	    memcpy (&(node->right), &(rnode->left), sizeof (struct qvalue));
	    free_node (rnode);
	}
	else
	{
	    node->right.type = QTYPE_NODE;
	    node->right.v.node = rnode;
	}

	// does the next operand match the operation ?
	*error = check_node (node);
	if (*error != QueryError::E_ok)
	{
	    *query = start;
	    free_node (node);
	    node = 0;
	    break;
	}

	// check for [left: x] op1 [right: y op2 z]
	// convert to [left: x] op1 [right:y op2|ATOMIC z]
	//   if op2 binds higher than op1

	if ((node->right.type == QTYPE_NODE)
	    && (op_binding (node->right.v.node->op) < op_binding (node->op)))
	{
	    struct qnode *lower_node;
	    lower_node = node->right.v.node;

#ifdef PARSER_DEBUG
fprintf (stderr, "switch binding (%s)\n", *query);
#endif
	    // lower_node == (x <low_op> y)
	    // node == (lower_node <high_op> z)

	    // change node to (y <high_op> z)
	    memcpy (&(node->right), &(lower_node->left), sizeof (struct qvalue));

	    // change or_node to (x <low_op> (y <high_op> z))
	    lower_node->left.type = QTYPE_NODE;
	    lower_node->left.v.node = node;

	    // lower_node is top of tree
	    node = lower_node;
	}

#ifdef PARSER_DEBUG
fprintf (stderr, "final (%s)\n", *query);
#endif
	if (**query == 0)
	    break;
	if (**query != ')')
	{
	    *error = QueryError::E_syntax;
	    free_node (node);
	    node = 0;
	}
	break;
    }
#ifdef PARSER_DEBUG
fprintf (stderr, "parse_expr => (%d, %d, %d)\n", (int)node->left.type, (int)node->op, (int)node->right.type);
#endif
    return node;
}

//-------------------------------------------------------------------
// public

PMError
QueryParser::parseQuery (const std::string& query, int& errpos, struct qnode **node)
{
    if (node == 0)
	return QueryError::E_error;
    PMError err;
    const char *cquery = query.c_str();
    *node = parse_expr (&cquery, &err);
    if (err != QueryError::E_ok)
    {
	errpos = cquery - query.c_str();
    }
    return err;
}

/*

   QueryNode.h

   data type definitions for query handling

   Author: kkaempf@suse.de
   Maintainer: kkaempf@suse.de
*/

#ifndef QueryNODE_H
#define QueryNODE_H
/*
   define maximum id values for query keywords
   see tools/keyword.h
 */
#define QKEY_MAX_FLAG	   99		// bool attr keyword
#define QKEY_MAX_VALUE    999		// value attr keyword
#define QKEY_MAX_COMPARE 9999		// compare keyword

/*
  node operations

 */

enum operation {
    OP_NONE = 0,			// undefined operation
    OP_CONST,				//1 unary operation -> constant
    OP_FLAG,				//2 unary operation -> flag
    OP_NOT,				//3 unary operation -> boolean not
    OP_UNARY,			// -- separator --
    OP_AND, OP_OR,			//5,6 boolean
    OP_BOOLEAN,			// -- separator --
    OP_EQ, OP_NEQ,			//8,9 equality
    OP_GT, OP_GE, OP_LT, OP_LE,		//10..13 greater, lesser
    OP_CONTAINS, OP_LIKE		//14,15 string list compare
};

// flag to indicate paranthesized nodes
#define OP_ATOMIC 0x80

/*
  type of query value

 */
enum qvaltype {
    QTYPE_NONE=0,		// unknown
    QTYPE_FLAG,			//1 bool attribute
    QTYPE_ATTR,			//2 constant attribute
    QTYPE_CONST,		//3 constant value
    QTYPE_NODE			//4 query node
};

struct qnode;

/*
  value of node
  used to define left or right side of a node

 */
struct qvalue {
    enum qvaltype type;
    union {
        int attr;			// QTYPE_ATTR
        char *constant;			// QTYPE_CONST
        struct qnode *node;		// QTYPE_NODE
    } v;
};

/*
  query node
  binds two values by an operation

 */

struct qnode {
    enum operation op;                  // operation
    struct qvalue left;
    struct qvalue right;
};

#endif // QueryNODE_H

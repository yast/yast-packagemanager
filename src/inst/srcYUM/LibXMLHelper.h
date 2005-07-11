#ifndef LibXMLHelper_h
#define LibXMLHelper_h
#include <libxml/tree.h>
#include <string>

/**
 * @short Easy access to xmlNodes for C++
 */

class LibXMLHelper {
public:
  /**
   * The default constructor will return an object that does not
   * look into the namespace properties of the nodes. Later, another
   * constructor will be added that takes a list of namespaces as parameters
   * (and maybe also character encoding information), and all nodes of different
   * namespaces will be ignored (i.e., attributes will not be used, and for elements
   * in different namespaces isElement() will return false).
   */
  LibXMLHelper();
  
  /**
   * Destructor
   */
  virtual ~LibXMLHelper();
  
  /**
   * Fetch an attribute
   * @param node the xmlNode
   * @param name name of the attribute
   * @param defaultValue the value to return if this attribute does not exist
   * @return the value of the attribute
   */
  std::string attribute(const xmlNodePtr node, 
                        const std::string &name, 
                        const std::string &defaultValue = std::string()) const;
  
  /**
   * @short The TEXT content of the node and all child nodes
   * Read the value of a node, this can be either the text carried directly by this node if
   * it's a TEXT node or the aggregate string of the values carried by this node child's
   * (TEXT and ENTITY_REF). Entity references are substituted.
   * @param nodePtr the xmlNode
   * @return the content
   */
  std::string content(const xmlNodePtr nodePtr) const;
  
  /**
   * The name of the node
   * @param nodePtr the xmlNode
   * @return the name
   */
  std::string name(const xmlNodePtr nodePtr) const;
  
  /**
   * returns whether this is an element node (and not, e.g., a attribute or namespace node)
   * @param nodePtr the xmlNode
   * @return true if it is an element node
   */
  bool isElement(const xmlNodePtr nodePtr) const;
};

#endif

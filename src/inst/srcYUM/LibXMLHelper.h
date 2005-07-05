#ifndef LibXMLHelper_h
#define LibXMLHelper_h
#include <libxml/tree.h>
#include <string>

class LibXMLHelper {
public:
  LibXMLHelper();
  virtual ~LibXMLHelper();
  std::string attribute(const xmlNodePtr node, 
                        const std::string &name, 
                        const std::string &defaultValue = std::string()) const;
  std::string content(const xmlNodePtr nodePtr) const;
  std::string name(const xmlNodePtr nodePtr) const;
  bool isElement(const xmlNodePtr nodePtr) const;
};

#endif

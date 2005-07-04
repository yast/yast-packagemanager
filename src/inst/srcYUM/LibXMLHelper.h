#ifndef LibXMLHelper_h
#define LibXMLHelper_h

struct xmlDoc;
struct xmlNode;

class LibXMLHelper {
  LibXMLHelper();
  virtual ~LibXMLHelper();
  std::string attribute(const xmlNode *node, 
                        const std::string &name, 
                        const std::string &defaultValue = std::string()) const;
  std::string content(const xmlNodePtr *nodePtr) const;
  std::string name(const xmlNode *nodePtr) const;
  bool isElement(const xmlNode *nodePtr) const;
}

#endif

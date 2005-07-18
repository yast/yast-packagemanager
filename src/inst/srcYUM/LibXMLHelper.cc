#include <y2pm/LibXMLHelper.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>
#include <cassert>
#include <sstream>

using namespace std;





LibXMLHelper::LibXMLHelper()
{ }

LibXMLHelper::~LibXMLHelper()
{ }

std::string LibXMLHelper::attribute(const xmlNodePtr nodePtr, 
                                    const string &name, 
                                    const string &defaultValue) const
{
  assert(nodePtr);
  xmlChar *xmlRes = xmlGetProp(nodePtr, BAD_CAST(name.c_str()));
  if (xmlRes == 0)
    return defaultValue;
  else {
    string res((const char *)xmlRes);
    xmlFree(xmlRes);
    return res;
  }
}


std::string LibXMLHelper::content(const xmlNodePtr nodePtr) const
{
  assert(nodePtr);
  xmlChar *xmlRes = xmlNodeGetContent(nodePtr);
  if (xmlRes == 0)
    return string();
  else {
    string res((const char*) xmlRes);
    xmlFree(xmlRes);
    return res;
  }
}

std::string LibXMLHelper::name(const xmlNodePtr nodePtr) const
{
  assert(nodePtr);
  return string((const char*) nodePtr->name);
}


bool LibXMLHelper::isElement(const xmlNodePtr nodePtr) const
{
  return nodePtr->type == XML_ELEMENT_NODE;
}

std::string LibXMLHelper::positionInfo(const xmlNodePtr nodePtr) const
{
  stringstream strm;
  strm << nodePtr->line;
  return string("at line ") + strm.str();
}

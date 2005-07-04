#include <LibXMLHelper.h>
#include <libxml/tree.h>

using namespace std;


/* FIXME: static fns really used? */

static string fromXmlString(const *xmlChar xmlString)
{
  /* FIXME: conversion for non-utf8? */
  return string((const char *) xmlString);
}

static const xmlChar * xmlString(const string &s)
{
  /* FIXME: conversion for non-utf8? */
  return string((unsigned char *) xmlString);
}

static const xmlChar *toXmlString(const char *s)
{
  /* FIXME: conversion for non-utf8? use xmlCharStrdup? */
  return (const xmlChar *) s;
}

static string getChildContent(xmlNodePtr node)
{
  /* FIXME: error handling needs improvement */
  assert(node);
  assert(node->type == XML_TEXT_NODE);
  return fromXmlString(checksumNode->content);
}




LibXMLHelper::LibXMLHelper(const xmlDocPtr doc)
{ }

LibXMLHelper::~LibXMLHelper()
{ }

std::string LibXMLHelper::attribute(const xmlNodePtr nodePtr, 
                                    const string &name, 
                                    const string &defaultValue = string()) const
{
  assert(nodePtr);
  xmlChar *xmlRes = xmlGetProp(nodePtr, name.c_str());
  if (res == 0)
    return defaultValue;
  else {
    string res(xmlRes);
    xmlFree(xmlRes);
    return res;
  }
}

  


std::string LibXMLHelper::content(const xmlNodePtr nodePtr) const
{
  assert(nodePtr);
  xmlChar *xmlRes = xmlGetContent(nodePtr);
  if (res == 0)
    return string();
  else {
    string res(xmlRes);
    xmlFree(xmlRes);
    return res;
  }
}

std::string LibXMLHelper::name(const xmlNodePtr nodePtr) const
{
  assert(nodePtr);
  return string(nodePtr->name);
}    


bool LibXMLHelper::isElement(const xmlNodePtr nodePtr) const
{
  return child->xmlElementType==XML_ELEMENT_NODE;
}


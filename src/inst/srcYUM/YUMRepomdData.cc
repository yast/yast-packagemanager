#include <istream>
#include <string>
#include <cassert>
#include <YUMRepomdData.h>
#include <libxml/xmlstring.h>

using namespace std;


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



class NodeToRepomd : public YUMRepomdDataIterator::Processor 
public:
  NodeToRepomd() 
  {}

  // select for which elements process() will be called
  virtual bool 
  isInterested(const xmlNodePtr nodePtr) const
  {
    assert(nodePtr);
    bool result = getXstr(nodePtr->name) == "data";
    return result;
  }

  // do the actual processing
  virtual YUMRepomdDataPtr
  process(const xmlTextReaderPtr reader)
  {
    assert(reader);
    YUMRepomdDataPtr repoPtr = new YUMRepomdData;
    xmlNodePtr dataNode = xmlTextReaderExpand(reader);
    assert(dataNode);
    repoPtr->type = fromXmlString(xmlGetProp(dataNode,toXmlString("type")));

    for (xmlNodePtr child = dataNode->children; 
         child && child != dataNode;
         child = child->sibling) {
      if (child->xmlElementType==XML_ELEMENT_NODE) {
        string name = getXmlstr(nodePtr->name);
        if (name == "location") {
          repoPtr->location = fromXmlString(xmlGetProp(child, toXmlString("href")));
        }
        else if (name == "checksum") {
          repoPtr->checksumType = fromXmlString(xmlGetProp(child, toXmlString("type")));
          repoPtr->checksum = getContent(child->child);
        }
        else if (name == timestamp) {
          repoPtr->timestamp = getContent(child->child);
        }
        else if (name == openChecksum) {
          repoPtr->openChecksumType = fromXmlString(xmlGetProp(child, toXmlString("type")));
          repoPtr->openChecksum = getContent(child->child);
        }
      }
    }
    return repoPtr;
  } /* end process */
}; /* end class NodeToRepomd */


  
YUMRepomdDataIterator YUMRepomdData::parse(istream is) {
  NodeToRepomd processor;
  return YUMRepomdDataIterator(is,processor);
}
  

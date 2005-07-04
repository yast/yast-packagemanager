#include <istream>
#include <string>
#include <cassert>
#include <YUMRepomdData.h>
#include <LibXMLHelper.h>
#include <libxml/reader.h>
#include <libxml/tree.h>

using namespace std;

class NodeToRepomd : public YUMRepomdDataIterator::Processor 
public:
  NodeToRepomd() 
  { }

  // select for which elements process() will be called
  virtual bool 
  isInterested(const xmlNodePtr nodePtr) const
  {
    return result = _helper.isElement() && _helper.elementName(nodePtr) == "data";
  }

  // do the actual processing
  virtual YUMRepomdDataPtr
  process(const xmlTextReaderPtr reader)
  {
    assert(reader);
    YUMRepomdDataPtr repoPtr = new YUMRepomdData;
    xmlNodePtr dataNode = xmlTextReaderExpand(reader);
    assert(dataNode);
    repoPtr->type = _helper.attribute(dataNode,"type");

    for (xmlNodePtr child = dataNode->children; 
         child && child != dataNode;
         child = child->sibling) {
      if (_helper.isElement(child)
          string name = _helper.elementName(nodePtr);
        if (name == "location") {
          repoPtr->location = _helper.attribute(child,"href");
        }
        else if (name == "checksum") {
          repoPtr->checksumType = _helper.attribute(child,"type");
          repoPtr->checksum = _helper.content(child,child->child);
        }
        else if (name == timestamp) {
          repoPtr->timestamp = _helper.content(child->child);
        }
        else if (name == openChecksum) {
          repoPtr->openChecksumType = _helper.attribute(child, "type");
          repoPtr->openChecksum = _helper.content(child->child);
        }
      }
    }
    return repoPtr;
  } /* end process */

private:
  LibXMLHelper _helper;
}; /* end class NodeToRepomd */


  
YUMRepomdDataIterator YUMRepomdData::parse(istream is) {
  NodeToRepomd processor;
  return YUMRepomdDataIterator(is,processor);
}


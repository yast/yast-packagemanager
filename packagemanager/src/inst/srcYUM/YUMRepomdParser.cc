#include <y2pm/YUMRepomdParser.h>
#include <y2util/LibXMLHelper.h>
#include <istream>
#include <string>
#include <cassert>
#include <libxml/xmlreader.h>
#include <libxml/tree.h>
#include <y2util/Y2SLog.h>
#include <y2pm/schemanames.h>

using namespace std;
using namespace YUM;

YUMRepomdParser::~YUMRepomdParser()
{ }

YUMRepomdParser::YUMRepomdParser()
{ }

YUMRepomdParser::YUMRepomdParser(YUMRepomdDataPtr& entry)
: XMLNodeIterator<YUMRepomdDataPtr>(entry)
{ }


// select for which elements process() will be called
bool 
YUMRepomdParser::isInterested(const xmlNodePtr nodePtr)
{
  return _helper.isElement(nodePtr) && _helper.name(nodePtr) == "data";
}

// do the actual processing
YUMRepomdDataPtr
YUMRepomdParser::process(const xmlTextReaderPtr reader)
{
  assert(reader);
  YUMRepomdDataPtr repoPtr = new YUMRepomdData;
  xmlNodePtr dataNode = xmlTextReaderExpand(reader);
  assert(dataNode);
  repoPtr->type = _helper.attribute(dataNode,"type");
  
  for (xmlNodePtr child = dataNode->children; 
       child && child != dataNode;
       child = child->next) {
    if (_helper.isElement(child)) {
      string name = _helper.name(child);
      if (name == "location") {
        repoPtr->location = _helper.attribute(child,"href");
      }
      else if (name == "checksum") {
        repoPtr->checksumType = _helper.attribute(child,"type");
        repoPtr->checksum = _helper.content(child);
      }
      else if (name == "timestamp") {
        repoPtr->timestamp = _helper.content(child);
      }
      else if (name == "open-checksum") {
        repoPtr->openChecksumType = _helper.attribute(child, "type");
        repoPtr->openChecksum = _helper.content(child);
      }
      else {
        WAR << "YUM <data> contains the unknown element <" << name << "> "
          << _helper.positionInfo(child) << ", skipping" << endl;
      }
    }
  }
  return repoPtr;
} /* end process */

  
YUMRepomdParser::YUMRepomdParser(istream &is, const string &baseUrl)
: XMLNodeIterator<YUMRepomdDataPtr>(is, baseUrl,REPOMDSCHEMA)
{
  fetchNext();
}

#include "YUMGroupDataIterator.h"
#include "YUMGroupData.h"
#include "LibXMLHelper.h"
#include <istream>
#include <string>
#include <cassert>
#include <libxml/xmlreader.h>
#include <libxml/tree.h>

using namespace std;
using namespace YUM;

YUMGroupDataIterator::~YUMGroupDataIterator()
{ }


// select for which elements process() will be called
bool 
YUMGroupDataIterator::isInterested(const xmlNodePtr nodePtr)
{
  return _helper.isElement(nodePtr) && _helper.name(nodePtr) == "group";
}

// do the actual processing
YUMGroupDataPtr
YUMGroupDataIterator::process(const xmlTextReaderPtr reader)
{
  assert(reader);
  YUMGroupDataPtr dataPtr = new YUMGroupData;
  xmlNodePtr dataNode = xmlTextReaderExpand(reader);
  assert(dataNode);
  
  for (xmlNodePtr child = dataNode->children;
       child && child != dataNode;
       child = child->next) {
         if (_helper.isElement(child)) {
           string name = _helper.name(child);
           if (name == "groupid") {
             dataPtr->groupid = _helper.content(child);
           }
           else if (name == "name") {
             dataPtr->name.push_back
               (YUMGroupData::MultiLang(_helper.attribute(child,"lang"),
                                        _helper.content(child)));
           }
           else if (name == "default") {
             dataPtr->default = _helper.content(child);
           }
           else if (name == "uservisible") {
             dataPtr->uservisible = _helper.content(child);
           }
           else if (name == "description") {
             dataPtr->description.push_back
               (YUMGroupData::MultiLang(_helper.attribute(child,"lang"),
                                        _helper.content(child)));
           }
           else if (name == "grouplist") {
             parseGrouplist(dataPtr, child);
           }
           else if (name == "packagelist)

           
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
         }
       }
  return repoPtr;
} /* end process */


YUMGroupDataIterator::YUMGroupDataIterator(istream &is, const string &baseUrl)
: XMLNodeIterator<YUMRepomdDataPtr>(is, baseUrl)
{ 
  fetchNext();
}

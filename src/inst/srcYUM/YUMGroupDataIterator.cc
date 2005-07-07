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
             dataPtr->groupId = _helper.content(child);
           }
           else if (name == "name") {
             dataPtr->name.push_back
               (YUMGroupData::MultiLang(_helper.attribute(child,"lang"),
                                        _helper.content(child)));
           }
           else if (name == "default") {
             dataPtr->default_ = _helper.content(child);
           }
           else if (name == "uservisible") {
             dataPtr->userVisible = _helper.content(child);
           }
           else if (name == "description") {
             dataPtr->description.push_back
               (YUMGroupData::MultiLang(_helper.attribute(child,"lang"),
                                        _helper.content(child)));
           }
           else if (name == "grouplist") {
             parseGrouplist(dataPtr, child);
           }
           else if (name == "packagelist") {
             parsePackageList(dataPtr, child);
           }
           else {
             /* FIXME: Log file problem */
           }
         }
       }
  return dataPtr;
} /* end process */

void YUMGroupDataIterator::parseGrouplist(YUMGroupDataPtr dataPtr,
                                          xmlNodePtr node)
{
  assert(dataPtr);
  assert(node);
  
  for (xmlNodePtr child = node->children;
       child != 0;
       child = child ->next) {
         if (_helper.isElement(child)) {
           string name = _helper.name(child);
           if (name == "metapkg" || name == "groupreq") {
             dataPtr->grouplist.push_back
               (YUMGroupData::MetaPkg(_helper.attribute(child,"type"),
                                      _helper.content(child)));
           }
         }
       }
}


void YUMGroupDataIterator::parsePackageList(YUMGroupDataPtr dataPtr,
                                            xmlNodePtr node)
{
  assert(dataPtr);
  assert(node);
  
  for (xmlNodePtr child = node->children;
       child != 0;
       child = child ->next) {
         if (_helper.isElement(child)) {
           string name = _helper.name(child);
           if (name == "packagereq") {
           dataPtr->packageList.push_back
             (YUMGroupData::PackageReq(_helper.attribute(child,"type"),
                                       _helper.attribute(child,"epoch"),
                                       _helper.attribute(child,"ver"),
                                       _helper.attribute(child,"rel"),
                                       _helper.content(child)));
           }
         }
       }
}



YUMGroupDataIterator::YUMGroupDataIterator(istream &is, const string &baseUrl)
: XMLNodeIterator<YUMGroupDataPtr>(is, baseUrl)
{ 
  fetchNext();
}

#include <y2pm/YUMGroupParser.h>
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

YUMGroupParser::YUMGroupParser()
{ }

YUMGroupParser::YUMGroupParser(YUMGroupDataPtr& entry)
: XMLNodeIterator<YUMGroupDataPtr>(entry)
{ }


YUMGroupParser::~YUMGroupParser()
{ }


// select for which elements process() will be called
bool 
YUMGroupParser::isInterested(const xmlNodePtr nodePtr)
{
  return _helper.isElement(nodePtr) && _helper.name(nodePtr) == "group";
}

// do the actual processing
YUMGroupDataPtr
YUMGroupParser::process(const xmlTextReaderPtr reader)
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
               (MultiLang(_helper.attribute(child,"lang"),
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
               (MultiLang(_helper.attribute(child,"lang"),
                          _helper.content(child)));
           }
           else if (name == "grouplist") {
             parseGrouplist(dataPtr, child);
           }
           else if (name == "packagelist") {
             parsePackageList(dataPtr, child);
           }
           else {
             WAR << "YUM <group> contains the unknown element <" << name << "> "
               << _helper.positionInfo(child) << ", skipping" << endl;
           }
         }
       }
  return dataPtr;
} /* end process */

void YUMGroupParser::parseGrouplist(YUMGroupDataPtr dataPtr,
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
               (MetaPkg(_helper.attribute(child,"type"),
                        _helper.content(child)));
           }
           else {
             WAR << "YUM <grouplist> contains the unknown element <" << name << "> "
               << _helper.positionInfo(child) << ", skipping" << endl;
           }
         }
       }
}


void YUMGroupParser::parsePackageList(YUMGroupDataPtr dataPtr,
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
             (PackageReq(_helper.attribute(child,"type"),
                         _helper.attribute(child,"epoch"),
                         _helper.attribute(child,"ver"),
                         _helper.attribute(child,"rel"),
                         _helper.content(child)));
           }
           else {
             WAR << "YUM <packagelist> contains the unknown element <" << name << "> "
               << _helper.positionInfo(child) << ", skipping" << endl;
           }
         }
       }
}



YUMGroupParser::YUMGroupParser(istream &is, const string &baseUrl)
: XMLNodeIterator<YUMGroupDataPtr>(is, baseUrl,GROUPSCHEMA)
{ 
  fetchNext();
}

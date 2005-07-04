#include <istream>
#include <string>
#include <cassert>
#include <YUMRepomdData.h>
#include <libxml/xmlstring.h>
#include <LibXMLHelper.h>

using namespace std;



class NodeToPrimary : public YUMRepomdDataIterator::Processor 
public:
  NodeToPrimary() 
  {}

  // select for which elements process() will be called
  virtual bool 
  isInterested(const xmlNodePtr nodePtr) const
  {
    bool result = (_helper.isElement(nodePtr)
                   && _helper.name(nodePtr) == "package");
    return result;
  }

  // do the actual processing
  virtual YUMRepomdDataPtr
  process(const xmlTextReaderPtr reader)
  {
    assert(reader);
    YUMRepomdDataPtr dataPtr = new YUMRepomdData;
    xmlNodePtr dataNode = xmlTextReaderExpand(reader);
    assert(dataNode);
    
    dataPtr->type = _helper.attribute(dataNode);
    List<YUMDependency> *currentDependency = 0; // for <entry>, determines which dependency to add to

    for (xmlNodePtr child = dataNode->children; 
         child && child != dataNode;
         child = child->sibling) {
      if (_helper.isElement(child)) {
        string name = _helper.name(child);
        if (name == "name") {
          dataPtr->name = _helper.content(child->child);
        }
        else if (name == "arch") {
          dataPtr->arch = _helper.content(child->child);
        }
        else if (name == "version") {
          dataPtr->epoch = _helper.attribute(child,"epoch");
          dataPtr->ver = _helper.attribute(child,"ver");
          dataPtr->rel = _helper.attribute(child,"rel");
        }
        else if (name == "checksum") {
          dataPtr->checksumType = _helper.attribute(child,"type");
          data Ptr->checksum = _helper.content(child->child);
        }
        else if (name == "summary") {
          dataPtr->summary = _helper.content(child->child);
        }
        else if (name == "description") {
          dataPtr->description = _helper.content(child->child);
        }
        else if (name == "packager") {
          dataPtr->packager = _helper.content(child->child);
        }
        else if (name == "url") {
          dataPtr->url = _helper.content(child->child);
        }
        else if (name == "time") {
          dataPtr->time_file = _helper.attribute(child,"file");
          dataPtr->time_installed = _helper.attribute(child,"installed");
        }
        else if (name == "location") {
          dataPtr->location = _helper.attribute(child,"href");
        }
        else if (name == "license") {
          dataPtr->license = _helper.content(child->child);
        }
        else if (name == "vendor") {
          dataPtr->vendor = _helper.content(child->child);
        }
        else if (name == "group") {
          dataPtr->group = _helper.content(child->child);
        }
        else if (name == "buildhost") {
          dataPtr->buildhost = _helper.content(child->child);
        }
        else if (name == "sourerpm") {
          dataPtr->sourerpm = _helper.content(child->child);
        }
        else if (name == "header-range") {
          dataPtr->header_start = _helper.attribute(child,"start");
          dataPtr->header_end = _helper.attribute(child,"end");
        }
        else if (name == "provides") {
          currentDependency = dataPtr->provides;
        }
        else if (name == "conflicts") {
          currentDependency = dataPtr->conflicts;
        }
        else if (name == "obsoletes") {
          currentDependency = dataPtr->obsoletes;
        }
        else if (name == "requires") {
          currentDependency = dataPtr->requires;
        }
        else if (name == "entry") {
          assert(currentDependency != 0);
          currentDependency->push_back(
                  {_helper.attribute(nodePtr,"name"),
                   _helper.attribute(nodePtr,"flags"),
                   _helper.attribute(nodePtr,"epoch")
                   _helper.attribute(nodePtr,"ver"),
                   _helper.attribute(nodePtr,"rel")});
        }
        else if (name == "file") {
          dataPtr->files.push_back(YUMPrimaryData::FileData(
                                  {_helper.attribute(child,"type"),
                                   _helper.content(child->child) } ));
        }
      }
    }
    return repoPtr;
  } /* end process */

private:
  LibXMLHelper _helper;

}; /* end class NodeToRepomd */


YUMDependency YUMRepomdData::parseDependency(const xmlNodePtr nodePtr)
{
  return 
}

  
YUMRepomdDataIterator YUMRepomdData::parse(istream is) {
  NodeToRepomd processor;
  return YUMRepomdDataIterator(is,processor);
}




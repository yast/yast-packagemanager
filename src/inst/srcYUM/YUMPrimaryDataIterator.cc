#include "YUMPrimaryDataIterator.h"
#include "YUMPrimaryData.h"
#include <istream>
#include <string>
#include <cassert>
#include <libxml/xmlstring.h>
#include "LibXMLHelper.h"

using namespace std;


// select for which elements process() will be called
bool 
YUMPrimaryDataIterator::isInterested(const xmlNodePtr nodePtr)
{
  bool result = (_helper.isElement(nodePtr)
                 && _helper.name(nodePtr) == "package");
  return result;
}


// do the actual processing
YUMPrimaryDataPtr
YUMPrimaryDataIterator::process(const xmlTextReaderPtr reader)
{
  assert(reader);
  YUMPrimaryDataPtr dataPtr = new YUMPrimaryData;
  xmlNodePtr dataNode = xmlTextReaderExpand(reader);
  assert(dataNode);
  
  dataPtr->type = _helper.attribute(dataNode,"type");
  list<YUMDependency> *currentDependency = 0; // for <entry>, determines which dependency to add to
  
  for (xmlNodePtr child = dataNode->children; 
       child && child != dataNode;
       child = child->next) {
    if (_helper.isElement(child)) {
      string name = _helper.name(child);
      if (name == "name") {
        dataPtr->name = _helper.content(child);
      }
      else if (name == "arch") {
        dataPtr->arch = _helper.content(child);
      }
      else if (name == "version") {
        dataPtr->epoch = _helper.attribute(child,"epoch");
        dataPtr->ver = _helper.attribute(child,"ver");
        dataPtr->rel = _helper.attribute(child,"rel");
      }
      else if (name == "checksum") {
        dataPtr->checksumType = _helper.attribute(child,"type");
        dataPtr->checksum = _helper.content(child);
      }
      else if (name == "summary") {
        dataPtr->summary = _helper.content(child);
      }
      else if (name == "description") {
        dataPtr->description = _helper.content(child);
      }
      else if (name == "packager") {
        dataPtr->packager = _helper.content(child);
      }
      else if (name == "url") {
        dataPtr->url = _helper.content(child);
      }
      else if (name == "time") {
        dataPtr->timeFile = _helper.attribute(child,"file");
        dataPtr->timeBuild = _helper.attribute(child,"build");
      }
      else if (name == "location") {
        dataPtr->location = _helper.attribute(child,"href");
      }
      else if (name == "license") {
        dataPtr->license = _helper.content(child);
      }
      else if (name == "vendor") {
        dataPtr->vendor = _helper.content(child);
      }
      else if (name == "group") {
        dataPtr->group = _helper.content(child);
      }
      else if (name == "buildhost") {
        dataPtr->buildhost = _helper.content(child);
      }
      else if (name == "sourerpm") {
        dataPtr->sourcerpm = _helper.content(child);
      }
      else if (name == "header-range") {
        dataPtr->headerStart = _helper.attribute(child,"start");
        dataPtr->headerEnd = _helper.attribute(child,"end");
      }
      else if (name == "provides") {
        currentDependency = &dataPtr->provides;
      }
      else if (name == "conflicts") {
        currentDependency = &dataPtr->conflicts;
      }
      else if (name == "obsoletes") {
        currentDependency = &dataPtr->obsoletes;
      }
      else if (name == "requires") {
        currentDependency = &dataPtr->requires;
      }
      else if (name == "entry") {
        assert(currentDependency != 0);
        currentDependency->push_back
          (YUMDependency(_helper.attribute(child,"name"),
                         _helper.attribute(child,"flags"),
                         _helper.attribute(child,"epoch"),
                         _helper.attribute(child,"ver"),
                         _helper.attribute(child,"rel")));
      }
      else if (name == "file") {
        dataPtr->files.push_back
          (YUMPrimaryData::FileData(_helper.attribute(child,"type"),
                                    _helper.content(child)));
      }
    }
  }
  return dataPtr;
} /* end process */


YUMPrimaryDataIterator::YUMPrimaryDataIterator(istream &is, const string& baseUrl)
  : XMLNodeIterator<YUMPrimaryDataPtr>(is, baseUrl)
{
  fetchNext();
}


YUMPrimaryDataIterator::~YUMPrimaryDataIterator()
{
}
  



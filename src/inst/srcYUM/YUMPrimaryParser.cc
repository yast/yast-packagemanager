#include <YUMPrimaryDataIterator.h>
#include <YUMPrimaryData.h>
#include <istream>
#include <string>
#include <cassert>
#include <libxml/xmlstring.h>
#include <LibXMLHelper.h>

using namespace std;
using namespace YUM;


YUMPrimaryDataIterator::YUMPrimaryDataIterator(istream &is, const string& baseUrl)
  : XMLNodeIterator<YUMPrimaryDataPtr>(is, baseUrl)
{
  fetchNext();
}


YUMPrimaryDataIterator::~YUMPrimaryDataIterator()
{
}
  



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
  dataPtr->installOnly = false;
  
  for (xmlNodePtr child = dataNode->children; 
       child != 0;
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
        dataPtr->checksumPkgid = _helper.attribute(child,"pkgid");
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
      else if (name == "size") {
        dataPtr->sizePackage = _helper.attribute(child,"package");
        dataPtr->sizeInstalled = _helper.attribute(child,"installed");
        dataPtr->sizeArchive = _helper.attribute(child,"archive");
      }
      else if (name == "location") {
        dataPtr->location = _helper.attribute(child,"href");
      }
      else if (name == "format") {
        parseFormatNode(dataPtr, child);
      }
    }
  }
  return dataPtr;
} /* end process */



void 
YUMPrimaryDataIterator::parseFormatNode(YUMPrimaryDataPtr dataPtr,
                                        xmlNodePtr formatNode)
{
  assert(formatNode);
  dataPtr->installOnly = false;
  for (xmlNodePtr child = formatNode->children; 
       child != 0;
       child = child ->next) {
    if (_helper.isElement(child)) {
      string name = _helper.name(child);
      if (name == "license") {
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
      else if (name == "sourcerpm") {
        dataPtr->sourcerpm = _helper.content(child);
      }
      else if (name == "header-range") {
        dataPtr->headerStart = _helper.attribute(child,"start");
        dataPtr->headerEnd = _helper.attribute(child,"end");
      }
      else if (name == "provides") {
        parseDependencyEntries(& dataPtr->provides, child);
      }
      else if (name == "conflicts") {
        parseDependencyEntries(& dataPtr->conflicts, child);
      }
      else if (name == "obsoletes") {
        parseDependencyEntries(& dataPtr->obsoletes, child);
      }
      else if (name == "requires") {
        parseDependencyEntries(& dataPtr->requires, child);
      }
      else if (name == "file") {
        dataPtr->files.push_back
          (YUMPrimaryData::FileData(_helper.content(child),
                                    _helper.attribute(child,"type")));
      }
      /* SUSE specific elements */
      else if (name == "authors") {
        parseAuthorEntries(& dataPtr->authors, child);
      }
      else if (name == "keywords") {
        parseKeywordEntries(& dataPtr->keywords, child);
      }
      else if (name == "media") {
        dataPtr->media = _helper.attribute(child,"mediaid");
      }
      else if (name == "dirsizes") {
        parseDirsizeEntries(& dataPtr->dirSizes, child);
      }
      else if (name == "freshen") {
        parseDependencyEntries(& dataPtr->freshen, child);
      }
      else if (name == "install_only") {
        dataPtr->installOnly = true;
      }
      else { /* FIXME: log parse warning */ }
    }
  }
}


void
YUMPrimaryDataIterator::parseDependencyEntries(list<YUMDependency> *depList, 
                                               xmlNodePtr depNode)
{
  assert(depList);
  assert(depNode);

  for (xmlNodePtr child = depNode->children; 
       child != 0;
       child = child ->next) {
    if (_helper.isElement(child)) {
      string name = _helper.name(child);
      if (name == "entry") { 
        depList->push_back
          (YUMDependency(_helper.attribute(child,"name"),
                         _helper.attribute(child,"flags"),
                         _helper.attribute(child,"epoch"),
                         _helper.attribute(child,"ver"),
                         _helper.attribute(child,"rel")));
      }
    }
  }
}

void
YUMPrimaryDataIterator::parseAuthorEntries(list<string> *authors,
                                           xmlNodePtr node)
{
  assert(authors);
  assert(node);

  for (xmlNodePtr child = node->children; 
       child != 0;
       child = child ->next) {
    if (_helper.isElement(child)) {
      string name = _helper.name(child);
      if (name == "author") { 
        authors->push_back(_helper.content(child));
      }
    }
  }
}

void YUMPrimaryDataIterator::parseKeywordEntries(list<string> *keywords,
                                                 xmlNodePtr node)
{
  assert(keywords);
  assert(node);

  for (xmlNodePtr child = node->children; 
       child != 0;
       child = child ->next) {
    if (_helper.isElement(child)) {
      string name = _helper.name(child);
      if (name == "keyword") { 
        keywords->push_back(_helper.content(child));
      }
    }
  }
}

void YUMPrimaryDataIterator::parseDirsizeEntries(list<YUMDirSize> *sizes,
                                                 xmlNodePtr node)
{
  assert(sizes);
  assert(node);

  for (xmlNodePtr child = node->children; 
       child != 0;
       child = child ->next) {
    if (_helper.isElement(child)) {
      string name = _helper.name(child);
      if (name == "dirsize") { 
        sizes->push_back(YUMDirSize(_helper.attribute(child,"path"),
                                    _helper.attribute(child,"size-kbyte"),
                                    _helper.attribute(child,"filecount")));
      }
    }
  }
}

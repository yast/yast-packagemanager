

#ifndef YUMPrimaryDataIterator_h
#define YUMPrimaryDataIterator_h

#include "YUMPrimaryDataPtr.h"
#include "XMLNodeIterator.h"
#include "LibXMLHelper.h"
#include "YUMDependency.h"
#include "YUMDirSize.h"
#include <list>

namespace YUM {

  class YUMDependency;

  class YUMPrimaryDataIterator : public XMLNodeIterator<YUMPrimaryDataPtr>
  {
  public:
    YUMPrimaryDataIterator(std::istream &is, const std::string &baseUrl);
    virtual ~YUMPrimaryDataIterator();

  private:
    virtual bool isInterested(const xmlNodePtr nodePtr);
    virtual YUMPrimaryDataPtr process(const xmlTextReaderPtr reader);
    void parseFormatNode(YUMPrimaryDataPtr dataPtr,
                         xmlNodePtr formatNode);
    void parseDependencyEntries(std::list<YUMDependency> *depList, 
                                xmlNodePtr depNode);
    void YUMPrimaryDataIterator::parseAuthorEntries(std::list<std::string> *authors,
                                                    xmlNodePtr node);
    void YUMPrimaryDataIterator::parseKeywordEntries(std::list<std::string> *keywords,
                                                     xmlNodePtr node);
    void YUMPrimaryDataIterator::parseDirsizeEntries(std::list<YUMDirSize> *sizes,
                                                     xmlNodePtr node);

    LibXMLHelper _helper;
  };
}

#endif

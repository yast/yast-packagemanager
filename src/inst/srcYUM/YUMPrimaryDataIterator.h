

#ifndef YUMPrimaryDataIterator_h
#define YUMPrimaryDataIterator_h

#include "YUMPrimaryDataPtr.h"
#include "XMLNodeIterator.h"
#include "LibXMLHelper.h"
#include "YUMDependency.h"
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

    LibXMLHelper _helper;
  };
}

#endif



#ifndef YUMGroupDataIterator_h
#define YUMGroupDataIterator_h

#include "YUMGroupDataPtr.h"
#include "XMLNodeIterator.h"
#include "LibXMLHelper.h"
#include <list>

namespace YUM {

  class YUMGroupDataIterator : public XMLNodeIterator<YUMGroupDataPtr>
  {
  public:
    YUMGroupDataIterator(std::istream &is, const std::string &baseUrl);
    virtual ~YUMGroupDataIterator();
    
  private:
    virtual bool isInterested(const xmlNodePtr nodePtr);
    virtual YUMGroupDataPtr process(const xmlTextReaderPtr reader);
    void parseGrouplist(YUMGroupDataPtr dataPtr,
                        xmlNodePtr node);
    void parsePackageList(YUMGroupDataPtr dataPtr,
                          xmlNodePtr node);
    
    LibXMLHelper _helper;
  };
}

#endif

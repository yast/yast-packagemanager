

#ifndef YUMGroupParser_h
#define YUMGroupParser_h

#include <YUMParserData.h>
#include <XMLNodeIterator.h>
#include <LibXMLHelper.h>
#include <list>

namespace YUM {

  class YUMGroupParser : public XMLNodeIterator<YUMGroupDataPtr>
  {
  public:
    YUMGroupParser(std::istream &is, const std::string &baseUrl);
    virtual ~YUMGroupParser();
    
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

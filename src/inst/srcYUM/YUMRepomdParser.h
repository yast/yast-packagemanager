

#ifndef YUMRepomdParser_h
#define YUMRepomdParser_h

#include <YUMData.h>
#include <XMLNodeIterator.h>
#include <LibXMLHelper.h>

namespace YUM {

  class YUMRepomdParser : public XMLNodeIterator<YUMRepomdDataPtr>
  {
  public:
    YUMRepomdParser(std::istream &is, const std::string &baseUrl);
    virtual ~YUMRepomdParser();

  private:
    virtual bool isInterested(const xmlNodePtr nodePtr);
    virtual YUMRepomdDataPtr process(const xmlTextReaderPtr reader);

    LibXMLHelper _helper;
  };
}

#endif



#ifndef YUMPrimaryParser_h
#define YUMPrimaryParser_h

#include <YUMData.h>
#include <XMLNodeIterator.h>
#include <LibXMLHelper.h>
#include <list>

namespace YUM {

  class YUMDependency;

  class YUMPrimaryParser : public XMLNodeIterator<YUMPrimaryDataPtr>
  {
  public:
    YUMPrimaryParser(std::istream &is, const std::string &baseUrl);
    virtual ~YUMPrimaryParser();

  private:
    virtual bool isInterested(const xmlNodePtr nodePtr);
    virtual YUMPrimaryDataPtr process(const xmlTextReaderPtr reader);
    void parseFormatNode(YUMPrimaryDataPtr dataPtr,
                         xmlNodePtr formatNode);
    void parseDependencyEntries(std::list<YUMDependency> *depList, 
                                xmlNodePtr depNode);
    void parseAuthorEntries(std::list<std::string> *authors,
                            xmlNodePtr node);
    void parseKeywordEntries(std::list<std::string> *keywords,
                             xmlNodePtr node);
    void parseDirsizeEntries(std::list<YUMDirSize> *sizes,
                             xmlNodePtr node);

    LibXMLHelper _helper;
  };
}

#endif

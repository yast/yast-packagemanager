

#ifndef YUMRepomdParser_h
#define YUMRepomdParser_h

#include <y2pm/YUMParserData.h>
#include <y2pm/XMLNodeIterator.h>
#include <y2pm/LibXMLHelper.h>

namespace YUM {

  /**
  * @short Parser for YUM repomd.xml files (describing the repository)
  * Use this class as an iterator that produces, one after one,
  * YUMRepomdDataPtr(s) for the XML package elements.
  * Here's an example:
  *
  * for (YUMRepomdParser iter(anIstream, baseUrl),
  *      iter != YUMOtherParser.end(),     // or: iter() != 0, or ! iter.atEnd()
  *      ++iter) {
  *    doSomething(*iter)
  * }
  *
  * The iterator owns the pointer (i.e., caller must not delete it)
  * until the next ++ operator is called. At this time, it will be
  * destroyed (and a new ENTRYTYPE is created.)
  *
  * If the input is fundamentally flawed so that it makes no sense to
  * continue parsing, XMLNodeIterator will log it and consider the input as finished.
  * You can query the exit status with errorStatus().
  */
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

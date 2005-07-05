

#ifndef YUMRepomdDataIterator_h
#define YUMRepomdDataIterator_h

#include "YUMRepomdDataPtr.h"
#include "XMLNodeIterator.h"
#include "LibXMLHelper.h"

class YUMRepomdDataIterator : public XMLNodeIterator<YUMRepomdDataPtr>
{
public:
  YUMRepomdDataIterator(std::istream &is, const std::string &baseUrl);
  virtual ~YUMRepomdDataIterator();
  virtual bool isInterested(const xmlNodePtr nodePtr);
  virtual YUMRepomdDataPtr process(const xmlTextReaderPtr reader);
private:
  LibXMLHelper _helper;
};

#endif

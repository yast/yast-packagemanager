

#ifndef YUMPrimaryDataIterator_h
#define YUMPrimaryDataIterator_h

#include "YUMPrimaryDataPtr.h"
#include "XMLNodeIterator.h"
#include "LibXMLHelper.h"

class YUMPrimaryDataIterator : public XMLNodeIterator<YUMPrimaryDataPtr>
{
public:
  YUMPrimaryDataIterator(std::istream &is, const std::string &baseUrl);
  virtual ~YUMPrimaryDataIterator();

private:
  virtual bool isInterested(const xmlNodePtr nodePtr);
  virtual YUMPrimaryDataPtr process(const xmlTextReaderPtr reader);

  LibXMLHelper _helper;
};

#endif

/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:       XMLNodeIterator.h

  Author:     Michael Radziej <mir@suse.de>
  Maintainer: Michael Radziej <mir@suse.de>

  Purpose: Provides an iterator interface for XML files

  Use like:
  typedef Iterator XMLNodeIterator<ContentClass>
  aNodeProcessor = Iterator::ProcessorSubclass();
  for (Iterator iter(anIstream, aNodeProcessor),
       iter != Iterator.end(),     -- or: iter() != 0, or ! iter.atEnd()
       ++iter) {
     doSomething(*iter)
  }

  processNode() uses the xmlTextReaderPtr to find the data it needs 
  and returns a ContentClass object. (Hint: xmlTextReaderCurrentNode() 
  returns the current node)

  FIXME: error handling (esp. out of memory) missing
         NodeProcessor should be referenced with reference counting?

*/


#include <libxml/xmlreader.h>
// #include <libxml/debugXML.h>
#include <iostream>
#include <cassert>

struct xmlDoc;

static int ioread(void *context,
                  char *buffer,
                  int bufferLen)
{
  assert(buffer);
  std::istream *streamPtr = (std::istream *) context;
  assert(streamPtr);
  streamPtr->read(buffer,bufferLen);
  return streamPtr->gcount();
}

static int ioclose(void * context)
{
  /* don't close. destructor will take care. */
  return 0;
}

template <class ENTRYTYPE>
class XMLNodeIterator {
public:
  class Processor;
  XMLNodeIterator(std::istream &s, 
                  const std::string &baseUrl,
                  Processor &processor)
  {
    d = new Data({s, 
                  xmlReaderForIO(ioread, ioclose, &s, baseUrl.c_str(), "utf-8",0),
                  processor});
    _currentDataPtr = 0;
    fetchNext();
  }


  /* this is a trivial iterator over only one element
     needed e.g. for postinc (iter++) */
  XMLNodeIterator(const ENTRYTYPE &entry)
  { 
    d = 0;
    _currentDataPtr = new ENTRYTYPE(entry);
  }
    
  /* this is the final NodeIterator returned by last() */
  XMLNodeIterator() 
    : _currentDataPtr(0), d(0)
  { }
      
  ~XMLNodeIterator() 
  { /* FIXME */
    if (d) {
      if (d->reader != 0)
        xmlFreeTextReader(d->reader);
    }
  }

  static XMLNodeIterator<ENTRYTYPE> 
  end()
  {
    return XMLNodeIterator<ENTRYTYPE>();
  }

  bool atEnd() const
  {
    return _currentDataPtr == 0;
  }

  bool 
  operator==(const XMLNodeIterator<ENTRYTYPE> &otherNode) const 
  {
    if (atEnd())
      return otherNode.atEnd();
    else if (otherNode.atEnd())
      return false;
    else 
      return *_currentDataPtr == *otherNode._currentDataPtr;
  }

  bool 
  operator!=(const XMLNodeIterator<ENTRYTYPE> &otherNode) const 
  {
    return ! operator==(otherNode);
  }

  const ENTRYTYPE & 
  operator*() const 
  {
    assert (! atEnd());
    return * _currentDataPtr;
  }

  const ENTRYTYPE * 
  operator()() const 
  {
    return _currentDataPtr;
  }

  XMLNodeIterator<ENTRYTYPE> &  /* ++iter */
  operator++() {
    assert (d && !atEnd());
    fetchNext();
    return *this;
  }

  XMLNodeIterator operator++(int)   /* iter++ */
  {
    assert (d && !atEnd());
    XMLNodeIterator<ENTRYTYPE> tmp(*currentDataPtr);
    fetchNext();
    return tmp;
  }

  class Processor {
  public:
    Processor(const xmlDoc *docPtr) 
    {}

    virtual ~Processor()
    { }

    virtual Processor & 
    operator=(const Processor& otherProcessor)
    {
      return *this;
    }

    virtual bool 
    isInterested(const xmlNodePtr nodePtr) const = 0;

    virtual ENTRYTYPE 
    process(const xmlTextReaderPtr readerPtr) = 0;
  }; /* end class Processor */




private:

  void fetchNext() /* this might be the only stuff that makes sense to be overloaded.
                      declare as virtual this is needed once */
  {
    int status;
    assert (! atEnd());
    if (d == 0) {
      /* this is a trivial iterator over only one element,
         and we reach the end now. */
      _currentDataPtr.reset();
    }
    else {
      assert(d->reader);
      /* repeat as long as we successfully read nodes
         breaks out when Processor is interested */
      while ((status = xmlTextReaderRead(d->reader))==1) {
        xmlNodePtr node = xmlTextReaderCurrentNode(d->reader);
        if (d->processor.isInterested(node)) {
          _currentDataPtr.reset(d->processor.process(node));
          // status = xmlTextReaderNext(d->reader);     <--- do we need to skip the subtree?
          break;
        }
      }
      if (status == 0) {  // no more nodes to read
        _currentDataPtr.reset();
      }
      else if (status != 1) {  // error occured
        /* FIXME: error handling */
        _currentDataPtr.reset();
      }
    }
  }

  /* forbid copy constructor */
  XMLNodeIterator<ENTRYTYPE> & operator=(const XMLNodeIterator<ENTRYTYPE>& otherNode)
  { 
    /* copy cannot work since you cannot copy a xmlTextReader */
  }

  /* hold the private data that might
     not exist in a trivial iterator */
  class Data {
    std::istream &s;
    xmlTextReaderPtr reader;
    Processor &processor;
    ~Data() {
      xmlFreeTextReader(reader);
    }
  }; /* end class Data */

  std::auto_ptr(ENTRYTYPE) _currentDataPtr;
      
  std::auto_ptr<Data> d;
 
}; /* end class XMLNodeIterator */










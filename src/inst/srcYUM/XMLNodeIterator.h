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
  for (Iterator iter(anIstream, &aNodeProcessor),
       iter != Iterator.end(),     -- or: iter() != 0, or ! iter.atEnd()
       ++iter) {
     doSomething(*iter)
  }

  processNode() must convert the xml node structure in an xmlNodePtr
  to ContentClass.

  FIXME: error handling (esp. out of memory) missing
         NodeProcessor should be referenced with reference counting

*/


#include <libxml/xmlreader.h>
// #include <libxml/debugXML.h>
#include <iostream>
#include <cassert>


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
                  Processor *processor)
  {
    d = new Data(s, 
                 xmlReaderForIO(ioread, ioclose, &s, baseUrl.c_str(), "utf-8",0),
                 processor);
    if (d->reader == 0)
      _currentDataPtr = 0; /* FIXME: Out of mem */
    else {
      _currentDataPtr = new ENTRYTYPE;
      fetchNext();
    }
  }


  /* this is a trivial iterator over only one element
     needed e.g. for postinc (iter++) */
  XMLNodeIterator(const ENTRYTYPE &entry)
    : d(0)
  { 
    _currentDataPtr = new ENTRYTYPE;
    assert(_currentDataPtr);
    *_currentDataPtr = entry;
  }
    
  /* this is the final NodeIterator returned by last() */
  XMLNodeIterator() 
    : _currentDataPtr(0), d(0)
  { }
      
  ~XMLNodeIterator() 
  {
    if (_currentDataPtr != 0)
      delete _currentDataPtr;
    if (d) {
      if (d->reader != 0)
        xmlFreeTextReader(d->reader);
      delete d;
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

protected:
  void fetchNext() /* this might be the only stuff that makes sense to be overloaded.
                      declare as virtual this is needed once */
  {
    int status;
    assert (! atEnd());
    if (d == 0) {
      /* this is a trivial iterator over only one element */
      delete _currentDataPtr;
      _currentDataPtr = 0;
    }
    else {
      assert(d->reader);
      /* repeat as long as we successfully read nodes
         breaks out when Processor is interested */
      while ((status = xmlTextReaderRead(d->reader))==1) {
        if (d->processor->isInterested(xmlTextReaderCurrentNode(d->reader))) {
          xmlNodePtr nodePtr = xmlTextReaderExpand(d->reader);
          // xmlDebugDumpNode(stdout, nodePtr, 4);
          *_currentDataPtr = d->processor->process(nodePtr);
          // status = xmlTextReaderNext(d->reader);     <--- do we need to skip the subtree?
          break;
        }
      }
      if (status == 0) {  // no more nodes to read
        delete _currentDataPtr;
        _currentDataPtr = 0;
      }
      else if (status != 1) {  // error occured
        /* FIXME: error handling */
        delete _currentDataPtr;
        _currentDataPtr = 0;
      }        
    }
  }

  ENTRYTYPE *_currentDataPtr; /* is 0 at end */
      
private:
  /* forbid copy constructor */
  XMLNodeIterator<ENTRYTYPE> & operator=(const XMLNodeIterator<ENTRYTYPE>& otherNode)
  { 
    /* copy cannot work since you cannot copy a xmlTextReader */
  }

  /* a stupid "class" to hold the private data that might
     not exist in a trivial iterator */
  class Data {
  public:
    std::istream &s;
    xmlTextReaderPtr reader;
    Processor *processor;
    Data(std::istream &s, 
         const xmlTextReaderPtr &reader, 
         Processor *processor)
      : s(s), reader(reader), processor(processor)
    {}
  }; /* end class Data */

  Data *d;
 
  class Processor {
  public:
    Processor() 
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
    process(const xmlNodePtr nodePtr) = 0;
  }; /* end class Processor */



}; /* end class XMLNodeIterator */










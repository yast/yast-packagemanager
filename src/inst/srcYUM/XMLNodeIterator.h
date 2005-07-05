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
  Create a subclass YourIterator : public XMLNodeIterator<ContentClass>, and
  override isInterested() and process() to implement the actual processing.

  for (Iterator iter(anIstream, baseUrl),
       iter != Iterator.end(),     -- or: iter() != 0, or ! iter.atEnd()
       ++iter) {
     doSomething(*iter)
  }

  (Hint: xmlTextReaderCurrentNode() returns the current node)

  FIXME: error handling (esp. out of memory) missing
         Create a common subclass to reduce template bloat

*/

#ifndef XMLNodeIterator_h
#define XMLNodeIterator_h

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
  XMLNodeIterator(std::istream &s, 
                  const std::string &baseUrl)
  {  
    d.reset(new Data(s, 
                 xmlReaderForIO(ioread, ioclose, &s, baseUrl.c_str(), "utf-8",0)));
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
      
  virtual ~XMLNodeIterator() 
  { /* FIXME */
    if (d.get()) {
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
    return _currentDataPtr.get() == 0;
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
    return *(_currentDataPtr.get());
  }

  const ENTRYTYPE * 
  operator()() const 
  {
    return (_currentDataPtr.get());
  }

  XMLNodeIterator<ENTRYTYPE> &  /* ++iter */
  operator++() {
    assert (d.get() && !atEnd());
    fetchNext();
    return *this;
  }

  XMLNodeIterator operator++(int)   /* iter++ */
  {
    assert (d.get() && !atEnd());
    XMLNodeIterator<ENTRYTYPE> tmp(*currentDataPtr);
    fetchNext();
    return tmp;
  }

protected:

  /* which nodes result in an ENTRYTYPE? */
  virtual bool 
  isInterested(const xmlNodePtr nodePtr) = 0;
  
  /* convert a node to ENTRYTYPE */
  virtual ENTRYTYPE 
  process(const xmlTextReaderPtr readerPtr) = 0;
  
private:
  
  void fetchNext()
  {
    int status;
    assert (! atEnd());
    if (d.get() == 0) {
      /* this is a trivial iterator over only one element,
         and we reach the end now. */
      _currentDataPtr.reset();
    }
    else {
      assert(d->reader);
      /* repeat as long as we successfully read nodes
         breaks out an interesting node has been found */
      while ((status = xmlTextReaderRead(d->reader))==1) {
        xmlNodePtr node = xmlTextReaderCurrentNode(d->reader);
        if (isInterested(node)) {
          _currentDataPtr.reset(new ENTRYTYPE(process(d->reader)));
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

  /* forbid assignment: We can't copy an xmlTextReader */
  XMLNodeIterator<ENTRYTYPE> & operator=(const XMLNodeIterator<ENTRYTYPE>& otherNode);

  /* forbid copy constructor: We can't copy an xmlTextReader */
  XMLNodeIterator<ENTRYTYPE>(const XMLNodeIterator<ENTRYTYPE>& otherNode);

  /* hold the private data that might
     not exist in a trivial iterator */
  class Data {
  public:
    std::istream& s;
    xmlTextReaderPtr reader;
    Data(std::istream& s, 
         xmlTextReaderPtr reader)
      : s(s), reader(reader)
    { }

    ~Data() 
    {
      xmlFreeTextReader(reader);
    }
  }; /* end class Data */

  std::auto_ptr<ENTRYTYPE> _currentDataPtr;
      
  std::auto_ptr<Data> d;
 
}; /* end class XMLNodeIterator */


#endif







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
#include <iterator>

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
class XMLNodeIterator : public std::iterator<std::input_iterator_tag, ENTRYTYPE> {
public:
  XMLNodeIterator(std::istream &input, 
                  const std::string &baseUrl)
    : _input(& input),
      _reader(xmlReaderForIO(ioread, ioclose, _input, baseUrl.c_str(), "utf-8",0)),
      _currentDataPtr(0)
  {  
    /* Derived classes must call fetchNext() in their constructors themselves,
       XMLNodeIterator has no access to their virtual functions during 
       construction */
  }


  /* this is a trivial iterator over only one element
     needed e.g. for postinc (iter++) */
  XMLNodeIterator(const ENTRYTYPE &entry)
    : _input(0), _reader(0), _currentDataPtr(& entry)
  { }
    
  /* this is the final NodeIterator returned by last() */
  XMLNodeIterator() 
    : _input(0), _reader(0), _currentDataPtr(0)
  { }
      
  virtual ~XMLNodeIterator() 
  {
    if (_reader != 0)
      xmlFreeTextReader(_reader);
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

  ENTRYTYPE & 
  operator*() const 
  {
    assert (! atEnd());
    return *(_currentDataPtr.get());
  }

  ENTRYTYPE * 
  operator()() const 
  {
    return (_currentDataPtr.get());
  }

  XMLNodeIterator<ENTRYTYPE> &  /* ++iter */
  operator++() {
    assert (_reader && !atEnd());
    fetchNext();
    return *this;
  }

  XMLNodeIterator operator++(int)   /* iter++ */
  {
    assert (!atEnd());
    XMLNodeIterator<ENTRYTYPE> tmp(*currentDataPtr);
    fetchNext();
    return tmp;
  }

  const ENTRYTYPE * 
  operator->()
  {
    assert(! atEnd());
    return _currentDataPtr.get();
  }

protected:

  /* which nodes result in an ENTRYTYPE? */
  virtual bool 
  isInterested(const xmlNodePtr nodePtr) = 0;
  
  /* convert a node to ENTRYTYPE */
  virtual ENTRYTYPE 
  process(const xmlTextReaderPtr readerPtr) = 0;
  
  void fetchNext()
  {
    int status;
    if (_reader == 0) {
      /* this is a trivial iterator over (max) only one element,
         and we reach the end now. */
      _currentDataPtr.reset();
    }
    else {
      /* repeat as long as we successfully read nodes
         breaks out an interesting node has been found */
      while ((status = xmlTextReaderRead(_reader))==1) {
        xmlNodePtr node = xmlTextReaderCurrentNode(_reader);
        if (isInterested(node)) {
          // xmlDebugDumpNode(stdout,node,5);
          _currentDataPtr.reset(new ENTRYTYPE(process(_reader)));
          status = xmlTextReaderNext(_reader);
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

private:

  /* forbid assignment: We can't copy an xmlTextReader */
  XMLNodeIterator<ENTRYTYPE> & operator=(const XMLNodeIterator<ENTRYTYPE>& otherNode);

  /* forbid copy constructor: We can't copy an xmlTextReader */
  XMLNodeIterator<ENTRYTYPE>(const XMLNodeIterator<ENTRYTYPE>& otherNode);

  std::istream* _input;
  xmlTextReaderPtr _reader;
  std::auto_ptr<ENTRYTYPE> _currentDataPtr;
 
}; /* end class XMLNodeIterator */


#endif







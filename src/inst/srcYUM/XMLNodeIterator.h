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
  FIXME: Create a common subclass to reduce template bloat
         Put the internal functions separate.

*/

#ifndef XMLNodeIterator_h
#define XMLNodeIterator_h

#include <libxml/xmlreader.h>
#include <LibXMLHelper.h>
// #include <libxml/debugXML.h>
#include <iostream>
#include <sstream>
#include <cassert>
#include <iterator>
#include <y2util/Y2SLog.h>
#include <y2util/Exception.h>



namespace{
  /**
   * Internal function to read from the input stream.
   * This feeds the xmlTextReader used in the XMLNodeIterator.
   * @param context points to the istream to read from
   * @param buffer is to be filled with what's been read
   * @param bufferLen max memory bytes to read
   * @return 
   */
  int ioread(void *context,
             char *buffer,
             int bufferLen)
  {
  assert(buffer);
  std::istream *streamPtr = (std::istream *) context;
  assert(streamPtr);
  streamPtr->read(buffer,bufferLen);
  return streamPtr->gcount();
}

  /**
   * Internal function to finish reading.
   * This is required by the xmlTextReader API, but
   * not needed since the stream will be created when
   * the istream object vanishes.
   * @param context points to the istream to read from
   * @return 0 on success.
   */
  int ioclose(void * context)
  {
  /* don't close. destructor will take care. */
  return 0;
  }
}


/**
 * @short Exception class for syntax errors in XMLNodeIterator.
 */
class XMLParserError : public Exception {
public:
  /**
   * Constructor
   */
  XMLParserError(const char *msg,
                 xmlParserSeverities severity,
                 xmlTextReaderLocatorPtr locator,
                 int docLine,
                 int docColumn)
    throw()
    : Exception(msg), _severity(severity), _locator(locator),
    _docLine(docLine), _docColumn(docColumn)
  { }

  ~XMLParserError() throw()
  { }

  /**
   * The severity of this error
   */
  xmlParserSeverities severity() const throw()
  { return _severity; }

  /**
   * See libxml2 documentation
   */
  xmlTextReaderLocatorPtr locator() const throw()
  { return _locator; }

  /**
   * The line number in the xml document where the error occurred.
   */
  int docLine() const throw()
  { return _docLine; }

  /**
   * The column number in the xml document where the error occurred.
   */
  int docColumn() const throw()
  { return _docColumn; }

  /**
   * Gives a string describing the position in the xml document.
   * (either empty, or "at line ..., column ...")
   **/
  std::string position() const throw()
  {
    if (_docLine!=-1 && _docLine!=-1) {
      std::stringstream<char> strm;
      strm << "at line " << _docLine
        <<", column " << _docColumn;
      return strm.str();
    }
    else
      return "";
  }

    

private:
  xmlParserSeverities _severity;
  xmlTextReaderLocatorPtr _locator;
  int _docLine;
  int _docColumn;
};



/**
 *
 * @short Abstract class to iterate over an xml stream
 *
 * Derive from XMLNodeIterator<ENTRYTYPE> to get an iterator
 * that returns ENTRYTYPE objects. A derived class must provide
 * isInterested() and process(). It should also provide a
 * Constructor Derived(std::stream,std::string baseUrl) which
 * must call fetchNext().
 *
 * The derived iterator class should be compatible with an stl
 * input iterator. Use like this:
 *
 * for (Iterator iter(anIstream, baseUrl),
 *      iter != Iterator.end(),     // or: iter() != 0, or ! iter.atEnd()
 *      ++iter) {
 *    doSomething(*iter)
 * }
 *
 * The iterator owns the pointer (i.e., caller must not delete it)
 * until the next ++ operator is called. At this time, it will be
 * destroyed (and a new ENTRYTYPE is created.)
 *
 * If the input is fundamentally flawed so that it makes no sense to
 * continue parsing, the functions that access the current item
 * will throw an XMLParserError.
 */

 

template <class ENTRYTYPE>
class XMLNodeIterator : public std::iterator<std::input_iterator_tag, ENTRYTYPE> {
public:
  /**
   * Constructor. Derived classes must call fetchNext() here.
   * @param input is the input stream (contains the xml stuff)
   * @param baseUrl is the base URL of the xml document
   */
  XMLNodeIterator(std::istream &input, 
                  const std::string &baseUrl,
                  const char *validationPath = 0)
    : _error(0),
      _input(& input),
      _reader(xmlReaderForIO(ioread, ioclose, _input, baseUrl.c_str(), "utf-8",0)),
      _currentDataPtr(0)
  {
    xmlTextReaderSetErrorHandler(_reader, errorHandler, this);
    if (_reader && validationPath)
      if (xmlTextReaderRelaxNGValidate
          (_reader,validationPath)==-1)
            WAR << "Could not enable validation of repomd document" << endl;
            
    /* Derived classes must call fetchNext() in their constructors themselves,
       XMLNodeIterator has no access to their virtual functions during 
       construction */
  }


  /* this is a trivial iterator over only one element
     needed e.g. for postinc (iter++) */
  /**
   * Constructor for a trivial iterator.
   * A trivial iterator contains only one element.
   * This is at least needed internally for the 
   * postinc (iter++) operator
   * @param entry is the one and only element of this iterator.
   */
  XMLNodeIterator(const ENTRYTYPE &entry)
    : _error(0), _input(0), _reader(0), _currentDataPtr(& entry)
  { }
    
  /**
   * Constructor for an empty iterator.
   * An empty iterator is already at its end.
   * This is what end() returns ...
   */
  XMLNodeIterator() 
    : _error(0), _input(0), _reader(0), _currentDataPtr(0)
  { }
      
  /**
   * Destructor
   */
  virtual ~XMLNodeIterator() 
  {
    if (_reader != 0)
      xmlFreeTextReader(_reader);
  }

  /**
   * This is the end.
   * @return an iterator that has reached the end.
   */
  static XMLNodeIterator<ENTRYTYPE> 
  end()
  {
    return XMLNodeIterator<ENTRYTYPE>();
  }

  /**
   * Have we reached the end?
   * @return whether the end has been reached. 
   */
  bool atEnd() const
  {
    return (_error.get() == 0
            && _currentDataPtr.get() == 0);
  }

  /**
   * Two iterators are equal if both are at the end
   * or if they are identical.
   * Since you cannot copy an XMLNodeIterator, everything
   * else is not equal.
   * @param other the other iterator
   * @return true if equal
   */
  bool 
  operator==(const XMLNodeIterator<ENTRYTYPE> &other) const 
  {
    if (atEnd())
      return other.atEnd();
    else 
      return this != & other;
  }

  /**
   * Opposit of operator==
   * @param other the other iterator
   * @return true if not equal
   */
  bool 
  operator!=(const XMLNodeIterator<ENTRYTYPE> &otherNode) const 
  {
    return ! operator==(otherNode);
  }

  /**
   * Fetch a pointer to the current element
   * @return pointer to the current element.
   */
  ENTRYTYPE & 
  operator*() const 
  {
    assert (! atEnd());
    checkError();
    return *(_currentDataPtr.get());
  }

  /**
   * Fetch the current element
   * @return the current element
   */
  ENTRYTYPE * 
  operator()() const 
  {
    checkError();
    return (_currentDataPtr.get());
  }

  /**
   * Go to the next element and return it
   * @return the next element
   */
  XMLNodeIterator<ENTRYTYPE> &  /* ++iter */
  operator++() {
    assert (_reader && !atEnd());
    fetchNext();
    checkError();
    return *this;
  }

  /**
   * remember the current element, go to next and return remembered one.
   * avoid this, usually you need the preinc operator (++iter)
   * This function may throw ParserError if something is fundamentally wrong
   * with the input.
   * @return the current element
   */
  XMLNodeIterator operator++(int)   /* iter++ */
  {
    assert (!atEnd());
    checkError();
    XMLNodeIterator<ENTRYTYPE> tmp(*_currentDataPtr);
    fetchNext();
    return tmp;
  }

  /**
   * similar to operator*, allows direct member access
   * @return pointer to current element
   */
  const ENTRYTYPE * 
  operator->()
  {
    assert(! atEnd());
    checkError();
    return _currentDataPtr.get();
  }

protected:

  /**
   * filter for the xml nodes
   * The derived class decides which xml nodes it is actually interested in.
   * For each that is selected, process() will be called an the resulting ENTRYTYPE
   * object used as the next value for the iterator.
   * Documentation for the node structure can be found in the libxml2 documentation.
   * Have a look at LibXMLHelper to access node attributes and contents.
   * @param nodePtr points to the xml node in question. Only the node is available, not the subtree.
   *                See libxml2 documentation.
   * @return true if interested
   */
  virtual bool
  isInterested(const xmlNodePtr nodePtr) = 0;
  
  /**
   * process an xml node
   * The derived class has to produce the ENTRYTYPE object here.
   * Details about the xml reader is in the libxml2 documentation.
   * You'll most probably want to use xmlTextReaderExpand(reader) to
   * request the full subtree, and then use the links in the resulting 
   * node structure to traverse, and class LibXMLHelper to access the 
   * attributes and element contents.
   * fetchNext() cannot throw an error since it will be called in the constructor.
   * Instead, in case of a fundamental syntax error the error is saved
   * and will be thrown with the next checkError().
   * @param readerPtr points to the xmlTextReader that reads the xml stream.
   * @return 
   */
  virtual ENTRYTYPE 
  process(const xmlTextReaderPtr readerPtr) = 0;
  
  /**
   * Fetch the next element and save it in _currentDataPtr
   */
  void fetchNext()
  {
    int status;
    /* throw away the old entry */
    _currentDataPtr.reset();
    if (_reader == 0) {
      /* this is a trivial iterator over (max) only one element,
         and we reach the end now. */
      ;
    }
    else {
      /* repeat as long as we successfully read nodes
         breaks out when an interesting node has been found */
      while ((status = xmlTextReaderRead(_reader))==1) {
        xmlNodePtr node = xmlTextReaderCurrentNode(_reader);
        if (isInterested(node)) {
          // xmlDebugDumpNode(stdout,node,5);
          _currentDataPtr.reset(new ENTRYTYPE(process(_reader)));
          status = xmlTextReaderNext(_reader);
          break;
        }
      }
      if (status == -1) {  // error occured
        if (_error.get() == 0) {
          errorHandler(this, "Unknown error while parsing xml file\n",
                       XML_PARSER_SEVERITY_ERROR, 0);
        }
        /* next checkError will throw this error */
      }
    }
  }

  /**
   * Internal function to set the _error variable
   * in case of a parser error. It logs the message
   * and saves errors in _error, so that they will
   * be thrown by checkError().
   * @param arg set to this with xmlReaderSetErrorHandler()
   * @param msg the error message
   * @param severity the severity
   * @param locator as defined by libxml2
   */
  static void
  errorHandler(void * arg,
               const char * msg,
               xmlParserSeverities severity,
               xmlTextReaderLocatorPtr locator)
  {
    XMLNodeIterator<ENTRYTYPE> *obj;
    obj = (XMLNodeIterator<ENTRYTYPE>*) arg;
    assert(obj);
    xmlTextReaderPtr reader = obj->_reader;
    const char *errOrWarn = (severity & XML_PARSER_SEVERITY_ERROR) ? "error" : "warning";
    std::ostream& out = (severity & XML_PARSER_SEVERITY_ERROR) ? ERR : WAR;
    
    /* Log it */
    out << "XML syntax " << errOrWarn << ": " << msg;
    if (obj->_error.get()) {
      out << "(encountered during error recovery!)" << endl;
    }
    if (reader)
      out  << "at line " << xmlTextReaderGetParserLineNumber(reader)
      << ", column " << xmlTextReaderGetParserColumnNumber(reader);
    out << endl;

    /* save it */
    if ((severity & XML_PARSER_SEVERITY_ERROR)
        && ! obj->_error.get()) {
      if (reader)
        obj->_error.reset(new XMLParserError
                          (msg, severity,locator,
                           xmlTextReaderGetParserLineNumber(reader),
                           xmlTextReaderGetParserColumnNumber(reader)));
      else
        obj->_error.reset(new XMLParserError
                          (msg, severity, locator,
                           -1, -1));
      obj->_error->setLocation(SOURCECODELOCATION);
    }
  }



  /**
   * check for pending error. Throw it.
   */
  virtual void checkError() const
  {
    if (_error.get() != 0)
      throw *_error;
  }

  /**
   * contains the latest error
   **/
  std::auto_ptr<XMLParserError> _error;

private:

  /**
   * assignment is forbidden.
   * Reason: We can't copy an xmlTextReader
   */
  XMLNodeIterator<ENTRYTYPE> & operator=(const XMLNodeIterator<ENTRYTYPE>& otherNode);

  /**
   * copy constructor is forbidden.
   * Reason: We can't copy an xmlTextReader
   * 
   * @param otherNode 
   * @return 
   */
  XMLNodeIterator<ENTRYTYPE>(const XMLNodeIterator<ENTRYTYPE>& otherNode);

  /**
   * contains the istream to read the xml file from.
   * Can be 0 if at end or if _currentDataPtr is the only element left.
   **/
  std::istream* _input;
  
  /**
   * contains the xmlTextReader used to parse the xml file.
   **/
  xmlTextReaderPtr _reader;
  
  /**
   * contains the current element of the iterator.
   * a pointer is used to be able to handle non-assigneable ENTRYTYPEs.
   * The iterator owns the element until the next ++ operation.
   * It can be 0 when the end has been reached.
   **/
  std::auto_ptr<ENTRYTYPE> _currentDataPtr; 
}; /* end class XMLNodeIterator */


#endif







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

  File:       binHeader.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef binHeader_h
#define binHeader_h

extern "C" {
#include <stdint.h>
}

#include <iosfwd>
#include <string>
#include <list>

#include <y2pm/binHeaderPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : binHeader
/**
 *
 **/
class binHeader : public CountedRep {
  REP_BODY(binHeader);

  public:

    typedef int32_t tag;

    typedef struct headerToken * Header;

    class intList;

    class stringList;

  private:

    Header _h;

    bool assertHeader();

  public:

    binHeader( Header h_r = 0 );

    /**
     * <B>Dangerous!<\B> This one takes the header out of rhs
     * and leaves rhs empty.
     **/
    binHeader( binHeaderPtr & rhs );

    virtual ~binHeader();

  public:

    bool empty() const { return( _h == NULL ); }

    bool has_tag( tag tag_r ) const;

    unsigned int_list( tag tag_r, intList & lst_r ) const;

    unsigned string_list( tag tag_r, stringList & lst_r ) const;

    int int_val( tag tag_r ) const;

    std::string string_val( tag tag_r ) const;

  public:

    std::list<std::string> stringList_val( tag tag_r ) const;

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : binHeader::intList
/**
 *
 **/
class binHeader::intList {
  intList            ( const intList & );
  intList & operator=( const intList & );
  private:
    unsigned cnt;
    void *   val;
    tag      type;
  private:
    friend class binHeader;
    unsigned set( void * val_r, tag cnt_r, tag type_r );
  public:
    intList();
    unsigned size() const { return cnt; }
    int operator[]( const unsigned idx_r ) const;
};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : binHeader::stringList
/**
 *
 **/
class binHeader::stringList {
  stringList            ( const stringList & );
  stringList & operator=( const stringList & );
  private:
    unsigned cnt;
    char **  val;
    void clear();
  private:
    friend class binHeader;
    unsigned set( char ** val_r, tag cnt_r );
  public:
    stringList();
    ~stringList() { clear(); }
    unsigned size() const { return cnt; }
    std::string operator[]( const unsigned idx_r ) const;
};

///////////////////////////////////////////////////////////////////

#endif // binHeader_h

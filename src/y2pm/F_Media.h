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

  File:       F_Media.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef F_Media_h
#define F_Media_h

#include <iosfwd>
#include <string>

#include <y2util/Vendor.h>
#include <y2util/Pathname.h>

#include <y2pm/PMError.h>
#include <y2pm/PMLangCode.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : F_Media
/**
 *
 **/
class F_Media {

  friend std::ostream & operator<<( std::ostream & str, const F_Media & obj );

  public:

    enum Flag {
      DOUBLESIDED = 0x01
    };

    typedef std::map<unsigned,PM::LocaleString> LabelMap;

    static const PM::LocaleString _noLabel;

  private:

    Vendor       _vendor;
    std::string  _ident;
    unsigned     _count;
    unsigned     _flags;
    LabelMap     _labels;

  public:

    F_Media();
    ~F_Media();

    const Vendor & vendor() const { return _vendor; }
    const std::string & ident() const { return _ident; }
    unsigned count() const { return _count; }

    bool doublesided() const { return( _flags & DOUBLESIDED ); }

    const LabelMap & labels() const { return _labels; }

    const PM::LocaleString & label( unsigned number_r ) const {
      LabelMap::const_iterator found( _labels.find( number_r ) );
      return( found == _labels.end() ? _noLabel : found->second );
    }

  public:

   void reset() { *this = F_Media(); }

   PMError read( std::istream & stream_r, const bool quick_r = false );
   PMError read( const Pathname & file_r, const bool quick_r = false  );

   PMError quickRead( std::istream & stream_r ) { return read( stream_r, /*quick*/true ); }
   PMError quickread( const Pathname & file_r ) { return read( file_r, /*quick*/true ); }
};

///////////////////////////////////////////////////////////////////

#endif // F_Media_h

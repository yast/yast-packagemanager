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

  File:       F_Medianames.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef F_Medianames_h
#define F_Medianames_h

#include <iosfwd>
#include <string>
#include <map>

#include <y2util/LangCode.h>
#include <y2util/Pathname.h>

#include <y2pm/PMError.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : F_Medianames
/**
 *
 **/
class F_Medianames {

  friend std::ostream & operator<<( std::ostream & str, const F_Medianames & obj );

  public:

    typedef std::map<LangCode,std::string> LangString;
    typedef std::map<unsigned,LangString>  LabelMap;

  private:

    static const std::string _noLabel;

    LabelMap _labels;

  public:

    F_Medianames();
    ~F_Medianames();

    const LabelMap & labels() const { return _labels; }

    const std::string & label( unsigned number_r ) const;

  public:

   PMError read( std::istream & stream_r );
   PMError read( const Pathname & file_r );
};

///////////////////////////////////////////////////////////////////

#endif // F_Medianames_h

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

   File:       PkgArch.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PkgArch_h
#define PkgArch_h

#include <iosfwd>

#include <y2util/UniqStr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgArch
/*
 *
 **/
class PkgArch {

  private:

    static UniqStr nameHash;
    const char *   name;

  public:

    PkgArch( const char * n = "" )   { name = nameHash.add( n ); }
    PkgArch( const std::string & s ) { name = nameHash.add( s.c_str() ); }

    bool operator==( const PkgArch & n2 ) const { return name == n2.name; }
    bool operator!=( const PkgArch & n2 ) const { return name != n2.name; }

    operator const char * () const { return name; }

    friend std::ostream & operator<<( std::ostream & str, const PkgArch & obj );
};

///////////////////////////////////////////////////////////////////

#endif // PkgArch_h

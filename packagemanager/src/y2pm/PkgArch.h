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

#include <y2util/Ustring.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgArch
/*
 *
 **/
class PkgArch : public Ustring {

  private:

    static UstringHash _nameHash;

  public:

    explicit PkgArch( const std::string & n = "" ) : Ustring( _nameHash, n ) {}

  public:

    /**
      Compare two PkgArch's according to Y2PM::allowedArchs.
     
      If \a lhs is smaller than \a rhs 1 is returned, if \a lhs is equal to \a
      rhs 0 is returned, if \a lhs is greater than \a rhs -1 is returned. If
      both architectures aren't contained in Y2PM::allowedArchs 0 is returned.
    **/
    static int compare( const PkgArch & lhs, const PkgArch & rhs );

    int compare( const PkgArch & rhs ) const { return compare( *this, rhs ); }
};

///////////////////////////////////////////////////////////////////

#endif // PkgArch_h

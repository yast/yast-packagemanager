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
     * Comapre two PkgArch's according to Y2PM::allowedArchs
     **/
    static int compare( const PkgArch & lhs, const PkgArch & rhs );

    int compare( const PkgArch & rhs ) const { return compare( *this, rhs ); }
};

///////////////////////////////////////////////////////////////////

#endif // PkgArch_h

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

  File:       PkgSplit.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef PkgSplit_h
#define PkgSplit_h

#include <iosfwd>
#include <string>
#include <set>
#include <functional>

#include <y2util/Pathname.h>

#include <y2pm/PkgName.h>

///////////////////////////////////////////////////////////////////
class PkgSplit;

typedef std::set<PkgSplit> PkgSplitSet;

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgSplit
/**
 * @short Represents information of splitprovides
 *
 * Syntax for splitprovides is: <CODE>pkgname<B>:/</B>absolute/path</CODE>
 *
 * A package that contains a splitprovides partially replaces an <b>installed<\b>
 * package <CODE>pkgname</CODE>, iff the installed <CODE>pkgname</CODE> contains
 * <CODE>/absolute/path</CODE> in it's filelist.
 *
 * The indicator file <CODE>/absolute/path</CODE> is one of the files that were moved
 * out of <CODE>pkgname</CODE> and into this package, as the package was slpitted.
 *
 * If an unsplitted version of <CODE>pkgname</CODE> is replaced (e.g. on update), this
 * package should be automatically set to install.
 **/
class PkgSplit {

  private:

    PkgName  _ipkg;
    Pathname _file;

  public:

    /**
     * Default constructor
     **/
    PkgSplit() {}

    /**
     * Construct from string <CODE>"pkgname:/absolute/path"</CODE>. If the string
     * does not form a valid splitprovides, an error is reported, unless quiet_r
     * is true. To form a valid splitprovides, the string must not contain whitespace
     * and neither pkgname nor path may be empty.
     **/
    PkgSplit( const std::string & splitprovides_r, const bool quiet_r = false );

    /**
     * Construct from PkgName and Pathname.
     **/
    PkgSplit( const PkgName & ipkg_r, const Pathname & file_r );

    ~PkgSplit();

    /**
     * Return true if neither PkgName nor Pathname are empty. Usefull in
     * conjunction with <CODE>PkgSplit( string, true )</CODE> in e.g. parser
     * to filter out splitprovides.
     **/
    bool valid() const { return ! ( _ipkg->empty() || _file.empty() ); }

    /**
     * Returns the name of the package that has to be installed.
     **/
    const PkgName & ipkg() const { return _ipkg; }

    /**
     * Returns the file that must be owned by the installed package.
     **/
    const Pathname & file() const { return _file; }

  public:

    /**
     * Stream output as 'ipkg:/file'
     **/
    friend std::ostream & operator<<( std::ostream & str, const PkgSplit & obj );
};

///////////////////////////////////////////////////////////////////

/**
 * Order to be used by associative std::container (set/map). Lexicographic by
 * ipkg, then file. <B>Do not change this ordering! We rely on it.</B>
 *
 * A matter of taste, as 'operator<' would do the same job. But I don't like it
 * in classes where it's meaning isn't obvious.
 **/
template<>
inline bool std::less<PkgSplit>::operator()(const PkgSplit & lhs, const PkgSplit & rhs ) const {
  int d = lhs.ipkg().asString().compare( rhs.ipkg().asString() );
  if ( d )
    return( d < 0 );
  return( lhs.file().asString() < rhs.file().asString() );
}

///////////////////////////////////////////////////////////////////

#endif // PkgSplit_h


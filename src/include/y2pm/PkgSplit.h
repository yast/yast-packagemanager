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

#include <y2util/Pathname.h>

#include <y2pm/PkgName.h>

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
     * is true.
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

    friend std::ostream & operator<<( std::ostream & str, const PkgSplit & obj );
};

///////////////////////////////////////////////////////////////////

#endif // PkgSplit_h

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

  File:       PkgIdent.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef PkgIdent_h
#define PkgIdent_h

#include <iosfwd>
#include <string>

#include <y2pm/PkgName.h>
#include <y2pm/PkgEdition.h>
#include <y2pm/PkgArch.h>
#include <y2pm/PMSolvablePtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgIdent
/**
 * @short Strore 'name-version-release.arch'
 **/
class PkgIdent {

  friend std::ostream & operator<<( std::ostream & str, const PkgIdent & obj );

  private:

    PkgName    _name;
    PkgEdition _edition;
    PkgArch    _arch;

  public:

    PkgIdent() {}

    PkgIdent( constPMSolvablePtr slv_r );

    PkgIdent( const PkgName & name_r, const PkgEdition & edition_r, const PkgArch & arch_r )
      : _name( name_r )
      , _edition( edition_r )
      , _arch( arch_r )
    {}

    virtual ~PkgIdent() {}

  public:

    const PkgName &     name()    const { return _name; }
    const PkgEdition &  edition() const { return _edition; }
    const std::string & version() const { return _edition.version(); }
    const std::string & release() const { return _edition.release(); }
    const PkgArch &     arch()    const { return _arch; }

    /**
     * Returns "name-version-release" string
     **/
    std::string nameEd() const { return _name.asString() + '-' + _edition.asString(); }

    /**
     * Returns "name-version-release.arch" string
     **/
    std::string nameEdArch() const { return nameEd() + '.' + _arch.asString(); }

  public:

    /**
     * Order to be used by associative std::container (set/map):
     * Lexicographic by _name, _edition then _arch.
     **/
    friend bool std::less<PkgIdent>::operator()( const PkgIdent & lhs, const PkgIdent & rhs ) const;
};

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : std::less<PkgIdent>::operator()
//	METHOD TYPE : bool
//
//      Order to be used by associative std::container (set/map):
//      Lexicographic by _name, _edition then _arch.
//
inline bool std::less<PkgIdent>::operator()( const PkgIdent & lhs, const PkgIdent & rhs ) const
{
  int r = lhs.name()->compare( rhs.name() );
  if ( r != 0 )
    return( r < 0 );
  r = lhs.version().compare( rhs.version() );
  if ( r != 0 )
    return( r < 0 );
  r = lhs.release().compare( rhs.release() );
  if ( r != 0 )
    return( r < 0 );
  return( lhs.arch()->compare( rhs.arch() ) < 0 );

}

///////////////////////////////////////////////////////////////////

#endif // PkgIdent_h

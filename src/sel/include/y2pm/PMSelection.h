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

  File:       PMSelection.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Defines the software Selection object.

/-*/
#ifndef PMSelection_h
#define PMSelection_h

#include <iosfwd>
#include <string>

#include <y2util/LangCode.h>

#include <y2pm/PMSelectionDataProviderPtr.h>
#include <y2pm/PMSelectionPtr.h>
#include <y2pm/PMPackagePtr.h>

#include <y2pm/PMObject.h>
#include <y2pm/PkgArch.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelection
/**
 * The software selection
 **/
class PMSelection : virtual public Rep, public PMObject {
  REP_BODY(PMSelection);

  public:

    /**
     * PMObject attributes that should be realized by each concrete Object.
     * @see PMObject
     **/
    std::string                    summary    ( const LangCode& locale ) const;
    std::list<std::string>         description( const LangCode& locale ) const;
    std::list<std::string>         insnotify  ( const LangCode& locale ) const;
    std::list<std::string>         delnotify  ( const LangCode& locale ) const;

    virtual std::string            summary()     const { return summary( LangCode("") ); }
    virtual std::list<std::string> description() const { return description( LangCode("") ); }
    virtual std::list<std::string> insnotify()   const { return insnotify( LangCode("") ); }
    virtual std::list<std::string> delnotify()   const { return delnotify( LangCode("") ); }
    virtual FSize                  size()        const;

    /**
     * PMSelection attributes passed off to PMSelectionDataProvider
     * @see PMSelectionDataProvider
     **/
    std::string			category()         const;
    bool			visible()          const;
    std::list<std::string>	suggests()         const;
    std::list<PMSelectionPtr>	suggests_ptrs();
    std::list<std::string>	recommends()       const;
    std::list<PMSelectionPtr>	recommends_ptrs();
    std::list<std::string>	inspacks     ( const LangCode& locale = LangCode("") ) const;
    std::list<PMSelectablePtr>	inspacks_ptrs( const LangCode& locale = LangCode("") );
    std::list<std::string>	delpacks     ( const LangCode& locale = LangCode("") ) const;
    std::list<PMSelectablePtr>	delpacks_ptrs( const LangCode& locale = LangCode("") );
    FSize			archivesize()      const;
    std::string			order()            const;

    const bool			isBase()           const;

  protected:

    /**
     * There's no public acctess to _dataProvider!
     * Just the controlling InstSrcData may access it.
     **/
    friend class InstSrcData;

    PMSelectionDataProviderPtr _dataProvider;

  public:

    PMSelection (const PkgName &    name_r,
		 const PkgEdition & edition_r,
		 const PkgArch &    arch_r,
		 PMSelectionDataProviderPtr dataProvider_r,
		 constInstSrcPtr source = 0);

    virtual ~PMSelection();

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMSelection_h

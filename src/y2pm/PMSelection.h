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

#include <y2pm/PMSelectionDataProvider.h> // should introduce all attribute data types.

#include <y2pm/PMSelectionPtr.h>
#include <y2pm/PMPackagePtr.h>

#include <y2pm/PMObject.h>
#include <y2pm/PkgArch.h>
#include <y2pm/PMTypes.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelection
/**
 * The software selection
 **/
class PMSelection : public PMObject {
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
    virtual bool                   providesSources() const;

    virtual std::string            instSrcLabel()    const;
    virtual Vendor                 instSrcVendor()   const;
    virtual unsigned               instSrcRank()     const;

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
    std::list<std::string>	delpacks     ( const LangCode& locale = LangCode("") ) const;
    FSize			archivesize()      const;
    std::string			order()            const;

    const bool			isBase()           const;

    /**
     * <code>PackageManager</code>s list of Selectables included in
     * this Selection for a given locale. This is probaeely <b>not</b>,
     * what you want. See <code>inspacks_ptrs()</code>.
     **/
    std::set<PMSelectablePtr>	inspacks_ptrs( const LangCode & locale );
    std::set<PMSelectablePtr>	inspacks_ptrs( const PM::LocaleSet & locales );
    /**
     * <code>PackageManager</code>s list of Selectables included in
     * this Selection for a given locale. This is probaeely <b>not</b>,
     * what you want. See <code>delpacks_ptrs()</code>.
     **/
    std::set<PMSelectablePtr>	delpacks_ptrs( const LangCode & locale );
    std::set<PMSelectablePtr>	delpacks_ptrs( const PM::LocaleSet & locales );


    // physical access to the sel file.
    PMError provideSelToInstall( Pathname & ) const;

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
		 PMSelectionDataProviderPtr dataProvider_r );

    virtual ~PMSelection();

    /**
     * <code>PackageManager</code>s list of Selectables included in
     * this Selection. This is the list of inspacks the SelectionManger
     * operates with.
     **/
    std::set<PMSelectablePtr>	inspacks_ptrs();

    /**
     * <code>PackageManager</code>s list of Selectables included in
     * this Selection. This is the list of delpacks the SelectionManger
     * operates with.
     **/
    std::set<PMSelectablePtr>	delpacks_ptrs();

  public:

    /**
     * Compare two Selections by order value, then by name.
     **/
    static int compareByOrder( constPMSelectionPtr lhs, constPMSelectionPtr rhs ) {
      if ( lhs == rhs )
	return 0;
      if ( ! ( lhs && rhs ) )
	return( rhs ? -1 : 1 );
      int res = lhs->order().compare( rhs->order() );
      if ( res )
	return res;
      return lhs->name()->compare( rhs->name() );
    }

    /**
     * Return whether compared by order lhs is less than rhs.
     **/
    static bool lessByOrder( constPMSelectionPtr lhs, constPMSelectionPtr rhs ) {
      return( compareByOrder( lhs, rhs ) < 0 );
    }

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMSelection_h

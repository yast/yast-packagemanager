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

  File:       PMSelectionDataProvider.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Common interface to be realized by all SelectionDataProvider.

/-*/
#ifndef PMSelectionDataProvider_h
#define PMSelectionDataProvider_h

#include <iosfwd>
#include <string>
#include <list>
#include <set>

#include <y2util/FSize.h>
#include <y2util/Vendor.h>
#include <y2util/LangCode.h>
#include <y2util/Pathname.h>

#include <y2pm/PMSelectablePtr.h>

#include <y2pm/PMError.h>
#include <y2pm/PMSelectionDataProviderPtr.h>
#include <y2pm/PMSelectionPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelectionDataProvider
/**
 * @short Common interface to be realized by all SelectionDataProvider.
 *
 * PMSelection attribute retrieval. The PMSelection this pointer is passed
 * in case the concrete DataProvider has do some lookup.
 *
 * Default values provided by static members
 *
 * @see DataProvider
 **/
class PMSelectionDataProvider : public CountedRep {
  REP_BODY(PMSelectionDataProvider);

  public:

    ///////////////////////////////////////////////////////////////////
    // default for unprovided values
    ///////////////////////////////////////////////////////////////////

    // PMObject attributes
    static std::string               summary    ( const LangCode& lang = LangCode("") ) { return std::string(); }
    static std::list<std::string>    description( const LangCode& lang = LangCode("") ) { return std::list<std::string>(); }
    static std::list<std::string>    insnotify  ( const LangCode& lang = LangCode("") ) { return std::list<std::string>(); }
    static std::list<std::string>    delnotify  ( const LangCode& lang = LangCode("") ) { return std::list<std::string>(); }
    static FSize                     size()            { return FSize(0); }
    static bool                      providesSources() { return false; }
    static std::string               instSrcLabel()    { return std::string(); }
    static Vendor                    instSrcVendor()   { return Vendor(); }
    static unsigned                  instSrcRank()     { return unsigned(-1); }

    // PMSelection attributes
    static std::string               category()        { return std::string(); }
    static bool                      visible()         { return true; }
    static std::list<std::string>    suggests()        { return std::list<std::string>(); }
    static std::list<PMSelectionPtr> suggests_ptrs()   { return std::list<PMSelectionPtr>(); }
    static std::list<std::string>    recommends()      { return std::list<std::string>(); }
    static std::list<PMSelectionPtr> recommends_ptrs() { return std::list<PMSelectionPtr>(); }
    static std::list<std::string>    inspacks     ( const LangCode& lang = LangCode("") ) { return std::list<std::string>(); }
    static std::list<std::string>    delpacks     ( const LangCode& lang = LangCode("") ) { return std::list<std::string>(); }

    // the per locale entry ( no default lang argument! )
    static std::set<PMSelectablePtr> inspacks_ptrs( const LangCode& lang ) { return std::set<PMSelectablePtr>(); }
    static std::set<PMSelectablePtr> delpacks_ptrs( const LangCode& lang ) { return std::set<PMSelectablePtr>(); }

    static FSize                     archivesize()     { return FSize(0); }
    static std::string               order()           { return "000"; }

    static bool                      isBase()          { return false; }

    // physical access to the sel file.
    static PMError provideSelToInstall( Pathname & path_r ) { path_r = Pathname(); return PMError::E_error; }

  protected:

    PMSelectionDataProvider();

    virtual ~PMSelectionDataProvider();

    /**
     * In case concrete SelectionDataProvider wants PMSelectionPtr for lookup.
     **/
    PMSelectionPtr mkPtr( const PMSelection & sel_r ) const { return const_cast<PMSelection*>(&sel_r); }

  public:

    ///////////////////////////////////////////////////////////////////
    // Overload values you can provide
    ///////////////////////////////////////////////////////////////////

    // PMObject attributes
    virtual std::string               summary        ( const PMSelection & sel_r, const LangCode& lang = LangCode("") ) const { return summary(); }
    virtual std::list<std::string>    description    ( const PMSelection & sel_r, const LangCode& lang = LangCode("") ) const { return description(); }
    virtual std::list<std::string>    insnotify      ( const PMSelection & sel_r, const LangCode& lang = LangCode("") ) const { return insnotify(); }
    virtual std::list<std::string>    delnotify      ( const PMSelection & sel_r, const LangCode& lang = LangCode("") ) const { return delnotify(); }
    virtual FSize                     size           ( const PMSelection & sel_r ) const { return size(); }
    virtual bool                      providesSources( const PMSelection & sel_r ) const { return providesSources(); }
    virtual std::string               instSrcLabel   ( const PMSelection & sel_r ) const { return instSrcLabel(); }
    virtual Vendor                    instSrcVendor  ( const PMSelection & sel_r ) const { return instSrcVendor(); }
    virtual unsigned                  instSrcRank    ( const PMSelection & sel_r ) const { return instSrcRank(); }

    // PMSelection attributes
    virtual std::string               category       ( const PMSelection & sel_r ) const { return category(); }
    virtual bool                      visible        ( const PMSelection & sel_r ) const { return visible(); }
    virtual std::list<std::string>    suggests       ( const PMSelection & sel_r ) const { return suggests(); }
    virtual std::list<PMSelectionPtr> suggests_ptrs  ( const PMSelection & sel_r ) const { return suggests_ptrs(); }
    virtual std::list<std::string>    recommends     ( const PMSelection & sel_r ) const { return recommends(); }
    virtual std::list<PMSelectionPtr> recommends_ptrs( const PMSelection & sel_r ) const { return recommends_ptrs(); }
    virtual std::list<std::string>    inspacks       ( const PMSelection & sel_r, const LangCode& lang = LangCode("") ) const { return inspacks(); }
    virtual std::list<std::string>    delpacks       ( const PMSelection & sel_r, const LangCode& lang = LangCode("") ) const { return delpacks(); }

    // the per locale entry ( no default lang argument! )
    virtual std::set<PMSelectablePtr> inspacks_ptrs  ( const PMSelection & sel_r, const LangCode& lang ) const { return inspacks_ptrs(lang); }
    virtual std::set<PMSelectablePtr> delpacks_ptrs  ( const PMSelection & sel_r, const LangCode& lang ) const { return delpacks_ptrs(lang); }

    virtual FSize                     archivesize    ( const PMSelection & sel_r ) const { return archivesize(); }
    virtual std::string               order          ( const PMSelection & sel_r ) const { return order(); }

    virtual bool                      isBase         ( const PMSelection & sel_r ) const { return isBase(); }

    // physical access to the sel file.
    virtual PMError provideSelToInstall( const PMSelection & sel_r, Pathname & path_r ) const { return provideSelToInstall( path_r ); }
};

///////////////////////////////////////////////////////////////////

#endif // PMSelectionDataProvider_h


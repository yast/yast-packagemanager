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

#include <y2util/FSize.h>
#include <y2util/LangCode.h>
#include <y2pm/PMSelectionDataProviderPtr.h>

#include <y2pm/PMSelection.h>

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
class PMSelectionDataProvider : virtual public Rep {
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
    static FSize                     size()            { return size(); }

    // PMSelection attributes
    static std::string               category()        { return std::string(); }
    static bool                      visible()         { return true; }
    static std::list<std::string>    suggests()        { return std::list<std::string>(); }
    static std::list<PMSelectionPtr> suggests_ptrs()   { return std::list<PMSelectionPtr>(); }
    static std::list<std::string>    recommends()      { return std::list<std::string>(); }
    static std::list<PMSelectionPtr> recommends_ptrs() { return std::list<PMSelectionPtr>(); }
    static std::list<std::string>    inspacks     ( const LangCode& lang = LangCode("") ) { return std::list<std::string>(); }
    static std::list<PMSelectablePtr>inspacks_ptrs( const LangCode& lang = LangCode("") ) { return std::list<PMSelectablePtr>(); }
    static std::list<std::string>    delpacks     ( const LangCode& lang = LangCode("") ) { return std::list<std::string>(); }
    static std::list<PMSelectablePtr>delpacks_ptrs( const LangCode& lang = LangCode("") ) { return std::list<PMSelectablePtr>(); }
    static FSize                     archivesize()     { return FSize(0); }
    static std::string               order()           { return "000"; }

    static bool                      isBase()          { return false; }

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

    // PMSelection attributes
    virtual std::string               category       ( const PMSelection & sel_r ) const { return category(); }
    virtual bool                      visible        ( const PMSelection & sel_r ) const { return visible(); }
    virtual std::list<std::string>    suggests       ( const PMSelection & sel_r ) const { return suggests(); }
    virtual std::list<PMSelectionPtr> suggests_ptrs  ( const PMSelection & sel_r ) const { return suggests_ptrs(); }
    virtual std::list<std::string>    recommends     ( const PMSelection & sel_r ) const { return recommends(); }
    virtual std::list<PMSelectionPtr> recommends_ptrs( const PMSelection & sel_r ) const { return recommends_ptrs(); }
    virtual std::list<std::string>    inspacks       ( const PMSelection & sel_r, const LangCode& lang = LangCode("") ) const { return inspacks(); }
    virtual std::list<PMSelectablePtr>inspacks_ptrs  ( const PMSelection & sel_r, const LangCode& lang = LangCode("") ) const { return inspacks_ptrs(); }
    virtual std::list<std::string>    delpacks       ( const PMSelection & sel_r, const LangCode& lang = LangCode("") ) const { return delpacks(); }
    virtual std::list<PMSelectablePtr>delpacks_ptrs  ( const PMSelection & sel_r, const LangCode& lang = LangCode("") ) const { return delpacks_ptrs(); }
    virtual FSize                     archivesize    ( const PMSelection & sel_r ) const { return archivesize(); }
    virtual std::string               order          ( const PMSelection & sel_r ) const { return order(); }

    virtual bool                      isBase         ( const PMSelection & sel_r ) const { return isBase(); }
};

///////////////////////////////////////////////////////////////////

#endif // PMSelectionDataProvider_h


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
     * Attributes provided by PMSelection
     **/
    enum PMSelectionAttribute {
      PMSEL_ATTR_BEGIN = PMOBJ_NUM_ATTRIBUTES,
      ATTR_CATEGORY = PMSEL_ATTR_BEGIN,
      ATTR_VISIBLE,
      ATTR_SUGGESTS,
      ATTR_RECOMMENDS,
      ATTR_INSPACKS,
      ATTR_DELPACKS,
      ATTR_ARCHIVESIZE,
      // last entry:
      PMSEL_NUM_ATTRIBUTES
    };

	// overlay virtual PMObject functions
	const std::string summary() const { return summary(""); }
	const std::string summary(const std::string& lang = "") const;
	const std::list<std::string> description() const { return description(""); }
	const std::list<std::string> description(const std::string& lang = "") const;
	const std::list<std::string> insnotify() const { return insnotify (""); }
	const std::list<std::string> insnotify(const std::string& lang = "") const;
	const std::list<std::string> delnotify() const { return delnotify (""); }
	const std::list<std::string> delnotify(const std::string& lang = "") const;
	const FSize size() const;

	/**
	 * access functions for PMSelection attributes
	 */

	const std::string category () const;
	const bool visible () const;
	const std::list<std::string> suggests() const;
	const std::list<PMSelectionPtr> suggests_ptrs() const;
	const std::list<std::string> recommends() const;
	const std::list<PMSelectionPtr> recommends_ptrs() const;
	const std::list<std::string> inspacks (const std::string& lang = "") const;
	const std::list<PMPackagePtr> inspacks_ptrs (const std::string& lang = "") const;
	const std::list<std::string> delpacks (const std::string& lang = "") const;
	const std::list<PMPackagePtr> delpacks_ptrs (const std::string& lang = "") const;
	const FSize archivesize() const;
	const std::string order() const;

	/**
	 * helper functions
	 */
	const bool isBase () const;

  protected:

    PMSelectionDataProviderPtr _dataProvider;

  protected:
    friend class ParseDataUL;
    /**
     * Provide DataProvider access to the underlying Object
     **/
    virtual PMDataProviderPtr dataProvider() const { return _dataProvider; }

  public:

    PMSelection( const PkgName &    name_r,
		 const PkgEdition & edition_r,
		 const PkgArch &    arch_r,
		 PMSelectionDataProviderPtr dataProvider_r );

    virtual ~PMSelection();

    void startRetrieval() const;
    void stopRetrieval() const;

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMSelection_h

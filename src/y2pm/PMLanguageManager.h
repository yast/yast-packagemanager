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

  File:       PMLanguageManager.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMLanguageManager_h
#define PMLanguageManager_h

#include <iosfwd>
#include <list>
#include <map>
#include <set>

#include <y2pm/PMError.h>
#include <y2pm/PMManager.h>
#include <y2pm/PMLanguage.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMLanguageManager
/**
 *
 **/
class PMLanguageManager : public PMManager {

  friend std::ostream & operator<<( std::ostream & str, const PMLanguageManager & obj );

  PMLanguageManager & operator=( const PMLanguageManager & );
  PMLanguageManager            ( const PMLanguageManager & );

  private:

    friend class Y2PM;
    PMLanguageManager();
    virtual ~PMLanguageManager();

  private:

    typedef std::map<LangCode,PMLanguagePtr> Objects;

    Objects _installedObjs;

    Objects _availableObjs;

  private:

    /**
     * Make sure the passed PMObjectPtr actually references a PMLanguage. If not,
     * report error and return NULL.
     **/
    virtual PMObjectPtr assertObjectType( const PMObjectPtr & object_r ) const;

    /**
     * Pre poolSetInstalled hook
     **/
    virtual void prePSI();
    /**
     * Post poolSetInstalled hook
     **/
    virtual void postPSI();

    /**
     * Receives trigger from Selectable on state change
     **/
    virtual void selectableNotify( constPMSelectablePtr item_r, SelState old_r, SelState new_r );

  public:

    virtual void rescan();

  public:

    typedef std::set<PMSelectablePtr> PkgSelectables;

    PkgSelectables getLangPackagesFor( LangCode langCode_r ) const;

    PkgSelectables getLangPackagesFor( constPMLanguagePtr lang_r ) const {
      if ( lang_r )
	return getLangPackagesFor( lang_r->langCode() );
      return PkgSelectables();
    }

    PkgSelectables getLangPackagesFor( constPMSelectablePtr lang_r ) const {
      if ( lang_r )
	return getLangPackagesFor( constPMLanguagePtr( lang_r->theObject() ) );
      return PkgSelectables();
    }

};

///////////////////////////////////////////////////////////////////

#endif // PMLanguageManager_h

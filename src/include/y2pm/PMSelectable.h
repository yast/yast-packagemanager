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

   File:       PMSelectable.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMSelectable_h
#define PMSelectable_h

#include <iosfwd>
#include <list>

#include <y2pm/PMSelectablePtr.h>

#include <y2pm/PkgName.h>
#include <y2pm/PMObjectPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelectable
/**
 *
 **/
class PMSelectable : virtual public Rep {
  REP_BODY(PMSelectable);

  public:

    typedef std::list<PMObjectPtr>  PMObjectList;

  private:

    friend class PMManager;

    PMManager * _manager;
    unsigned    _mgr_idx;

    PkgName _name;
    PMObjectPtr _installedObj;
    PMObjectPtr _candidateObj;
    PMObjectList _candidateList;

    enum Error {
      E_Ok = 0,
      E_Error
    };

    PMObjectList::iterator clistLookup( PMObjectPtr obj_r );

    Error setInstalledObj( PMObjectPtr obj_r );
    Error delInstalledObj();

    Error clistAdd( PMObjectPtr obj_r );
    Error clistDel( PMObjectPtr obj_r );
    Error clistClearAll();

    Error clearAll();

    bool isEmpty() const;

    void check() const;

  protected:

    PMSelectable();

    PMSelectable( const PkgName & name_r );

    virtual ~PMSelectable();

  public:

    static const unsigned no_mgr = (unsigned)-1;

    unsigned mgr_idx() const { return _mgr_idx; }

  public:

    /**
     * The common name of all Objects managed by this Selectable.
     **/
    const PkgName & name() const { return _name; }

    /**
     * If not NULL, the Object actually installed on the target system.
     *
     * Otherwise no Object with this name is currently installed on the
     * target system.
     **/
    PMObjectPtr installedObj() const { return _installedObj; }

    /**
     * If not NULL, Among all available Objects with this name (from any enabled
     * InstSrc), this is considered to be the best choice for an installation.
     *
     * Otherwise there is no Object with this name available on any (enabled)
     * InstSrc. The list of availableObjs is empty.
     **/
    PMObjectPtr candidateObj() const { return _candidateObj; }

    /**
     * Number of Objects with this name availavle on any enabled InstSrc. If the list
     * is not empty, one among these is choosen as candidate for an installation.
     **/
    unsigned availableObjs() const { return _candidateList.size(); }

    /**
     * Iterator for the list of available Objects.
     **/
    PMObjectList::const_iterator av_begin() const { return _candidateList.begin(); }

    /**
     * Iterator for the list of available Objects.
     **/
    PMObjectList::const_iterator av_end() const { return _candidateList.end(); }

    /**
     * Iterator for the list of available Objects.
     **/
    PMObjectList::const_reverse_iterator av_rbegin() const { return _candidateList.rbegin(); }

    /**
     * Iterator for the list of available Objects.
     **/
    PMObjectList::const_reverse_iterator av_rend() const { return _candidateList.rend(); }

  public:

    /**
     * Used by ostream::operator<< to print some debug lines
     **/
    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMSelectable_h


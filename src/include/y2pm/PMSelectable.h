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

#include <y2pm/SelState.h>
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
    SelState    _state;

    void _mgr_attach( PMManager * mgr_r, const unsigned idx_r );
    void _mgr_detach();

    void _attach_obj( PMObjectPtr & obj_r );
    void _detach_obj( PMObjectPtr & obj_r );

  private:

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

    Error setCandidateObj( PMObjectPtr obj_r );
    Error delCandidateObj();

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
     * PRELIMINARILY: For state queries.
     **/
    const SelState & state() const { return _state; }

    /**
     * PRELIMINARILY: For state manipulation.
     **/
    SelState & state() { return _state; }

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
     * One among all available Objects with this name (from any enabled InstSrc),
     * considered to be the best choice for an installation.
     *
     * Might NULL, if no available Object is appropriate.
     **/
    PMObjectPtr candidateObj() const { return _candidateObj; }

    /**
     * Number of Objects with this name availavle on any enabled InstSrc. If the list
     * is not empty, one among these might be choosen as candidate for an installation.
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
     * UI likes to have one among the Objects refered to here, whichs data are
     * shown per default.
     *
     * Reurns installedObj(). If not available candidateObj(). If not available
     * one out of availableObjs().
     **/
    PMObjectPtr theObject() const {
      if ( _installedObj )
	return _installedObj;
      if ( _candidateObj )
	return _candidateObj;
      if ( availableObjs() )
	return *av_begin();
      return PMObjectPtr();
    }

  public:

    /**
     * Used by ostream::operator<< to print some debug lines
     **/
    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMSelectable_h


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
#include <y2pm/PkgArch.h>
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
    SelState    _state;

    void _mgr_attach( PMManager * mgr_r );
    void _mgr_detach();

    void _attach_obj( PMObjectPtr & obj_r );
    void _detach_obj( PMObjectPtr & obj_r );

    enum Error {
      E_Ok = 0,
      E_Error
    };

    Error setInstalledObj( PMObjectPtr obj_r );
    Error delInstalledObj();

    Error clistAdd( PMObjectPtr obj_r );
    Error clistDel( PMObjectPtr obj_r );
    Error clistClearAll();

    Error clearAll();

    bool isEmpty() const;

    void check() const;

  private:

    PkgName _name;

    PMObjectPtr _installedObj;
    PMObjectPtr _candidateObj;
    PMObjectPtr _userCandidateObj;

    PMObjectList _candidateList;

    PMObjectList::iterator clistLookup( PMObjectPtr obj_r );

    void chooseCandidateObj();
    void clearCandidateObj();

  protected:

    PMSelectable();

    PMSelectable( const PkgName & name_r );

    virtual ~PMSelectable();

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
     * The one among all available Objects with this name (from any enabled InstSrc),
     * That could be actually installed.
     *
     * Might be NULL, if no available Object is appropriate or TABOO flag is set.
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
     * Returns installedObj(). If not available candidateObj(). If not available
     * one out of availableObjs().
     **/
    PMObjectPtr theObject() const {
      if ( _installedObj )
	return _installedObj;
      if ( _candidateObj )
	return _candidateObj;
      PMObjectPtr ret = bestCandidate();
      if ( ret )
	return ret;
      if ( availableObjs() )
	return *av_begin();
      return PMObjectPtr();
    }

    /**
     * Find candidate matching arch
     * May be NULL, if no candidate for given arch is available.
     **/
    PMObjectPtr archCandidate (const PkgArch& arch) const;

    /**
     * Best among the availableObjs() Determined by ranking.
     * May be NULL, if no available is better than the installed.
     **/
    PMObjectPtr autoCandidate() const;

    /**
     * One among the availableObjs() explicitly requested by user.
     **/
    PMObjectPtr userCandidate() const {
      // PRELIMINARILY:
      return _userCandidateObj;
    }

    /**
     * userCandidate() if not NULL. Otherwise autoCandidate().
     **/
    PMObjectPtr bestCandidate() const {
      PMObjectPtr ret = userCandidate();
      if ( ret )
	return ret;
      return autoCandidate();
    }

    /**
     * Set the userCandidate().
     **/
    bool setUserCandidate( const PMObjectPtr & obj_r );

    /**
     * Unset the userCandidate().
     **/
    bool clrUserCandidate() { return setUserCandidate( PMObjectPtr() ); }

  public:

    // Keep this order, it's used for sorting package lists!
    // Dangerous / noteworthy states are sorted first.
    //
    // sh@suse.de

    enum UI_Status {
      S_Taboo,               // hide candidateObj so it can't be installed. ( have no installedObj )
      // requested by user:
      S_Del,                 // delete  installedObj
      S_Update,              // install candidateObj ( have installedObj )
      S_Install,             // install candidateObj ( have no installedObj ) ( clears taboo )
      // not requested by user:
      S_AutoDel,             // delete  installedObj
      S_AutoUpdate,          // install candidateObj ( have installedObj )
      S_AutoInstall,         // install candidateObj ( have no installedObj )
      // no modification:
      S_KeepInstalled,       // no modification      ( have installedObj )
      S_NoInst,              // no modification      ( have no installedObj )
    };

    friend std::ostream & operator<<( std::ostream & str, UI_Status obj );

  private:

    /**
     * If doit is true, clears a TABOO flag, and
     * sets candidateObj to bestCandidate, if available.
     *

     * Anyway return whether there is (or would be) a
     * candidateObj available afterwards.
     **/
    bool clearTaboo( const bool doit );

    /**
     * Test or trigger status change according to doit.
     **/
    bool intern_set_status( const UI_Status state_r, const bool doit );

  public:

    /**
     * If possible, trigger action according to state_r.
     **/
    bool set_status( const UI_Status state_r );

    /**
     * Test whether set_status(state_r) would succseed..
     **/
    bool test_set_status( const UI_Status state_r ) { return intern_set_status( state_r, false ); }

    /**
     * Return the current ui_status (no flags returned)
     **/
    UI_Status status() const;

  public:

    /**
     * Set to neither install nor delete (keeps taboo)
     **/
    void setNothingSelected() { _state.user_unset( true ); }

    ///////////////////////////////////////////////////////////////////
    // public usable part of SelState
    ///////////////////////////////////////////////////////////////////

    /**
     * True if either installed or candidate object is present
     **/
    bool has_object()    const { return _state.has_object(); }

    /**
     * True if installed object is present
     **/
    bool has_installed() const { return _state.has_installed(); }

    /**
     * True if candidate object is present
     **/
    bool has_candidate() const { return _state.has_candidate(); }

    /**
     * True if installed and candidate object is present
     **/
    bool has_both_objects() const { return _state.has_both_objects(); }

    /**
     * True if installed object is present but no candidate.
     **/
    bool has_installed_only() const { return _state.has_installed_only(); }

    /**
     * True if candidate object is present but no installed.
     **/
    bool has_candidate_only() const { return _state.has_candidate_only(); }

  public:

    /**
     * True if either to delete or to install
     **/
    bool to_modify()     const { return _state.to_modify(); }

    /**
     * True if to delete
     **/
    bool to_delete()     const { return _state.to_delete(); }

    /**
     * True if to install
     **/
    bool to_install()    const { return _state.to_install(); }

  public:

    /**
     * True if modification was requested by user
     **/
    bool by_user()       const { return _state.by_user(); }

    /**
     * True if modification was requested by application
     **/
    bool by_appl()       const { return _state.by_appl(); }

    /**
     * True if modification was auto requested
     **/
    bool by_auto()       const { return _state.by_auto(); }

  public:

    /**
     * True if forbidden to install a candidate object.
     **/
    bool is_taboo()      const { return _state.is_taboo(); }

  public:

    /**
     * User request to clear state (neither delete nor install).
     **/
    bool user_unset() { return _state.user_unset( true ); }

    /**
     * User request to delete the installed object. Fails if no
     * installed object is present.
     **/
    bool user_set_delete() { return _state.user_set_delete( true ); }

    /**
     * User request to install the candidate object. Fails if no
     * candidate object is present, or taboo.
     **/
    bool user_set_install() { return _state.user_set_install( true ); }

  public:

    /**
     * Application request to clear state (neither delete nor install).
     * Fails if user requested modification.
     **/
    bool appl_unset() { return _state.appl_unset( true ); }

    /**
     * Application request to delete the installed object. Fails if no
     * installed object is present, or user requested install.
     **/
    bool appl_set_delete() { return _state.appl_set_delete( true ); }

    /**
     * Application request to install the candidate object. Fails if no
     * candidate object is present, or user requested delete, or taboo.
     **/
    bool appl_set_install() { return _state.appl_set_install( true ); }

  public:

    /**
     * Auto request to clear state (neither delete nor install).
     * Fails if user/appl requested modification.
     **/
    bool auto_unset() { return _state.auto_unset( true ); }

    /**
     * Auto request to install the candidate object. Fails if no
     * candidate object is present, or user/appl requested delete, or taboo.
     **/
    bool auto_set_install() { return _state.auto_set_install( true ); }

    /**
     * Auto request to delete the installed object. Fails if no
     * installed object is present, or user/appl requested 'install'.
     **/
    bool auto_set_delete() { return _state.auto_set_delete( true ); }

  public:

    /**
     * Used by ostream::operator<< to print some debug lines
     **/
    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMSelectable_h


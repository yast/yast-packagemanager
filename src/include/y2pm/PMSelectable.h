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

    class SavedState {
      public:
	SavedState() {}
      private:
	friend class PMManager;

	PMSelectablePtr _item;
	SelState        _state;
	PMObjectPtr     _userCandidateObj;

	SavedState( const PMSelectablePtr & item_r );
	bool mayReplay();
	bool replay();
    };

  private:

    friend class PMManager;
    friend class SavedState;

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

    void  _clearInstalledObj();
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
     * Might be NULL, if no available Object is appropriate.
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
      if ( _candidateObj )
	return _candidateObj;
      if ( _installedObj )
	return _installedObj;
      PMObjectPtr ret = bestCandidate();
      if ( ret )
	return ret;
      if ( availableObjs() )
	return *av_begin();
      return PMObjectPtr();
    }

    /**
     * Best among the availableObjs() Determined by ranking.
     * May be NULL, if no available is better than the installed.
     **/
    PMObjectPtr autoCandidate() const;

    /**
     * One among the availableObjs() explicitly requested by user.
     **/
    PMObjectPtr userCandidate() const {
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
      S_Protected,           // Keep this unmodified ( have installedObj && S_Protected )
      S_Taboo,               // Keep this unmodified ( have no installedObj && S_Taboo)
      // requested by user:
      S_Del,                 // delete  installedObj ( clears S_Protected if set )
      S_Update,              // install candidateObj ( have installedObj, clears S_Protected if set )
      S_Install,             // install candidateObj ( have no installedObj, clears S_Taboo if set )
      // not requested by user:
      S_AutoDel,             // delete  installedObj
      S_AutoUpdate,          // install candidateObj ( have installedObj )
      S_AutoInstall,         // install candidateObj ( have no installedObj )
      // no modification:
      S_KeepInstalled,       // no modification      ( have installedObj && !S_Protected, clears S_Protected if set )
      S_NoInst,              // no modification      ( have no installedObj && !S_Taboo, clears S_Taboo if set )
    };

    friend std::ostream & operator<<( std::ostream & str, UI_Status obj );

  private:

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
     * True if candidate object provides sources to install
     **/
    bool providesSources() const;

    /**
     * Set whether to install available sources.
     **/
    bool set_source_install( const bool install_r );

    /**
     * True if available sources would be installed.
     **/
    bool source_install() const { return _state.is_srcins(); }

  public:

    /**
     * Set to neither install nor delete (keeps taboo)
     **/
    void setNothingSelected() { _state.user_unset( true ); }

    /**
     * Downgrade condition. Returns true, iff both objects are present, and
     * installation of candidateObj would just replace or downgrade the installedObj.
     * <b>Always false for non package objects!</b>
     **/
    bool downgrade_condition() const;

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
     * True if no modification allowed by user.
     **/
    bool is_taboo()      const { return _state.is_taboo(); }

  public:

    /**
     * User request to clear state (neither delete nor install).
     * (keeps taboo)
     **/
    bool user_unset() { return _state.user_unset( true ); }

    /**
     * User request to delete the installed object. Fails if no
     * installed object is present (clears taboo).
     **/
    bool user_set_delete() { return _state.user_set_delete( true ); }

    /**
     * User request to install the candidate object. Fails if no
     * candidate object is present (clears taboo).
     **/
    bool user_set_install() { return _state.user_set_install( true ); }

    /**
     * No modification allowed by user.
     **/
    bool user_set_taboo() { return _state.user_set_taboo( true ); }

    /**
     * Clear taboo flag.
     **/
    bool user_clr_taboo() { return _state.user_clr_taboo( true ); }

  public:

    /**
     * Application request to clear state (neither delete nor install).
     * Fails if user requested modification.
     **/
    bool appl_unset() { return _state.appl_unset( true ); }

    /**
     * Application request to delete the installed object. Fails if no
     * installed object is present, or user requested install or taboo.
     **/
    bool appl_set_delete() { return _state.appl_set_delete( true ); }

    /**
     * Application request to install the candidate object. Fails if no
     * candidate object is present, or user requested delete or taboo.
     * <b>Does not check for downgrade_condition. Do not use it without need.</b>
     **/
    bool appl_force_install() { return _state.appl_set_install( true ); }

    /**
     * Application request to install the candidate object. Fails if no
     * candidate object is present, or user requested delete or taboo.
     * Fails if downgrade_condition is true.
     **/
    bool appl_set_install() { return !downgrade_condition() && appl_force_install(); }

  public:

    /**
     * Auto request to clear state (neither delete nor install).
     * Fails if user/appl requested modification.
     **/
    bool auto_unset() { return _state.auto_unset( true ); }

    /**
     * Auto request to delete the installed object. Fails if no
     * installed object is present, or user/appl requested install or taboo.
     **/
    bool auto_set_delete() { return _state.auto_set_delete( true ); }

    /**
     * Auto request to install the candidate object. Fails if no
     * candidate object is present, or user/appl requested delete or taboo.
     * <b>Does not check for downgrade_condition. Do not use it without need.</b>
     **/
    bool auto_force_install() { return _state.auto_set_install( true ); }

    /**
     * Auto request to install the candidate object. Fails if no
     * candidate object is present, or user/appl requested delete or taboo.
     * Fails if downgrade_condition is true.
     **/
    bool auto_set_install() { return !downgrade_condition() && auto_force_install(); }

  public:

    /**
     * Used by ostream::operator<< to print some debug lines
     **/
    virtual std::ostream & dumpOn( std::ostream & str ) const;

    /**
     * print some debug lines
     **/
    std::ostream & dumpStateOn( std::ostream & str ) const;

  public:

    /**
     * Test method like 'by_user()'
     **/
    typedef bool (PMSelectable::*Test_method)() const;

    /**
     * General test function
     **/
    typedef bool (*Test_fnc)( const constPMSelectablePtr & sel_r );

    /**
     * Evaluates a Test_method
     **/
    static bool test( const constPMSelectablePtr & sel_r, Test_method fnc_r ) {
      if ( sel_r && fnc_r )
	return (sel_r.operator->()->*fnc_r)();
      return false;
    }

};

///////////////////////////////////////////////////////////////////

#endif // PMSelectable_h


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

  File:       SelState.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Bits representing a PMSelectables state

/-*/
#ifndef SelState_h
#define SelState_h

#include <stdint.h>

#include <iosfwd>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : SelState
/**
 * @short Bits representing a PMSelectables state
 *
 * <b>Note:</b> taboo locks the state to <code>no modification</code>. This applies
 * to all modification reqests except <code>user_set_delete</code>, <code>user_set_install</code>
 * and <code>user_clr_taboo</code>, which perforn and unset taboo. <code>user_unset</code>
 * keeps taboo.
 **/
class SelState {

  private:

    /**
     * Bits representing a PMSelectables state
     **/
    typedef uint8_t bits;

  private:

    static const bits B_IS_I     = 0x01; // installed object present
    static const bits B_IS_C     = 0x02; // candidate object present

    // next two are mutual exclusive, both unset means no modification
    static const bits B_TO_DEL   = 0x04; // request to delete installed object
    static const bits B_TO_INS   = 0x08; // request to install candidate object

    // next two are mutual exclusive, both unset means requested by auto (Solver)
    static const bits B_BY_USER  = 0x10; // modification requested by user
    static const bits B_BY_APPL  = 0x20; // modification requested by application

    static const bits B_F_TABOO  = 0x40; // no modification allowed by user

    static const bits B_F_SRCINS = 0x80; // Installation of sources desired


    static const bits M_IS = B_IS_I | B_IS_C;

    static const bits M_TO = B_TO_DEL | B_TO_INS;

    static const bits M_BY = B_BY_USER | B_BY_APPL;

  private:

    /**
     * bits representing a PMSelectables state
     **/
    bits _bits;

    /**
     * set bits in mask
     **/
    void set( const bits mask_r ) { _bits |= mask_r; }
    /**
     * clear bits in mask_
     **/
    void clr( const bits mask_r ) { _bits &= ~mask_r; }

  public:

    SelState();
    ~SelState();

  public:

    /**
     * Set whether an installed object is present
     **/
    void set_has_installed( bool b = true );

    /**
     * Set whether a candidate object is present
     **/
    void set_has_candidate( bool b = true );

  public:

    /**
     * True if either installed or candidate object is present
     **/
    bool has_object()    const { return( _bits & M_IS ); }

    /**
     * True if installed object is present
     **/
    bool has_installed() const { return( _bits & B_IS_I ); }

    /**
     * True if candidate object is present
     **/
    bool has_candidate() const { return( _bits & B_IS_C ); }

    /**
     * True if installed and candidate object is present
     **/
    bool has_both_objects() const { return( _bits & M_IS ) == M_IS; }

    /**
     * True if installed object is present but no candidate.
     **/
    bool has_installed_only() const { return( _bits & M_IS ) == B_IS_I; }

    /**
     * True if candidate object is present but no installed.
     **/
    bool has_candidate_only() const { return( _bits & M_IS ) == B_IS_C; }

  public:

    /**
     * True if either to delete or to install
     **/
    bool to_modify()     const { return( _bits & M_TO ); }

    /**
     * True if to delete
     **/
    bool to_delete()     const { return( _bits & B_TO_DEL ); }

    /**
     * True if to install
     **/
    bool to_install()    const { return( _bits & B_TO_INS ); }

  public:

    /**
     * True if modification was requested by user
     **/
    bool by_user()       const { return( _bits & B_BY_USER ); }

    /**
     * True if modification was requested by application
     **/
    bool by_appl()       const { return( _bits & B_BY_APPL ); }

    /**
     * True if modification was auto requested
     **/
    bool by_auto()       const { return( !( _bits & M_BY ) && to_modify() ); }

    /**
     * True if no modification allowed by user.
     **/
    bool is_taboo()      const { return( _bits & B_F_TABOO ); }

    /**
     * True if install sources flag is set.
     **/
    bool is_srcins()     const { return( _bits & B_F_SRCINS ); }

  public:

    /**
     * User request to clear state (neither delete nor install)
     * (keeps taboo).
     **/
    bool user_unset( const bool doit );

    /**
     * User request to delete the installed object. Fails if no
     * installed object is present (clears taboo).
     **/
    bool user_set_delete( const bool doit );

    /**
     * User request to install the candidate object. Fails if no
     * candidate object is present (clears taboo).
     **/
    bool user_set_install( const bool doit );

    /**
     * No modification allowed by user.
     **/
    bool user_set_taboo( const bool doit );

    /**
     * Clear taboo flag.
     **/
    bool user_clr_taboo( const bool doit );

    /**
     * Set install sources flag.
     **/
    bool user_set_srcins( const bool doit );

    /**
     * Clear install sources flag.
     **/
    bool user_clr_srcins( const bool doit );

  public:

    /**
     * Application request to clear state (neither delete nor install).
     * Fails if user requested modification.
     **/
    bool appl_unset( const bool doit );

    /**
     * Application request to delete the installed object. Fails if no
     * installed object is present, or user requested install or taboo.
     **/
    bool appl_set_delete( const bool doit );

    /**
     * Application request to install the candidate object. Fails if no
     * candidate object is present, or user requested delete or taboo.
     **/
    bool appl_set_install( const bool doit );

  public:

    /**
     * Auto request to clear state (neither delete nor install).
     * Fails if user/appl requested modification.
     **/
    bool auto_unset( const bool doit );

    /**
     * Auto request to delete the installed object. Fails if no
     * installed object is present, or user/appl requested install or taboo.
     **/
    bool auto_set_delete( const bool doit );

    /**
     * Auto request to install the candidate object. Fails if no
     * candidate object is present, or user/appl requested delete or taboo.
     **/
    bool auto_set_install( const bool doit );

  public:

    friend std::ostream & operator<<( std::ostream & str, const SelState & obj );
};

///////////////////////////////////////////////////////////////////

#endif // SelState_h

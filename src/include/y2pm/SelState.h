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
 **/
class SelState {

  private:

    typedef uint8_t bits;

  private:

    static const bits B_IS_I     = 0x01; // installed object present
    static const bits B_IS_C     = 0x02; // candidate object present

    // next two are mutual exclusive
    static const bits B_TO_DEL   = 0x04; // request to delete installed object
    static const bits B_TO_INS   = 0x08; // request to install candidate object

    static const bits B_BY_USER  = 0x10; // modification requested by user

    static const bits B_F_TABOO  = 0x20; // In absence of installed object
                                         // forbid to install candidate object

    //static const bits B_UNUSED  = 0x40;
    //static const bits B_UNUSED  = 0x80;

    static const bits M_IS = B_IS_I | B_IS_C;

    static const bits M_TO = B_TO_DEL | B_TO_INS;

  private:

    bits _bits;

    void set( const bits mask_r ) { _bits |= mask_r; }
    void clr( const bits mask_r ) { _bits &= ~mask_r; }

  public:

    SelState();
    ~SelState();

  public:

    /**
     * Set whether an installed object is present (clears taboo)
     **/
    void set_has_installed( bool b = true );

    /**
     * Set whether a candidate object is present (clears taboo)
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

    /**
     * True if modification was requested by user
     **/
    bool by_user()       const { return( _bits & B_BY_USER ); }

    /**
     * True if forbidden to install a candidate object.
     **/
    bool is_taboo()      const { return( _bits & B_F_TABOO ); }

  public:

    /**
     * User request to clear state (neither delete nor install).
     **/
    bool user_unset( const bool doit );

    /**
     * User request to delete the installed object. Fails if no
     * installed object is present.
     **/
    bool user_set_delete( const bool doit );

    /**
     * User request to install the candidate object. Fails if no
     * candidate object is present, or taboo.
     **/
    bool user_set_install( const bool doit );

    /**
     * Forbid to install candidate object.
     **/
    bool user_set_taboo( const bool doit );

    /**
     * Clear taboo flag.
     **/
    bool user_clr_taboo( const bool doit );

    /**
     * Auto request to clear state (neither delete nor install).
     * Fails if user requested modification.
     **/
    bool auto_unset( const bool doit );

    /**
     * Auto request to delete the installed object. Fails if no
     * installed object is present, or user requested install.
     **/
    bool auto_set_delete( const bool doit );

    /**
     * Auto request to install the candidate object. Fails if no
     * candidate object is present, or user requested delete, or taboo.
     **/
    bool auto_set_install( const bool doit );

  public:

    friend std::ostream & operator<<( std::ostream & str, const SelState & obj );
};

///////////////////////////////////////////////////////////////////

#endif // SelState_h

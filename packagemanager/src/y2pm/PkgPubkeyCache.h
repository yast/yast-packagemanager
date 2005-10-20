/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                    (C) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:       PkgPubkeyCache.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Manage gpg-pubkeys (package singning keys)

/-*/
#ifndef PkgPubkeyCache_h
#define PkgPubkeyCache_h

#include <iosfwd>

#include <y2util/Pathname.h>

#include <y2pm/InstSrcError.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgPubkeyCache
/**
 * @short Manage gpg-pubkeys (package singning keys)
 *
 * Store ascii armored package singning keys and import then
 * into the RPM database on @ref sync.
 *
 * The name of a keyfile must match
 * <code>gpg-pubkey-<version>-<release>.asc</code>, with
 * <version>-<release> as it will appear in the RPM database
 * after import.
 **/
class PkgPubkeyCache {

  friend std::ostream & operator<<( std::ostream & str, const PkgPubkeyCache & obj );

  PkgPubkeyCache & operator=( const PkgPubkeyCache & );
  PkgPubkeyCache            ( const PkgPubkeyCache & );

  private:

    static const std::string _prefix;
    static const std::string _ext;

    Pathname _cachedir;

  public:

    /**
     * default error class
     **/
    typedef InstSrcError Error;

  public:

    /**
     * Constructor (no chache)
     **/
    PkgPubkeyCache() {}

    /**
     * Constructor (use cachedir_r)
     **/
    PkgPubkeyCache( const Pathname & cachedir_r );

    /**
     * Destructor
     **/
    ~PkgPubkeyCache();

  public:

    /**
     * @return The cache dir, or an empty path if disabled.
     **/
    const Pathname & cachedir() const { return _cachedir; }

    /**
     * @return Whether the cache is disabled (no cachedir set).
     **/
    bool cacheDisabled() const { return _cachedir.empty(); }

    /**
     * @return True if keyname_r matches
     * 'gpg-pubkey-<version>-<release>.asc'
     **/
    static bool isKey( const std::string & keyname_r );

    /**
     * @return True if basename of keyfile_r matches
     * 'gpg-pubkey-<version>-<release>.asc'
     **/
    static bool isKey( const Pathname & keyfile_r ) {
      return isKey( keyfile_r.basename() );
    }

    /**
     * @return The '<version>-<release>' part of keyname_r,
     * or an empty string if it's no keyname.
     **/
    static std::string keyEdition( const std::string & keyname_r );

    /**
     * @return The '<version>-<release>' part of the key denoted by
     * keyfile_r, or an empty string if it's no keyname.
     **/
    static std::string keyEdition( const Pathname & keyfile_r ) {
      return keyEdition( keyfile_r.basename() );
    }

    /**
     * @return True if the key denoted by keyname_r
     * is in the cache. keyname_r must match eiter
     * 'gpg-pubkey-<version>-<release>.asc' or
     * '<version>-<release>'.
     **/
    bool hasKey( const std::string & keyname_r ) const;

    /**
     * @return True if the key denoted by keyfile_r
     * is in the cache.
     **/
    bool hasKey( const Pathname & keyfile_r ) const {
      return hasKey( keyfile_r.basename() );
    }

  public:

    /**
     * Set the cachedir to use. It will be created if necessary.
     **/
    PMError setCachedir( const Pathname & cachedir_r );

    /**
     * Store the key denoted by keyfile_r in the cache.
     **/
    PMError storeKey( const Pathname & keyfile_r );

    /**
     * Take care that all stored keys are imported into the
     * targets rpm database.
     **/
    PMError sync();


    /**
     * Copy cache content into newcachedir_r.
     **/
    PMError cacheCopyTo( const Pathname & newcachedir_r );
};

///////////////////////////////////////////////////////////////////

#endif // PkgPubkeyCache_h

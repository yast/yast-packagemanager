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

  File:       MediaError.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Definition of "Media" error values.

/-*/
#ifndef MediaError_h
#define MediaError_h

#include <iosfwd>

#include <y2pm/PMError.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaError
/**
 * @short Definition of "Media" error values.
 * @see PMError
 **/
class MediaError {

  private:

    friend class PMError;

    static const std::string errclass;

    static std::string errtext( const unsigned e );

  public:

    enum Error {
      E_ok    = PMError::E_ok,        	// no error
      E_error = PMError::C_MediaError,	// some error
      // more specific errors start here:
      E_system,		// see errno
      E_not_a_directory,
      E_not_a_file,
      E_no_access,
      E_no_permission,
      E_no_space,
      E_bad_url,
      E_bad_media_type,
      E_bad_filename,
      E_already_attached,
      E_not_attached,
      E_bad_attachpoint,
      E_not_open,
      E_no_destination,
      E_no_source,
      E_invalid_device,
      E_invalid_filesystem,
      E_no_host_specified,
      // wget errors
      E_file_not_found,
      E_login_failed,
      E_connection_failed,
      E_proxyauth_failed,
      //mount
      E_mount_failed,
      E_umount_failed,
      E_already_mounted,
      E_busy,
      //
      E_attachpoint_fixed,
      E_not_supported_by_media,
      // libcurl errors
      E_curl_setopt_failed,
      E_write_error,
      E_user_abort
    };

    friend std::ostream & operator<<( std::ostream & str, const Error & obj ) {
      return str << PMError( obj );
    }
};

///////////////////////////////////////////////////////////////////

#endif // MediaError_h


/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                     (C) 2002 SuSE AG |
\----------------------------------------------------------------------/

  File:       InstTargetError.h

  Author:     Ludwig Nussel <lnussel@suse.de>
  Maintainer: Ludwig Nussel <lnussel@suse.de>

  Purpose: Definition of "InstTarget" error values.


/-*/
#ifndef InstTargetError_h
#define InstTargetError_h

#include <iosfwd>

#include <y2pm/PMError.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstTargetError
/**
 * @short Definition of "InstTarget" error values.
 * @see PMError
 **/
class InstTargetError {

  private:

    friend class PMError;

    static const std::string errclass;

    static std::string errtext( const unsigned e );

  public:

    enum Error {
      E_ok    = PMError::E_ok,        	// no error
      E_error = PMError::C_InstTargetError,	// some error
      // more specific errors start here:
      E_system,		// see errno
      E_RpmDB_not_found,
      E_RpmDB_create_failed,
      E_RpmDB_check_old_version_failed,
      E_RpmDB_mkdir_failed,
      E_RpmDB_initdb_failed,
      E_RpmDB_copy_tmpdb_failed,
      E_RpmDB_rebuilddb_failed,
      E_RpmDB_not_initialized,
      E_RpmDB_subprocess_failed,
      E_RpmDB_old_db,
      //
      E_RpmLib_read_config_failed,
      E_RpmLib_dbopen_failed,
      E_RpmLib_db_not_open,
      //
      E_SelDB_already_open,
      E_SelDB_open_failed,
      E_SelDB_not_open,
      E_SelDB_file_not_found,
      E_SelDB_file_bad_magic,
      E_SelDB_install_failed,
      E_SelDB_not_installed,
      //
      E_ProdDB_already_open,
      E_ProdDB_open_failed,
      E_ProdDB_not_open,
      E_ProdDB_file_bad_magic,
      E_ProdDB_install_failed,
      E_ProdDB_not_installed,
      //
      E_UpdateInf_not_found,
      E_UpdateInf_malformed
    };

    friend std::ostream & operator<<( std::ostream & str, const Error & obj ) {
      return str << PMError( obj );
    }
};

///////////////////////////////////////////////////////////////////

#endif // InstTargetError_h


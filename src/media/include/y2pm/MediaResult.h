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

   File:	MediaResult.h

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Define error codes for MediaAccess

/-*/

#ifndef MediaResult_h
#define MediaResult_h

#ifndef N_
#  define N_(STR) STR
#endif

typedef enum media_result {
    E_none = 0,
    E_system,		// see errno
    E_not_a_directory,
    E_no_access,
    E_no_permission,
    E_no_space,
    E_bad_url,
    E_bad_media_type,
    E_bad_filename,
    E_already_attached,
    E_not_attached,
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
    E_already_mounted,
    E_busy,

    E_attachpoint_fixed,

    E_NUM_ERRORS
} MediaResult;

const char* const media_result_strings[] = {
	N_("OK"),
	N_("Unkown error in subsystem"),
	N_("not a directory"),
	N_("access denied"), //FIXME
	N_("insufficient permissions"),
	N_("no space left"),
	N_("invalid URL"),
	N_("invalid media type"),
	N_("invalid filename"),
	N_("already attached"),
	N_("not attached"),
	N_("not opened"),
	N_("no destination"),
	N_("no source"),
	N_("invalid device"),
	N_("invalid filesystem"),
	N_("no hostname specified"),

	N_("file not found"),
	N_("login failed"),
	N_("connection failed"),
	N_("proxy authenticatin failed"),
	
	//Translator: the standard mount program is meant
	N_("mount failed"),
	N_("source already mounted"),
	N_("ressource busy"),

	N_("attach point can not be changed")
};

#endif // MediaResult_h

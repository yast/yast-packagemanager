#ifndef MediaResult_h
#define MediaResult_h

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
    E_no_destination
} MediaResult;

#endif // MediaResult_h

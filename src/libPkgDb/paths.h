#ifndef _PkgDb_paths_h
#define _PkgDb_paths_h

#include <string>


#define _RPM_LIB_PATH		"/var/lib/rpm"
#ifdef NDEBUG
#define PKGDB_BIN_PATH		"/usr/lib/PHI:/usr/bin"
#else
#define PKGDB_BIN_PATH		"/usr/lib/PHI:/usr/bin:tools"
#endif
#define _PKGDB_VLIB_PATH		"/var/lib/PHI"
#define PKGDB_ULIB_PATH		"/usr/lib/PHI"

#define RPMDB_FILENAME		"packages.rpm"
#define ICACHE_FILENAME		"installed-cache"
#define REQFILES_FILENAME	"required-files"
#define OVERRIDES_FILENAME	"overrides"
#define ALTDEFAULTS_FILENAME "alternative-defaults"
#define INFO_FILENAME		"PACKAGE-INFOS"
#define RELEASE_FILENAME	"RELEASE"
#define RCACHE_PREFIX		"cache_"
#define RRELEASE_PREFIX		"release_"

/*
#define ICACHE_PROG		PKGDB_ULIB_PATH"/pkg-installed-cache"
#define MKSUM_PROG		PKGDB_ULIB_PATH"/pkg-rpm2sum"
#define ASCII2SUM_PROG		PKGDB_ULIB_PATH"/pkg-ascii2sum"
#define SUM2ASCII_PROG		PKGDB_ULIB_PATH"/pkg-sum2ascii"
*/

#define ICACHE_PROG		"pkg-installed-cache"
#define MKSUM_PROG		"pkg-rpm2sum"
#define ASCII2SUM_PROG		"pkg-ascii2sum"
#define SUM2ASCII_PROG		"pkg-sum2ascii"

extern char* rpm_lib_path;
extern char* pkgdb_vlib_path;
extern char* root;

/*
#define ICACHE_PROG			"phi-db --installed-cache"
#define MKSUM_PROG			"phi-db --rpm2sum"
#define ASCII2SUM_PROG		"phi-db --ascii2sum"
#define SUM2ASCII_PROG		"phi-db --sum2ascii"
*/

#endif  /* _PkgDb_paths_h */


// Local Variables:
// tab-width: 4
// End:

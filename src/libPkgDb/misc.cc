#include <y2pm/PkgDb.h>
#include <Exception.h>
#include <unistd.h>
#include <wait.h>
#include "paths.h"

int call_prog( const char *prog, ... )
{
	const char *bin = prog;
	int status, pid;
	
	if (prog[0] != '/') {
		int len = strlen(prog);
		char *path = strdup(PKGDB_BIN_PATH);
		char *p;
		// search for prog in PKGDB_BIN_PATH
		for( p = strtok( path, ":" ); p; p = strtok( NULL, ":" ) ) {
			char *s = new char[ strlen(p) + len + 2 ];
			strcpy( s, p );
			strcat( s, "/" );
			strcat( s, prog );
			if (access( s, X_OK ) == 0) {
				bin = s;
				break;
			}
			free( (void *)s );
		}
		free( (void *)path );
	}

	if ((pid = fork()) == -1) {
		if (bin != prog) free( (void *)bin );
		throw PkgDbFileExcp( "fork" );
	}
	else if (pid != 0) {
		// parent
		wait(&status);
	}
	else {
		// child
		execv( bin, (char * const *)&prog );
		if (bin != prog) free( (void *)bin );
		throw PkgDbFileExcp( "execve" );
	}
	
	if (bin != prog) free( (void *)bin );
	return WEXITSTATUS(status);
}

// Local Variables:
// tab-width: 4
// End:

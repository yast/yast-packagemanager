#include <cassert>
//#include <fstream>
#include <string>
#include <PkgDb.h>
#include <PkgSet.h>
#include <Exception.h>
#include <unistd.h>
/*
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <rpm/rpmlib.h>
#include "paths.h"
#include "misc.h"
*/

PkgDb PkgPool;

size_t hashfun( const PkgNameEd& ne )
{
	return hashfun(ne.name) + hashfun(ne.edition.as_string());
}


// ---------------------------------------------------------------------------
//						   constructor and helpers
// ---------------------------------------------------------------------------


void PkgDb::read_override_list()
{
	cerr << "read_override_list not implemented" << endl;
#if 0
	ifstream ov( PKGDB_ULIB_PATH "/" OVERRIDES_FILENAME );
	if (!ov)
		return;
	
	int lineno = 1;
	Tag tag;
	while( !ov.eof() && !ov.fail() ) {
		DbHeader *hdr = new DbHeader;
		hdr->read( ov, &lineno, true );

		try {
			tag = hdr->get_tag( RPMTAG_NAME );
			assert( tag.count == 1 && tag.type == RPM_STRING_TYPE );
			const char *n = cpTag(tag)[0];
			PkgName name(n);

			tag = hdr->get_tag( RPMTAG_VERSION );
			assert( tag.count == 1 && tag.type == RPM_STRING_TYPE );
			const char *version = cpTag(tag)[0];
			
			tag = hdr->get_tag( RPMTAG_RELEASE );
			assert( tag.count == 1 && tag.type == RPM_STRING_TYPE );
			const char *release = cpTag(tag)[0];

			PkgEdition edition;
			try {
				tag = hdr->get_tag( RPMTAG_EPOCH );
				assert( tag.count == 1 && tag.type == RPM_INT32_TYPE );
				int_32 epoch = iTag(tag)[0];
				edition = PkgEdition( epoch, version, release );
			}
			catch( PkgDbNoTagExcp ) {
				edition = PkgEdition( version, release );
			}

			PkgNameEd ne( name, edition );
			Overrides[ne] = hdr;
		}
		catch( PkgDbNoTagExcp ) {
			// if NAME, VERSION, or RELEASE tags not found, skip this header
		}
	}
#endif
}

void PkgDb::read_static_reqfiles()
{
	cerr << "read_static_reqfiles not implemented" << endl;
#if 0
	ifstream rf( PKGDB_ULIB_PATH "/" REQFILES_FILENAME );
	if (!rf)
		return;
	
	string line;
	while( rf ) {
		getline( rf, line );
		if (rf.fail())
			break;
		if (line[0])
			StaticRequiredFiles.push_back( strdup(line.c_str()) );
	}
#endif
}

void PkgDb::read_alt_defaults()
{
	cerr << "read_alt_defaults not implemented" << endl;
#if 0
	ifstream af( PKGDB_ULIB_PATH "/" ALTDEFAULTS_FILENAME );
	if (!af)
		return;

	string line;
	int lineno = 0;
	while( af ) {
		getline( af, line );
		if (af.fail())
			break;
		++lineno;
		size_t name_start = line.find_first_not_of( " \t" );
		if (name_start == string::npos)
			continue; // empty line
		size_t name_end = line.find( ':', name_start );
		if (name_end == string::npos) {
			cerr << ALTDEFAULTS_FILENAME ":" << lineno << ": "
			     << "no colon found\n";
			continue;
		}
		if (name_end == name_start) {
			cerr << ALTDEFAULTS_FILENAME ":" << lineno << ": "
			     << "empty package name before colon\n";
			continue;
		}
		string sname = line.substr( name_start, name_end-name_start );
		PkgName name(sname);
		alt_defaults.insert( name, AltDefaultList() );

		for( size_t start = name_end+1; start < line.length(); ) {
			start = line.find_first_not_of( ", \t", start );
			size_t end = line.find_first_of( ", \t", start );
			string pkg = line.substr( start, end-start );
			alt_defaults[name].push_back( PkgName(pkg) );
			start = (end == string::npos) ? end : end+1;
		}

		if (alt_defaults[name].size() == 0) {
			cerr << ALTDEFAULTS_FILENAME ":" << lineno << ": "
			     << "no alternatives\n";
			alt_defaults.erase(name);
		}
	}
#endif
}

PkgDb::PkgDb()
{
	try {
		read_override_list();
		read_static_reqfiles();
		read_alt_defaults();
	}
	catch( PkgDbExcp& e ) {
		cerr << "Exception in initialization of PkgPool: " << e << endl;
		terminate();
	}
}


// ---------------------------------------------------------------------------
//							add_source and helpers
// ---------------------------------------------------------------------------

#if 0
static string build_cache( const string& path, const string& sum_file1,
						   const string& sum_file2 )
{
	// Please note that access() does NOT report EROFS!
	int fd;
	string sum_file=((fd=::open(sum_file1.c_str(),O_RDWR|O_CREAT))!=-1) ?  sum_file1 : sum_file2;
	if (fd!=-1) ::close(fd);
	
	call_prog( MKSUM_PROG, "-o", sum_file.c_str(), path.c_str(), NULL );

	struct stat st;
	if (stat( sum_file.c_str(), &st ) < 0) {
		return string("");
	}

	return sum_file;
}

static string path_quote( string in )
{
	string out = in;
	const char *repl_chars = "*?[]<>|$\"'&:;_=";

	for( unsigned pos = out.find_first_of( repl_chars ); pos < out.length();
		 pos = out.find_first_of( repl_chars, pos ) ) {
		char buf[4];
		sprintf( buf, "=%02X", out[pos] );
		out.replace( pos, 1, buf );
		pos += 4;
	}
	for( unsigned pos = out.find( '/' ); pos < out.length();
		 pos = out.find( '/', pos ) ) {
		out.replace( pos, 1, "_" );
		pos++;
	}
	return out;
}

static bool is_newer_than_rpms( time_t cache_time, const char *path )
{
	DIR *dir;
	struct dirent *de;
	struct stat st;
	bool is_newer = true;

	if (!(dir = opendir( path ))) {
		// special case: If the path even doesn't exist, consider the cache
		// current. This is handy for "faked" caches in PKGDB_VLIB_PATH.
		return true;
	}
	while( (de = readdir( dir )) ) {
		string _fname = string(path) + "/" + string(de->d_name);
		const char *fname = _fname.c_str();
		const char *fname_end = fname + _fname.length();

		// skip files that aren't *.rpm or are *.src.rpm
		if (strcmp(fname_end-4, ".rpm") != 0 ||
			strcmp(fname_end-8, ".src.rpm") == 0)
			continue;
		if (stat( fname, &st ) < 0)
			continue;
		if (st.st_mtime > cache_time) {
			is_newer = false;
			break;
		}
	}
	closedir( dir );
	return is_newer;
}
#endif

void PkgDb::recheck_pool_for_ReqFile( const char *rf )
{
	for( iterator p = begin(); p != end(); ++p )
		check_one_ReqFile( p->value, rf );
}

void PkgDb::check_new_ReqFiles( Package *pkg, const ReqFiles_type& RF )
{
	for( ReqFiles_const_iterator p = RF.begin(); p != RF.end(); ++p )
		check_one_ReqFile( pkg, *p );
}

void PkgDb::notify_sets_of_new_provides( const Package *pkg,
										 const PkgRelation& new_p )
{
	for( list<PkgSet*>::iterator p = attached_sets.begin();
		 p != attached_sets.end(); ++p )
		(*p)->new_provides( pkg, new_p );
}

void PkgDb::check_one_ReqFile( Package *pkg, const char *rf )
{
	for( Package::FileList_const_iterator p = pkg->files().begin();
		 p != pkg->files().end(); ++p ) {
		if ((*p) == rf) {
			const PkgRelation& new_p = pkg->add_provides( rf );
			notify_sets_of_new_provides( pkg, new_p );
		}
	}
}

static bool inReqF( const PkgDb::ReqFiles_type& RF, const char *rf )
{
	for( PkgDb::ReqFiles_const_iterator p = RF.begin(); p != RF.end(); ++p )
		if (strcmp( *p, rf ) == 0)
			return true;
	return false;
}

static void free_ReqFiles( PkgDb::ReqFiles_type& RF )
{
	for( PkgDb::ReqFiles_iterator p = RF.begin(); p != RF.end(); ++p )
		free( (void *)*p );
}

#if 0
void PkgDb::add_source( const char *_path, DistTag tag )
{
	string path(_path);
	bool have_summary = false;
	string summary_file;
	PFD_t fd = 0;

	bool path_is_dir = false;
	struct stat p_stat;
	if (stat( _path, &p_stat ) == 0 && S_ISDIR(p_stat.st_mode))
		path_is_dir = true;
	
	if (tag == INSTALLED_Tag) {

		// INSTALLED_tag is handled differently: It stands for the "release"
		// of installed packages and we read it from our cache file
		// (ICACHE_FILENAME). If the cache is out of date (older than
		// /var/lib/dpkg/packages.rpm) it is rebuilt first.
		
		struct stat s_cache, s_rpmdb;
		char* rpmdb=(char*)malloc(strlen(rpm_lib_path)+2+strlen(RPMDB_FILENAME));
		strcpy(rpmdb,rpm_lib_path);
		strcat(rpmdb,"/"RPMDB_FILENAME);
		char* icache=(char*)malloc(strlen(pkgdb_vlib_path)+2+strlen(ICACHE_FILENAME));
		strcpy(icache,pkgdb_vlib_path);
		strcat(icache,"/"ICACHE_FILENAME);

		if (stat( rpmdb, &s_rpmdb ) < 0)
			throw PkgDbFileExcp(rpmdb);

		if (	(stat( icache, &s_cache ) < 0) ||
			(s_rpmdb.st_mtime > s_cache.st_mtime)
		) {
			int ret;

			ret = call_prog( ICACHE_PROG, "-o",pkgdb_vlib_path,"-r", root, NULL );
			if (ret == 42) // special code, 2.5.5 RPM
				ret=call_prog( ICACHE_PROG"-2.5.5", "-o",pkgdb_vlib_path,"-r",root, NULL );
			if (ret || (stat( icache, &s_cache ) < 0))
				throw PkgDbExcp( (string)(ICACHE_PROG " failed to update ") +pkgdb_vlib_path + (string)("/" ICACHE_FILENAME) );
		}

		fd = pfdOpen( icache, O_RDONLY, 0 );
		if (!pfdValid(fd))
			throw PkgDbFileExcp( icache );
		have_summary = true;
		summary_file = icache;
	}
	else if (tag == NONE_Tag) {
		// If no tag has been given as parameter, look for a file
		// "RELEASE" in the package path. Use the first line of this file
		// as tag if found.
		string rel_name1 = path + "/" RELEASE_FILENAME;
		string rel_name2 = (string)pkgdb_vlib_path+(string)("/"RRELEASE_PREFIX)+path_quote(path);
		struct stat st;
		if (path_is_dir &&
			stat( rel_name1.c_str(), &st ) == 0 && !S_ISDIR(st.st_mode)) {
			ifstream rel_file( rel_name1.c_str() );
			char rel[80];
			rel_file.getline(rel,sizeof(rel));
			tag = DistTag( rel );
		}
		else if (stat( rel_name2.c_str(), &st ) == 0 && !S_ISDIR(st.st_mode)) {
			ifstream rel_file( rel_name2.c_str() );
			char rel[80];
			rel_file.getline(rel,sizeof(rel));
			tag = DistTag( rel );
		}
		else {
			// If also that failed, simply use the path
			tag = DistTag( path );
		}
	}

	// if it wasn't INSTALLED_Tag, have_summary is still false
	// Look if there's a cache file in the path, otherwise if there's one in
	// PKGDB_VLIB_PATH. If both fails or the cache found is outdated,
	// build/update the cache. The new cache is written to the path if it's
	// writable, otherwise to PKGDB_VLIB_PATH.
	if (!have_summary) {
		string info_name1 = path + "/" INFO_FILENAME;
		string info_name2 = (string)pkgdb_vlib_path+(string)("/"RCACHE_PREFIX) + path_quote(path);
		struct stat st;

		if (path_is_dir &&
			stat( info_name1.c_str(), &st ) == 0 &&
			!S_ISDIR(st.st_mode) &&
			!access(info_name1.c_str(),R_OK) && 
			is_newer_than_rpms( st.st_mtime, _path )) {
			summary_file = info_name1;
		}
		else if (stat( info_name2.c_str(), &st ) == 0 &&
				 !S_ISDIR(st.st_mode) &&
				 is_newer_than_rpms( st.st_mtime, _path )) {
			summary_file = info_name2;
		} else {
			summary_file = build_cache( path, info_name1, info_name2 );
			if (summary_file.length() == 0)
				throw PkgDbExcp( "building cache failed" );
		}

		fd = pfdOpen( summary_file.c_str(), O_RDONLY, 0 );
		if (!pfdValid(fd))
			throw PkgDbFileExcp( summary_file.c_str() );
	}
	
	// read PkgDb prepended header
	DbHeader hdr( summary_file.c_str(), fd );
	try {
		Tag tag;
		tag = hdr.get_tag( PKGDBTAG_VERSION );
		assert( tag.count == 1 && tag.type == RPM_INT32_TYPE );
		if (iTag(tag)[0] > PKGDB_CURRENT_VERSION)
			throw PkgDbExcp( "too new cache version" );
	}
	catch( PkgDbNoTagExcp ) {
		throw PkgDbExcp( "This is no valid PkgDb cache "
						 "(PKGDBTAG_VERSION tag not found)" );
	}
	
	ReqFiles_type CacheRequiredFiles;
	try {
		Tag tag;
		tag = hdr.get_tag( PKGDBTAG_REQUIRED_FILES );
		for( int i = 0; i < tag.count; ++i )
			CacheRequiredFiles.push_back( strdup( cpTag(tag)[i] ));
	}
	catch( PkgDbNoTagExcp ) { }

	// If the pool's current ReqFile list (static+added) contains something
	// the cache doesn't know about, check all packages from the cache for
	// those files while adding them to the pool.
	ReqFiles_type RecheckRequiredFiles;
	for( ReqFiles_iterator p = StaticRequiredFiles.begin();
		 p != StaticRequiredFiles.end(); ++p ) {
		if (!inReqF( CacheRequiredFiles, *p ))
			RecheckRequiredFiles.push_back( strdup(*p) );
	}
	for( ReqFiles_iterator p = AddRequiredFiles.begin();
		 p != AddRequiredFiles.end(); ++p ) {
		if (!inReqF( CacheRequiredFiles, *p ))
			RecheckRequiredFiles.push_back( strdup(*p) );
	}
	
	// If the cache knows about some more required files than the pool until
	// now, go through all packages in the pool and look if they provide this
	// file as well.
	for( ReqFiles_iterator p = CacheRequiredFiles.begin();
		 p != CacheRequiredFiles.end(); ++p ) {
		if (!inReqF(StaticRequiredFiles,*p) && !inReqF(AddRequiredFiles,*p)) {
			recheck_pool_for_ReqFile( *p );
			AddRequiredFiles.push_back( strdup(*p) );
		}
	}
	free_ReqFiles( CacheRequiredFiles );

	// read real package headers
	while( 1 ) {
		try {
			DbHeader hdr( summary_file.c_str(), fd );
			Package *pkg = new Package( hdr );
			check_new_ReqFiles( pkg, RecheckRequiredFiles );
			add_package( pkg, tag );
		}
		catch( PkgDbExcp e ) {
			if (strncmp( e.opname(), "EOF", 3 ) == 0)
				break;
			throw;
		}
	}
	pfdClose( fd );
	free_ReqFiles( RecheckRequiredFiles );
}
#endif

void PkgDb::add_package( Package *pkg, PackageDataProvider* provider)
{
	PkgNameEd ne( pkg->name(), pkg->edition() );
	if (Pool.exists( ne )) {
		Package *p = Pool[ne];
		delete pkg;
		p->addDataProvider( provider );
//		p->add_dist( tag );
	}
	else {
//		pkg->add_dist( tag );
		pkg->addDataProvider( provider );
		Pool.insert( ne, pkg );
	}
}

// Local Variables:
// tab-width: 4
// End:

#include <iomanip>
#include <fstream>
#include <string>
#include <list>

#include <Y2PM.h>
#include <y2pm/RpmDb.h>

#include <y2pm/InstSrcManager.h>
#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/MediaAccess.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMSelectionManager.h>
#include <y2pm/InstTarget.h>

#include <rpm/rpmlib.h>

#include <y2util/Y2SLog.h>
#include <y2util/Date.h>
#include <y2util/stringutil.h>
#include <y2util/ExternalProgram.h>

#include <y2pm/RpmLibDb.h>


using namespace std;

static const string Q_A( "%{NAME} %{VERSION} %{RELEASE} %{ARCH}\n" );
static const string Q_F( "-\n%{NAME}\n%{VERSION}\n%{RELEASE}\n%{ARCH}\n[%{FILEDEVICES}\n%{FILEINODES}\n%{FILESIZES}\n%{FILENAMES}\n]" );

struct fdata {
  int_32 dev;
  int_32 ino;
  int_32 sze;
  string fname;
  fdata() { dev = ino = sze = 0; }
};

struct pdata {
  string name;
  string vers;
  string rel;
  string arch;
  list<fdata> files;
};

ostream & operator<<( ostream & str, const fdata & obj )
{
  str << obj.dev << " " << obj.ino << " " << obj.sze << " " << obj.fname << endl;
  return str;
}

ostream & operator<<( ostream & str, const pdata & obj )
{
  str << obj.name << "-" << obj.vers << "-" << obj.rel << "." << obj.arch << endl;
  for ( list<fdata>::const_iterator it = obj.files.begin(); it != obj.files.end(); ++it ) {
    str << *it;
  }
  return str;
}

void dumpTo( ostream & str, const list<pdata> & result_r )
{
  for ( list<pdata>::const_iterator it = result_r.begin(); it != result_r.end(); ++it ) {
    str << *it;
  }
}

void dumpTo( const string & file, const list<pdata> & result_r )
{
  ofstream runQuery( file.c_str() );
  dumpTo( runQuery, result_r );
  runQuery.close();
}

int runQuery( const string & query_r, list<pdata> & result_r )
{
  result_r.clear();

  MIL << "RUN: " << query_r << endl;
  Date ts = Date::now();

  const char* args[] = { "rpm", "-q", "-a", "--qf", query_r.c_str(), 0 };
  ExternalProgram cmd( args, ExternalProgram::Discard_Stderr );
  int idx = 10;
  for ( string line = cmd.receiveLine(); line.size(); line = cmd.receiveLine() ) {
    line = stringutil::rtrim( line );
    if ( line == "-" ) {
      if ( idx != 10 ) {
	WAR << "IDX not 10" << endl;
      }
      idx = 0;
      continue;
    }
    switch( idx ) {
    case 0:
      result_r.push_back( pdata() );
      result_r.back().name = line;
      ++idx;
      break;
    case 1:
      result_r.back().vers = line;
      ++idx;
      break;
    case 2:
      result_r.back().rel = line;
      ++idx;
      break;
    case 3:
      result_r.back().arch = line;
      idx = 10;
      break;
    case 10:
      result_r.back().files.push_back( fdata() );
      result_r.back().files.back().dev = atol( line.c_str() );
      ++idx;
      break;
    case 11:
      result_r.back().files.back().ino = atol( line.c_str() );
      ++idx;
      break;
    case 12:
      result_r.back().files.back().sze = atol( line.c_str() );
      ++idx;
      break;
    case 13:
      result_r.back().files.back().fname = line;
      idx = 10;
      break;
    default:
      ERR << "ABORT" << endl;
      exit( 1 );
      break;
    }
  }

  int ret = cmd.close();

  Date te = Date::now();
  MIL << "RETURN: " << (te-ts) << " sec; return " << ret << "; entries " << result_r.size() << endl;

  dumpTo( "runQuery.log", result_r );

  return ret;
}

int libQuery( list<pdata> & result_r )
{
  result_r.clear();

  MIL << "RUN: " << endl;
  Date ts = Date::now();

  int rc = rpmReadConfigFiles( 0, 0 );
  if (  rc ) {
    ERR << "rpmReadConfigFiles returned: " << rc << endl;
    return -1;
  }

  rpmdb db = 0;
  rc = rpmdbOpenForTraversal( 0, &db );
  if ( rc ) {
    ERR << "rpmdbOpenForTraversal returned: " << rc << endl;
    return -1;
  }

  for ( int recnum = rpmdbFirstRecNum( db ); recnum > 0; recnum = rpmdbNextRecNum( db, recnum ) ) {
    Header h = rpmdbGetRecord( db, recnum );
    if ( !h ) {
      ERR << "Skip bad record number " << recnum << " in rpmdb" << endl;
      continue; // ?? or break?
    }

    char * sval;
    result_r.push_back( pdata() );
    headerGetEntry(h, RPMTAG_NAME,    NULL, (void **) &sval, NULL);
    result_r.back().name = sval;
    headerGetEntry(h, RPMTAG_VERSION, NULL, (void **) &sval, NULL);
    result_r.back().vers = sval;
    headerGetEntry(h, RPMTAG_RELEASE, NULL, (void **) &sval, NULL);
    result_r.back().rel = sval;
    headerGetEntry(h, RPMTAG_ARCH,    NULL, (void **) &sval, NULL);
    result_r.back().arch = sval;

    const char ** baseNames;
    const char ** dirNames;
    int_32 *      dirIndexes;
    int_32 *      fileDev;
    int_32 *      fileIno;
    int_32 *      fileSize;
    int           count;

    int i;

    if (!headerGetEntry(h, RPMTAG_BASENAMES, NULL, (void **) &baseNames, &count)) {
      INT << "NoFiles " << result_r.back() << endl;
      continue;         /* no file list */
    }

    headerGetEntry(h, RPMTAG_DIRNAMES,   NULL, (void **) &dirNames,   NULL);
    headerGetEntry(h, RPMTAG_DIRINDEXES, NULL, (void **) &dirIndexes, NULL);

    headerGetEntry(h, RPMTAG_FILEDEVICES, NULL, (void **) &fileDev,  NULL);
    headerGetEntry(h, RPMTAG_FILEINODES,  NULL, (void **) &fileIno,  NULL);
    headerGetEntry(h, RPMTAG_FILESIZES,   NULL, (void **) &fileSize, NULL);

    for ( i = 0; i < count; i++ ) {
      result_r.back().files.push_back( fdata() );
      result_r.back().files.back().dev = fileDev[i];
      result_r.back().files.back().ino = fileIno[i];
      result_r.back().files.back().sze = fileSize[i];
      result_r.back().files.back().fname = string( dirNames[dirIndexes[i]] ) + baseNames[i];
    }

    xfree(baseNames);
    xfree(dirNames);
  }

  rpmdbClose( db );
  Date te = Date::now();
  MIL << "RETURN: " << (te-ts) << " sec; lines " << result_r.size() << endl;

  dumpTo( "libQuery.log", result_r );

  return rc;
}

ostream & operator <<( ostream & str, const list<string> & t ) {
  stringutil::dumpOn( str, t, true );
  return str;
}

void dataDump( ostream & str, constPMPackagePtr p ) {
  str << p << endl;
  str << "SUMMARY:       " << p->summary() << endl;
  str << "DESCRIPTION:   " << p->description() << endl;
  str << "INSNOTIFY:     " << p->insnotify() << endl;
  str << "DELNOTIFY:     " << p->delnotify() << endl;
  str << "SIZE:          " << p->size() << endl;
  str << "INSTSRCLABEL:  " << p->instSrcLabel() << endl;
  str << "INSTSRCVENDOR: " << p->instSrcVendor() << endl;
  str << "INSTSRCRANK:   " << p->instSrcRank() << endl;
  str << "BUILDTIME:     " << p->buildtime() << endl;
  str << "BUILDHOST:     " << p->buildhost() << endl;
  str << "INSTALLTIME:   " << p->installtime() << endl;
  str << "DISTRIBUTION:  " << p->distribution() << endl;
  str << "VENDOR:        " << p->vendor() << endl;
  str << "LICENSE:       " << p->license() << endl;
  str << "PACKAGER:      " << p->packager() << endl;
  str << "GROUP:         " << p->group() << endl;
  str << "CHANGELOG:     " << p->changelog() << endl;
  str << "URL:           " << p->url() << endl;
  str << "OS:            " << p->os() << endl;
  str << "PREIN:         " << p->prein() << endl;
  str << "POSTIN:        " << p->postin() << endl;
  str << "PREUN:         " << p->preun() << endl;
  str << "POSTUN:        " << p->postun() << endl;
  str << "SOURCELOC:     " << p->sourceloc() << endl;
  str << "SOURCESIZE:    " << p->sourcesize() << endl;
  str << "ARCHIVESIZE:   " << p->archivesize() << endl;
  str << "AUTHORS:       " << p->authors() << endl;
  str << "FILENAMES:     " << p->filenames() << endl;
  str << "RECOMMENDS:    " << p->recommends() << endl;
  str << "SUGGESTS:      " << p->suggests() << endl;
  str << "LOCATION:      " << p->location() << endl;
  str << "MEDIANR:       " << p->medianr() << endl;
  str << "KEYWORDS:      " << p->keywords() << endl;
}


void duInstalled( PkgDuMaster & du_master_r ) {
  PMPackageManager &   PMGR( Y2PM::packageManager() );
  du_master_r.resetStats();
  for ( PMPackageManager::PMSelectableVec::iterator it = PMGR.begin(); it != PMGR.end(); ++it ) {
    const constPMSelectablePtr & sel( *it );
    if ( sel->has_installed() ) {
      PMPackagePtr( sel->installedObj() )->du_add( du_master_r );
    }
  }
  SEC << du_master_r;
}

/******************************************************************
**
**
**	FUNCTION NAME : main
**	FUNCTION TYPE : int
**
**	DESCRIPTION :
*/
int main()
{
  Y2Logging::setLogfileName("-");
  MIL << "START" << endl;
  InstTarget &         TMGR( Y2PM::instTarget(true,"/") );

  PkgDuMaster du_master;
  {
    std::set<PkgDuMaster::MountPoint> mountpoints;
    mountpoints.insert( PkgDuMaster::MountPoint( "/", FSize(2,FSize::K), FSize(1,FSize::G) ) );
    mountpoints.insert( PkgDuMaster::MountPoint( "/usr", FSize(4,FSize::K), FSize(1,FSize::G) ) );
    mountpoints.insert( PkgDuMaster::MountPoint( "/usr/X11R6", FSize(4,FSize::K), FSize(1,FSize::G) ) );
    du_master.setMountPoints( mountpoints );
  }
  duInstalled( du_master );

  Y2PM::noAutoInstSrcManager();
  PMPackageManager &   PMGR( Y2PM::packageManager() );
  PMSelectionManager & SMGR( Y2PM::selectionManager() );
  InstSrcManager &     MGR( Y2PM::instSrcManager() );
  //InstTarget &         TMGR( Y2PM::instTarget(true,"/") );
  INT << "Total Packages "   << PMGR.size() << endl;
  INT << "Total Selections " << SMGR.size() << endl;
  duInstalled( du_master );

  unsigned cnt = 3;
  for ( PMPackageManager::PMSelectableVec::const_iterator i = PMGR.begin(); i != PMGR.end(); ++i ) {
    PMPackagePtr p( (*i)->installedObj() );
    if ( p ) {
      dataDump( DBG, p );
      if ( !--cnt )
	break;
    } else {
      WAR << "No insatlled " << *i << endl;
    }
  }


  SEC << "STOP" << endl;
  return 0;
}


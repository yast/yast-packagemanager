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

  File:       PkgDu.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>
#include <map>

#include <y2util/Y2SLog.h>
#include <y2util/stringutil.h>

#include <y2pm/PkgDu.h>
#include <y2pm/PMPackage.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgDuMaster::MountPoint
//
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuMaster::MountPoint::assignData
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PkgDuMaster::MountPoint::assignData( const MountPoint & rhs ) const
{
  if (    _mountpoint != rhs._mountpoint
       || _blocksize  != rhs._blocksize )
    return false;

  _total    = rhs._total;
  _used     = rhs._used;
  _pkgusage = rhs._pkgusage;
  return true;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const PkgDuMaster::MountPoint & obj )
{
  str
    << " T:" << obj._total   .form( FSize::K, 10 )
    << " U:" << obj._used    .form( FSize::K, 10 )
    << " P:" << obj._pkgusage.form( FSize::K, 10 )
    << " " << obj._mountpoint;
  return str;
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgDuMaster
//
///////////////////////////////////////////////////////////////////

unsigned PkgDuMaster::_counter = 0;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuMaster::PkgDuMaster
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PkgDuMaster::PkgDuMaster()
{
  newcount();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuMaster::~PkgDuMaster
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PkgDuMaster::~PkgDuMaster()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDu::addMaster
//	METHOD TYPE : void
//
//	DESCRIPTION : We know (and rely) PkgDu does not pass NULL or
//                    an array size != _mountpoints.size()
//
void PkgDuMaster::add( FSize * data_r )
{
  for ( set<MountPoint>::iterator it = _mountpoints.begin(); it != _mountpoints.end(); ++it, ++data_r ) {
    it->_pkgusage += *data_r;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuMaster::sub
//	METHOD TYPE : void
//
//	DESCRIPTION : We know (and rely) PkgDu does not pass NULL or
//                    an array size != _mountpoints.size()
//
void PkgDuMaster::sub( FSize * data_r )
{
  for ( set<MountPoint>::iterator it = _mountpoints.begin(); it != _mountpoints.end(); ++it, ++data_r ) {
    it->_pkgusage -= *data_r;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuMaster::resetStats
//	METHOD TYPE : unsigned
//
//	DESCRIPTION :
//
unsigned PkgDuMaster::resetStats()
{
  _total = 0;

  for ( set<MountPoint>::iterator it = _mountpoints.begin(); it != _mountpoints.end(); ++it ) {
    it->_pkgusage = 0;
  }
  return _mountpoints.size();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDuMaster::setMountPoints
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PkgDuMaster::setMountPoints( const set<MountPoint> & mountpoints_r )
{
  if ( _mountpoints.size() == mountpoints_r.size() ) {
    // Common case is update of nonvital data (total, used, etc.)
    // So we start, and if different vital data (mountpoint/blocksize)
    // occur we abort and assign the whole set.
    for ( set<MountPoint>::iterator lit = _mountpoints.begin(), rit = mountpoints_r.begin();
	  lit != _mountpoints.end(); ++lit, ++rit ) {
      if ( ! lit->assignData( *rit ) ) {
	// different vital data
	_mountpoints = mountpoints_r;
	newcount();
	return;
      }
    }
  } else {
    _mountpoints = mountpoints_r;
    newcount();
  }
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const set<PkgDuMaster::MountPoint> & obj )
{
  for ( set<PkgDuMaster::MountPoint>::const_iterator it = obj.begin(); it != obj.end(); ++it ) {
    str << *it << endl;
  }
  return str;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const PkgDuMaster & obj )
{
  str << "--[" << obj._total <<  "]----------------------------" << endl;
  str << obj._mountpoints;
  str << "---------------------------------" << endl;
  return str;
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgDu
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDu::PkgDu
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PkgDu::PkgDu()
    : _count( 0 )
    , _data( 0 )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDu::~PkgDu
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PkgDu::~PkgDu()
{
  delete [] _data;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDu::sync
//	METHOD TYPE : bool
//
//	DESCRIPTION : Assert not to return true, on NULL _data or
//                    an array size != master_r._mountpoints.size().
//
//                    - Whenever the masters set of mountpoints changes,
//                    the masters sync_count() changes. If the _count
//                    remembered here does not match the masters sync_count
//                    _data must be rebuilt according to masters new set of
//                    mountpoints. And we remember the new sync_count.
//
//                    - If masters set of mountpoints is not empty and
//                    pkg provides dudata, we must set up _data as an
//                    array of FSize. Size of the array must be equal
//                    to the size of masters set of mountpoints. The
//                    FSize values placed in _data must correspond to
//                    the sequence in which the masters set of mountpoints
//                    is iterated:
//
//                    master_r.mountpoints().begin() ->  "/"     <-  _data[0]
//                                                   ->  "/usr"  <-  _data[1]
//                                                   ->  "/varr" <-  _data[2]
//
//                    - The list<string> of dudata provided by a package looks
//                    like this:
//
//                    /                       0 5156 0 444
//                    etc/                    0   13 0   5
//                    etc/WindowMaker/       13    0 5   0
//                    ...
//
//                    1st col: directory path
//                    2nd col: ammount of byte (in kB) stored in directory
//                    3rd col: ammount of byte (in kB) stored below directory
//                    4th col: number of files created in in directory
//                    5th col: number of files created below directory
//
//                    It's asserted, that for every directory path that occurs
//                    in the list, an entry for each of it's parent directories
//                    exists too.
//
//                    In the above example the package would install 5156 kB
//                    in 444 files. No files reside in / and /etc. In
//                    /etc/WindowMaker 5 files with total size 13 kB; no files
//                    in any directory below. ...
//

struct Tentry {
  unsigned _idx;
  unsigned _fcnt; // file count
  FSize    _bsze; // blocksize
  FSize    _fsze; // pure file sizes
  Tentry() {/*empty - used by std::map only*/}
  Tentry( unsigned idx_r, const FSize & bsze_r ) { _idx = idx_r; _fcnt = 0; _bsze = bsze_r; _fsze = 0; }
  FSize total() const { return _fsze + ( _bsze * _fcnt / 2 ); }
  void  sub( const Tentry & rhs ) {
    _fcnt -= rhs._fcnt;
    _fsze -= rhs._fsze;
  }
};

bool PkgDu::sync( const PMPackage & pkg_r, PkgDuMaster & master_r ) const
{
  if ( _count != master_r.sync_count() ) {
    delete [] _data;
    _data  = 0;
    _count = master_r.sync_count();

    // now see if there are actual data to contribute
    if ( master_r.mountpoints().size() ) {
      list<string> dudata( pkg_r.du() );
      if ( dudata.size() ) {
	///////////////////////////////////////////////////////////////////
	// so we've got mountpoints and a list of du data.
	///////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////
	// tokmap maps mountpoints to _data array indices, according to
	// master_r.mountpoints() iterator sequence. i.e. data for '/'
	// have to be stored in _data[tokmap[/]].
	///////////////////////////////////////////////////////////////////
	typedef map<string,Tentry> TokMap;
	TokMap   tokmap;
	unsigned tmidx = 0;
	for ( set<MountPoint>::const_iterator mp = master_r.mountpoints().begin();
	      mp != master_r.mountpoints().end(); ++mp, ++tmidx ) {
	  tokmap[mp->_mountpoint] = Tentry( tmidx, mp->_blocksize );

	}

	///////////////////////////////////////////////////////////////////
	// scan the dudata and store mountpoint data
	///////////////////////////////////////////////////////////////////
	unsigned tofind = master_r.mountpoints().size(); // stop if feed complete
	for ( list<string>::iterator it = dudata.begin(); tofind && it != dudata.end(); ++it ) {

	  // *it contains just the 4 numbers afterwards:
	  string tok = stringutil::stripFirstWord( *it, /*ltrim_first*/true );
	  if ( tok.empty() )
	    continue;

	  if ( tok.size() != 1 ) {
	    // add leading, remove trailing '/'
	    if ( tok[0] != '/' )
	      tok.insert( 0, 1, '/' );
	    if ( tok[tok.size()-1] == '/' )
	      tok.erase( tok.size()-1 );
	  }

	  TokMap::iterator found = tokmap.find( tok );
	  if ( found != tokmap.end() ) {
	    vector<string> tdata;
	    if ( stringutil::split( *it, tdata ) == 4 ) {
	      found->second._fsze = FSize( atoi( tdata[0].c_str() ) + atoi( tdata[1].c_str() ),
					   FSize::K );
	      found->second._fcnt = atoi( tdata[2].c_str() ) + atoi( tdata[3].c_str() );
	      --tofind;
	    } else {
	      WAR << "Illegal DU entry: " << tok << ' ' << *it << endl;
	    }
	  }

	} // for

	///////////////////////////////////////////////////////////////////
	// The rest looks more complicated, than it is. ;)
	// TokMap is sorted by mountpoints, and holds the duinfo for each
	// mountpoint. In fact mountpoints are a tree and we have to subtract
	// each points data from its parents.
	///////////////////////////////////////////////////////////////////

	_data = new FSize[master_r.mountpoints().size()]; // initial values == 0

	for ( TokMap::reverse_iterator it = tokmap.rbegin(); it != tokmap.rend(); ++it ) {
	  if ( ! it->second._fsze )
	    continue; // nothing to do

	  // mount points has size, remove it from parents
	  TokMap::reverse_iterator up = it;
	  for ( ++up; up != tokmap.rend(); ++up ) {
	    if ( it->first.find( up->first ) == 0 ) {
	      up->second.sub( it->second );
	    }
	  }

	  // insert in _data
	  _data[it->second._idx] = it->second.total();
	}

      }
    }
  }
  return _data;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDu::add
//	METHOD TYPE : bool
//
//	DESCRIPTION : Assert not to pass NULL or
//                    an array size != master_r._mountpoints.size()
//
bool PkgDu::add( const PMPackage & pkg_r, PkgDuMaster & master_r ) const
{
  if ( sync( pkg_r, master_r ) ) {
    master_r.add( _data );
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgDu::sub
//	METHOD TYPE : bool
//
//	DESCRIPTION : Assert not to pass NULL or
//                    an array size != master_r._mountpoints.size()
//
bool PkgDu::sub( const PMPackage & pkg_r, PkgDuMaster & master_r ) const
{
  if ( sync( pkg_r, master_r ) ) {
    master_r.sub( _data );
    return true;
  }
  return false;
}

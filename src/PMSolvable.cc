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

   File:       PMSolvable.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <y2pm/PMSolvable.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSolvable
//
///////////////////////////////////////////////////////////////////

IMPL_HANDLES(PMSolvable);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSolvable::PMSolvable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMSolvable::PMSolvable()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSolvable::PMSolvable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMSolvable::PMSolvable( const PkgName& name,
			const PkgEdition& edition )
      : _name(name), _edition(edition)
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSolvable::~PMSolvable
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMSolvable::~PMSolvable()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSolvable::dumpOn
//	METHOD TYPE : std::ostream &
//
//	DESCRIPTION :
//
ostream & PMSolvable::dumpOn( ostream & os ) const
{
  Rep::dumpOn( os );
  os << "Name: " << (const char *)_name << endl;
  os << "Version: " << _edition.version() << endl;
  if (_edition.has_release())
    os << "Release: " << _edition.release() << endl;
  if (_edition.has_epoch())
    os << "Epoch: " << _edition.epoch() << endl;

  if (_requires.size())
    os << "Requires: " << _requires << endl;
  if (_conflicts.size())
    os << "Conflicts: " << _conflicts << endl;
  if (_provides.size())
    os << "Provides: " << _provides << endl;
  if (_obsoletes.size())
    os << "Obsoletes: " << _obsoletes << endl;

  //	os << "Size: " << _size << endl;

#if 0
  for( unsigned i = 0; i < Package::N_STRS; ++i ) {
    if (!pkg.strings[i] || pkg.strings[i]->empty())
      continue;
    // we need a named object for the tag name here, because egcs-2.91.66
    // otherwise gets confused with the temporary string and a SIGSEGV is
    // the result.
    string tagn = Tag::tagname(Package::str_tags[i]);
    os << tagn << ": " << modify_newlines(*pkg.strings[i]) << endl;
  }
  for( unsigned i = 0; i < Package::N_BINS; ++i ) {
    if (!pkg.binfields[i] || !pkg.binfields[i]->contents)
      continue;
    // we need a named object for the tag name here, because egcs-2.91.66
    // otherwise gets confused with the temporary string and a SIGSEGV is
    // the result.
    string tagn = Tag::tagname(Package::bin_tags[i]);
    os << tagn << ": "<< hex << setfill('0');
    for( unsigned j = 0; j < pkg.binfields[i]->size; ++j ) {
      os << setw(2) << (unsigned)(pkg.binfields[i]->contents[j]);
    }
    os << dec << setfill(' ') << endl;
  }
#endif
  return os;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream& operator<<( ostream& os, const PMSolvable::PkgRelList_type& rl )
{
  for( PMSolvable::PkgRelList_const_iterator q = rl.begin();
       q != rl.end(); ++q ){
    if (q != rl.begin())
      cout << ", ";
    cout << *q;
  }
  return os;
}

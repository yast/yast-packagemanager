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

#include <iostream>
#include <sstream>

#include <y2util/Y2SLog.h>

#include <y2pm/PMSolvable.h>
#include <y2pm/InstTarget.h>
#include <Y2PM.h>

using namespace std;

#warning Think about using PkgRelation set instead of list.

///////////////////////////////////////////////////////////////////
// static public
//	METHOD NAME : InstSrcDataUL::Tag2PkgRelList
//	METHOD TYPE : int
//
//	DESCRIPTION : convert list of strings (denoting dependencies)
//		      to PMSolvable::PkgRelList_type&
//		      return number of dependencies found
std::list<std::string>
PMSolvable::PkgRelList2StringList ( const PkgRelList_type & rellist_r )
{
    std::list<std::string> ret;
    for( PMSolvable::PkgRelList_const_iterator it = rellist_r.begin(); it != rellist_r.end(); ++it )
    {
	ret.push_back( it->asString() );
    }
    return ret;
}

///////////////////////////////////////////////////////////////////
// static public
//	METHOD NAME : PMSolvable::StringList2PkgRelList
//	METHOD TYPE : PkgRelList_type
//
//	DESCRIPTION : convert list of strings (denoting dependencies)
//		      to PkgRelList_type
//
PMSolvable::PkgRelList_type
PMSolvable::StringList2PkgRelList ( const list<string>& relationlist,
				    const PkgName &     forPkg,
				    const std::string & kind )
{
    PkgRelList_type pkgrellist;

    if (!relationlist.empty())
    {
	for (list<string>::const_iterator it = relationlist.begin(); it != relationlist.end(); ++it)
	{
	  PkgRelation newrel = PkgRelation::fromString (*it);
	  // ignore provides/conflicts/obsoletes/requires on own name
	  if ( forPkg->size() && newrel.name() == forPkg ) {
	    _I__("DEPCHECK") << forPkg << " self " << kind << ": " << newrel << endl;
	  }
	  else
	  {
	      pkgrellist.push_back (newrel);
	  }
	}
    }
    return pkgrellist;
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSolvable
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(PMSolvable);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSolvable::PMSolvable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMSolvable::PMSolvable( const PkgName& name,
			const PkgEdition& edition,
			const PkgArch& arch )
      : _name(name), _edition(edition), _arch(arch)
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
//	METHOD NAME : PMSolvable::addPreRequires
//	METHOD TYPE : const PMSolvable::PkgRelList_type&
//
//	DESCRIPTION : copy pre-requires to requires list and mark them
//
const PMSolvable::PkgRelList_type& PMSolvable::addPreRequires(PMSolvable::PkgRelList_type& prerequires)
{
    // walk through requires
    for (PkgRelList_type::iterator rit=_requires.begin();
	    rit != _requires.end(); ++rit)
    {
	// walk through new prerequires
	for (PkgRelList_type::iterator pit=prerequires.begin();
		pit != prerequires.end(); ++pit)
	{
	    if(*pit == *rit)
	    {
		rit->setPreReq(true);

		prerequires.erase(pit);
		break;
	    }
	}
    }

    return _requires;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSolvable::prerequires
//	METHOD TYPE : PMSolvable::PkgRelList_type
//
//	DESCRIPTION :
//
PMSolvable::PkgRelList_type PMSolvable::prerequires() const
{
    PkgRelList_type newlist;

    for (PkgRelList_type::const_iterator rit=_requires.begin();
	    rit != _requires.end(); ++rit)
    {
	if(rit->isPreReq())
	{
	    newlist.push_back(*rit);
	}
    }

    return newlist;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSolvable::doesProvide
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMSolvable::doesProvide(const PkgRelation& rel) const
{
    if(rel.matches(self_provides()))
    {
	D__ << rel << " self-provides" << endl;
	return true;
    }
    for(PkgRelList_const_iterator it = _provides.begin();
	it != _provides.end(); ++it)
    {
	if(rel.matches(*it))
	{
	    D__ << *it << " matcheXXs " << rel << endl;
	    return true;
	}
    }
    return false;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSolvable::doesObsolete
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMSolvable::doesObsolete( const constPMSolvablePtr & item_r ) const
{
  if ( item_r ) {
    for ( PkgRelList_const_iterator it = obsoletes_begin(); it != obsoletes_end(); ++it ) {
      if ( it->matches( item_r ) ) {
	return true;
      }
    }
  }
  return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSolvable::traceFileRel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMSolvable::traceFileRel( const PkgRelation & rel_r ) const
{
  if ( ! rel_r.isFileRel() )
    return;
  Y2PM::instTarget().traceFileRel( rel_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSolvable::dumpOn
//	METHOD TYPE : std::ostream &
//
//	DESCRIPTION :
//
ostream & PMSolvable::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str << '{' << nameEdArch() << '}';
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
      os << ", ";
    os << *q;
  }
  return os;
}

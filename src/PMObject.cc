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

   File:       PMObject.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/PMObject.h>
#include <y2pm/PMSelectable.h>
#include <y2pm/PMDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMObject
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER( PMObject, PMSolvable, PMSolvable );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMObject::PMObject
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMObject::PMObject( const PkgName &    name_r,
		    const PkgEdition & edition_r,
		    const PkgArch &    arch_r )
    : PMSolvable(name_r, edition_r, arch_r)
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMObject::~PMObject
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMObject::~PMObject()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMObject::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMObject::dumpOn( ostream & str ) const
{
  PMSolvable::dumpOn( str );
  str << '(' << (_selectable ? "MANAGED" : "unmanaged" ) << ')' << endl;
  return str;
}

///////////////////////////////////////////////////////////////////
//
// Shortcuts for UI
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMObject::isInstalledObj
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMObject::isInstalledObj() const
{
  return( hasSelectable() && _selectable->installedObj() == this );
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : PMObject::isCandidateObj
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMObject::isCandidateObj() const
{
  return( hasSelectable() && _selectable->candidateObj() == this );
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : PMObject::isAvailableOnly
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMObject::isAvailableOnly() const
{
  return( hasSelectable()
	  && _selectable->installedObj() != this
	  && _selectable->candidateObj() != this );
}



///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : PMObject::getInstalledObj
//	METHOD TYPE : PMObjectPtr
//
//	DESCRIPTION :
//
//  Convenience method: Retrieve the installed instance of this
//  selectable. This may be a brother of this object or this object itself
//  or 0 (if there is no installed instance or if this object doesn't have a
//  selectable - in which case something has gone wrong badly anyway).

PMObjectPtr PMObject::getInstalledObj() const
{
    if ( _selectable )
	return _selectable->installedObj();
    else
	return PMObjectPtr();	// Null pointer
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : PMObject::getCandidateObj
//	METHOD TYPE : PMObjectPtr
//
//	DESCRIPTION :
//
//  Convenience method: Retrieve the candidate instance of this
//  selectable. This may be a brother of this object or this object itself
//  or 0 (if there is no candidate instance or if this object doesn't have a
//  selectable - in which case something has gone wrong badly anyway).

PMObjectPtr PMObject::getCandidateObj() const
{
    if ( _selectable )
	return _selectable->candidateObj();
    else
	return PMObjectPtr();	// Null pointer
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : PMObject::hasInstalledObj
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
// Convenience method: Check if there is any installed instance of this
// selectable - this instance or any of its brothers.
// Not to be confused with isInstalledObj() !

bool PMObject::hasInstalledObj() const
{
    return _selectable && _selectable->installedObj();
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : PMObject::hasInstalledObj
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
// Convenience method: Check if there is any candidate instance of this
// selectable - this instance or any of its brothers.
// Not to be confused with isInstalledObj() !

bool PMObject::hasCandidateObj() const
{
    return _selectable && _selectable->candidateObj();
}




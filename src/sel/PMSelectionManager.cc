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

  File:       PMSelectionManager.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/LangCode.h>
#include <Y2PM.h>
#include <y2pm/PMSelectionManager.h>
#include <y2pm/InstTarget.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::PMSelectionManager
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMSelectionManager::PMSelectionManager()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::~PMSelectionManager
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMSelectionManager::~PMSelectionManager()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::assertObjectType
//	METHOD TYPE : PMObjectPtr
//
//	DESCRIPTION :
//
PMObjectPtr PMSelectionManager::assertObjectType( const PMObjectPtr & object_r ) const
{
  PMSelectionPtr p( object_r );
  if ( object_r && !p ) {
    ERR << "Object is not a Selection: " << object_r << endl;
  }
  return p;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const PMSelectionManager & obj )
{
    str << "PMSelectionManager" << endl;
    return str;
}

/******************************************************************
**
**
**	FUNCTION NAME : getAlternativeSelectable
**	FUNCTION TYPE : PMSelectablePtr
**
**	DESCRIPTION : get first matching alternative selection of given
**		      "pack (alt1, alt2, ...)" string
*/
PMSelectablePtr
PMSelectionManager::getAlternativeSelectable (std::string pkgstr,
					 PMPackageManager & package_mgr)
{
    PMSelectablePtr selectable;
    string::size_type startpos = pkgstr.find_first_of (" ");
    if (startpos == string::npos)
	return selectable;

    // try initial pack
    selectable = package_mgr.getItem (pkgstr.substr (0, startpos));
    if (!selectable)
    {
	// nope, try "(alt1, ...)"
	startpos = pkgstr.find_first_of ("(");
	string::size_type endpos = pkgstr.find_first_of (")");
	if ((startpos != string::npos)
	    && (endpos != string::npos))
	{
	    startpos++;
	    while (startpos < endpos)
	    {
		// find first comma or blank after startpos
		string::size_type commapos = pkgstr.find_first_of (", ", startpos);
		if (commapos == string::npos)
		    commapos = endpos;
		selectable = package_mgr.getItem (pkgstr.substr (startpos, commapos-startpos));
		if (selectable)
		    break;			// found it !
		startpos = commapos + 1;
	    }
	}
    }
    return selectable;
}


/******************************************************************
**
**
**	FUNCTION NAME : setSelectionPackages
**	FUNCTION TYPE : void
**
**	DESCRIPTION : set all packages of the given list to "auto install"
**
**		if these_are_delpacks == true, the package list is an
**		'delpacks' list and the selection just got de-activated.
**		In this case reset auto-delete packages to "unmodified"
*/
void
PMSelectionManager::setSelectionPackages (const std::list<std::string> packages,
					 bool these_are_delpacks,
					 PMPackageManager & package_mgr)
{
    for (std::list<std::string>::const_iterator it = packages.begin();
	 it != packages.end(); ++it)
    {
	// get selectable by name

	PMSelectablePtr selectable = package_mgr.getItem (*it);

	if (!selectable)
	{
	    // not found, try alternative packages
	    selectable = getAlternativeSelectable (*it, package_mgr);
	}

	if (selectable)
	{
	    if (these_are_delpacks)
	    {
		// de-activate delpacks
		// only 'to_delete' packages not 'by_user'
		selectable->appl_set_delete();
	    }
	    else
	    {
	      if ( !selectable->has_both_objects()
		   || selectable->installedObj()->edition() != selectable->candidateObj()->edition() )
	      {
		selectable->appl_set_install();
	      }
	    }
	}
    }
    return;
}

/******************************************************************
**
**
**	FUNCTION NAME : resetSelectionPackages
**	FUNCTION TYPE : void
**
**	DESCRIPTION : set all packages of the given list to "delete"
**		if their status is "auto" (i.e. not explicitly requested
**		by the user).
**
**		if these_are_inspacks == true, the package list is an
**		'inspacks' list and the selection just got de-activated.
**		In this case reset auto-install packages to "unmodified"
*/
void
PMSelectionManager::resetSelectionPackages (const std::list<std::string> packages,
					bool these_are_inspacks,
					PMPackageManager & package_mgr)
{
    for (std::list<std::string>::const_iterator it = packages.begin();
	 it != packages.end(); ++it)
    {
	// get selectable by name

	PMSelectablePtr selectable = package_mgr.getItem (*it);

	if (!selectable)
	{
	    // not found, try alternative packages
	    selectable = getAlternativeSelectable (*it, package_mgr);
	}

	if (selectable)
	{
	    selectable->appl_unset();
	}
    }
    return;
}

/******************************************************************
**
**
**	FUNCTION NAME : setSelection
**	FUNCTION TYPE : void
**
**	DESCRIPTION : set all packages of this selection to "auto"
*/
void
PMSelectionManager::setSelection (PMSelectionPtr selection, PMPackageManager & package_mgr)
{
    if (!selection)
	return;
MIL << "setSelection " << selection->name() << "." << _currently_preferred_locale << endl;
    // first, the delpacks
    setSelectionPackages (selection->delpacks(), true, package_mgr);
    if (!((const std::string &)_currently_preferred_locale).empty())
    {
	setSelectionPackages (selection->delpacks(_currently_preferred_locale), true, package_mgr);
    }
    for (std::list<LangCode>::iterator it = _currently_requested_locales.begin();
	 it != _currently_requested_locales.end(); ++it)
    {
	setSelectionPackages (selection->delpacks(*it),
			true,		// these_are_delpacks
			package_mgr);
    }

    // then, the inspacks
    setSelectionPackages (selection->inspacks(), false, package_mgr);
    if (!((const std::string &)_currently_preferred_locale).empty())
    {
	setSelectionPackages (selection->inspacks(_currently_preferred_locale), false, package_mgr);
    }
    for (std::list<LangCode>::iterator it = _currently_requested_locales.begin();
	 it != _currently_requested_locales.end(); ++it)
    {
	setSelectionPackages (selection->inspacks(*it),
			false,		// !these_are_delpacks
			package_mgr);
    }
    return;
}

/******************************************************************
**
**
**	FUNCTION NAME : resetSelection
**	FUNCTION TYPE : void
**
**	DESCRIPTION : set all packages of this selection to "don't install"
**		if their status is "auto" (i.e. not explicitly requested
**		by the user).
*/
void
PMSelectionManager::resetSelection (PMSelectionPtr selection, PMPackageManager & package_mgr)
{
    if (!selection)
	return;

MIL << "resetSelection " << selection->name() << "." << _currently_preferred_locale << endl;

    // get list of requested locales

    // first, the inspacks
    resetSelectionPackages (selection->delpacks(), true, package_mgr);
    if (!((const std::string &)_currently_preferred_locale).empty())
    {
	resetSelectionPackages (selection->delpacks(_currently_preferred_locale), true, package_mgr);
    }
    for (std::list<LangCode>::iterator it = _currently_requested_locales.begin();
	 it != _currently_requested_locales.end(); ++it)
    {
	resetSelectionPackages (selection->delpacks(*it),
			true,		// these_are_inspacks
			package_mgr);
    }

    // then, the inspacks
    resetSelectionPackages (selection->inspacks(), false, package_mgr);
    if (!((const std::string &)_currently_preferred_locale).empty())
    {
	resetSelectionPackages (selection->inspacks(_currently_preferred_locale), false, package_mgr);
    }
    for (std::list<LangCode>::iterator it = _currently_requested_locales.begin();
	 it != _currently_requested_locales.end(); ++it)
    {
	resetSelectionPackages (selection->inspacks(*it),
			false,		// !these_are_delpacks
			package_mgr);
    }
    return;
}


/******************************************************************
**
**
**	FUNCTION NAME : activate
**	FUNCTION TYPE : PMError
**
**	DESCRIPTION : activate all "selected" selections by
**			going through all their packages and
**			setting them to "selected"
*/
PMError
PMSelectionManager::activate (PMPackageManager & package_mgr)
{
    MIL << "PMSelectionManager::activate()" << endl;
    if (_currently_actives.size() > 0)
    {
	MIL << "resetting " << _currently_actives.size() << " active selections" << endl;
	// we currently have active selections.
	// loop through all of them and de-select packages
	// which aren't in the new set

	for (std::list<PMSelectablePtr>::iterator active = _currently_actives.begin();
	     active != _currently_actives.end(); ++active)
	{
	    if (!(*active)->to_install())
	    {
		// this one isn't active any more

		resetSelection ((*active)->candidateObj(), package_mgr);
	    }
	}
	_currently_actives.clear();
    }

    // now activate the new set of selections

    _currently_preferred_locale = Y2PM::getPreferredLocale();
    _currently_requested_locales = Y2PM::getRequestedLocales();

    for (PMSelectableVec::const_iterator it = begin();
	 it != end(); ++it)
    {
	if ((*it)->to_install())
	{
	    setSelection ((*it)->candidateObj(), package_mgr);
	    _currently_actives.push_back (*it);
	}
    }
    MIL << _currently_actives.size() << " are active now" << endl;

    return installOnTarget();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::installOnTarget
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError PMSelectionManager::installOnTarget() const
{
  return PMError::E_ok;
}


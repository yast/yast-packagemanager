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

   File:       PMPackageManager.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/Pathname.h>

#include <y2pm/PMPackageManager.h>
#include <y2pm/InstallOrder.h>

#include <Y2PM.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::PMPackageManager
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMPackageManager::PMPackageManager()
{
  _rpmGroupsTree = new YRpmGroupsTree();

#if 0
  _rpmGroupsTree->addFallbackRpmGroups();
#endif
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::~PMPackageManager
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMPackageManager::~PMPackageManager()
{
  if ( _rpmGroupsTree )
    delete _rpmGroupsTree;

  MIL << "PMPackageManager::~PMPackageManager()" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::assertObjectType
//	METHOD TYPE : PMObjectPtr
//
//	DESCRIPTION :
//
PMObjectPtr PMPackageManager::assertObjectType( const PMObjectPtr & object_r ) const
{
  PMPackagePtr p( object_r );
  if ( object_r && !p ) {
    ERR << "Object is not a Package: " << object_r << endl;
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
ostream & operator<<( ostream & str, const PMPackageManager & obj )
{
  str << "PMPackageManager" << endl;
  return str;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::anythingByUser
//	METHOD TYPE : bool
//
//	DESCRIPTION :are there currently any "by_user" selectables ?
//
bool
PMPackageManager::anythingByUser(void)
{
    for ( PMSelectableVec::iterator it = begin(); it != end(); ++it )
    {
	if ((*it)->by_user())
	    return true;
    }
    return false;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::anythingToDelete
//	METHOD TYPE : void
//
//	DESCRIPTION :are there currently any selectables to delete ?
//
bool
PMPackageManager::anythingToDelete (void)
{
    for ( PMSelectableVec::iterator it = begin(); it != end(); ++it )
    {
	if ((*it)->to_delete())
	    return true;
    }
    return false;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::anythingToInstall
//	METHOD TYPE : void
//
//	DESCRIPTION :are there currently any selectables to install ?
//
bool
PMPackageManager::anythingToInstall (void)
{
    for ( PMSelectableVec::iterator it = begin(); it != end(); ++it )
    {
	if ((*it)->to_install())
	    return true;
    }
    return false;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::getPackagesToInsDel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMPackageManager::getPackagesToInsDel( std::list<PMPackagePtr> & dellist_r,
					    std::list<PMPackagePtr> & instlist_r )
{
    dellist_r.clear();
    instlist_r.clear();

    DBG << "getPackagesToInsDel..." << endl;

    for ( PMSelectableVec::iterator it = begin(); it != end(); ++it )
    {
	const PMSelectablePtr & sel( *it );

	if ( !sel )
	{
	    INT << "NULL SELECTABLE" << endl;
	    continue;
	}

	if (sel->to_install())
	{
	    if ( sel->candidateObj() )
	    {
		// unordered list for backup
		instlist_r.push_back( sel->candidateObj() );
	    }
	    else
	    {
		INT << "NULL candidate to install" << endl;
	    }
	}
	else if (sel->to_delete())
	{
	    if ( sel->installedObj() )
	    {
		dellist_r.push_back( sel->installedObj() );
	    }
	    else
	    {
		INT << "NULL installed to delete" << endl;
	    }
	    break;
	}
    }

    DBG << "num packages: delete " << dellist_r.size() << ", install " << instlist_r.size() << endl;

    ///////////////////////////////////////////////////////////////////
    // sort installed list.
    ///////////////////////////////////////////////////////////////////

    // backup list
    std::list<PMPackagePtr> instbackup_r;
    instbackup_r.swap( instlist_r );

    if ( instlist_r.size() )
    {
	// oops, swap() is supposed to empty it's argument
	INT << "DONT swap lists" << endl;
	instlist_r.clear();
    }

#warning check source priority and media count

    // loop over medias
    for (unsigned int cdnum = 1; cdnum < 10; ++cdnum)
    {
	PkgSet iset; // for install order

	// loop over packages, adding matchin medianr to set
	for (std::list<PMPackagePtr>::const_iterator pkgIt = instbackup_r.begin();
	     pkgIt != instbackup_r.end(); ++pkgIt)
	{
	    if ((*pkgIt)->medianr() == cdnum)
	    {
		iset.add (*pkgIt);
	    }
	}

	// matching medianr found -> solve
	if (!iset.empty())
	{
	    InstallOrder order( iset );

	    // start recursive depth-first-search
	    order.startrdfs();

	   for (InstallOrder::SolvableList pkgs = order.computeNextSet();
		!pkgs.empty(); pkgs = order.computeNextSet() )
	   {

		for (InstallOrder::SolvableList::const_iterator cit = pkgs.begin();
		     cit != pkgs.end(); ++cit )
		{
		    PMPackagePtr cpkg = PMPackagePtr::cast_away_const( *cit );

		    if ( !cpkg )
		    {
			INT << "SORT returned NULL Package" << endl;
			continue;
		    }

		    instlist_r.push_back( cpkg );
		    order.setInstalled( *cit );
		}
	    }
	} // ! iset.empty
    } // cdnum loop

    if ( instbackup_r .size() != instlist_r.size() )
    {
	INT << "Lost packages in InstallOrder sort." << endl;
    }

}


static bool
suse_vendor (constPMPackagePtr package)
{
    if (!package)
	return true;

    string vendor = package->vendor();
    if (vendor.empty()
	|| (vendor.size() < 4)
	|| (vendor.substr (0,4) != "SuSE"))
    {
	DBG << "vendor '" << vendor << "'" << endl;
	return false;
    }
    return true;
}



///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::doUpdate
//	METHOD TYPE : void
//
//	DESCRIPTION : go through all installed (but not yet touched by user)
//		packages and look for update candidates
//		Handle splitprovides
//		Mark packages appl_delete or appl_install accordingly
//
//		return number of packages affected
//		return non-suse packages for which an update candidate exists in noinstall_r
//		return non-suse packages for which an obsolete exists in nodelete_r
//
int
PMPackageManager::doUpdate (std::list<PMPackagePtr>& noinstall_r, std::list<PMPackagePtr>& nodelete_r)
{
    int count = 0;
    int i = 0;
    noinstall_r.clear();
    nodelete_r.clear();

    DBG << "doUpdate..." << size() << " selectables" << endl;

    for ( PMSelectableVec::iterator it = begin(); it != end(); ++it )
    {
	++i;
	DBG << endl << i << ". " << (*it)->name() << ": ";

	//-----------------------------------------------------------------
	// pre check: candidate ? taboo ? user ? non-suse ?

	PMPackagePtr candidate = (*it)->candidateObj();
	if (!candidate)
	{
	    continue;
	}
	if ((*it)->is_taboo())			// skip taboo
	{
	    DBG << "taboo";
	    continue;
	}
	if ((*it)->by_user())			// skip user
	{
	    DBG << "user";
	    continue;
	}

	PMPackagePtr installed = (*it)->installedObj();
	if (!installed)
	{
	    DBG << "not installed";
	}

	//-----------------------------------------------------------------
	// check splitprovides

	const list<string> splitprovides = candidate->splitprovides();
	for (list<string>::const_iterator splitit = splitprovides.begin();
	     splitit != splitprovides.end(); ++splitit)
	{
	    // *splitit = "rpmname:/path/to/file"
	    DBG << "split ? '" << *splitit << "'" <<endl;

	    string::size_type colonpos = (*splitit).find (":");
	    if (colonpos == string::npos)
	    {
		ERR << "Bad split !";
		continue;
	    }

	    // check if rpm exists in target which provides the file
	    // compare only the rpm name, not the edition

	    string name = (*splitit).substr (0, colonpos);
	    string rpmname = Y2PM::instTarget().belongsTo (Pathname ((*splitit).substr (colonpos+1)), false);	// name only
	    DBG << "'" << name << "' matches '" << rpmname << "' ?" << endl;
	    if (name == rpmname)		// name matches
	    {
		DBG << "Yes !";
		if (suse_vendor (installed))
		{
		    (*it)->appl_set_install ();
		    count++;
		}
		else
		    noinstall_r.push_back (installed);
		break;
	    }

	}

	//-----------------------------------------------------------------
	// check for newer candidate


#warning TDB checks edition only
	if (installed)
	{
	    if (installed->edition() < candidate->edition())
	    {
		DBG << "Edition " << PkgEdition::toString(installed->edition()) << " < " << PkgEdition::toString(candidate->edition());
		if (suse_vendor (installed))
		{
		    (*it)->appl_set_install ();
		    count++;
		}
		else
		    noinstall_r.push_back (installed);
	    }
	    else if (installed->buildtime() < candidate->buildtime())
	    {
		DBG << "Edition " << PkgEdition::toString(installed->edition()) << " ? " << PkgEdition::toString(candidate->edition()) << endl;
		DBG << "Buildtime " << installed->buildtime() << " < " << candidate->buildtime();
		if (suse_vendor (installed))
		{
		    (*it)->appl_set_install ();
		    count++;
		}
		else
		    noinstall_r.push_back (installed);
	    }
	}

	//-----------------------------------------------------------------
	// check for obsoletes

	const PMSolvable::PkgRelList_type obsoletes = candidate->obsoletes();
	for (PMSolvable::PkgRelList_const_iterator obsit = obsoletes.begin();
	     obsit != obsoletes.end(); ++obsit)
	{
	    PMSelectablePtr obsslc = getItem (obsit->name());
	    if (obsslc							// obsoletes is known
		&& (obsslc->has_installed())				// and is installed
		&& (!(obsslc->is_taboo() || obsslc->by_user())))	// and to taboo or set by user
	    {

		PMPackagePtr installed = obsslc->installedObj();

		if (!(*it)->to_install())		// if not selected for installion yet
		{
		    // look for a matching provides

		    const PMSolvable::PkgRelList_type provides = candidate->provides();
		    for (PMSolvable::PkgRelList_const_iterator prvit = provides.begin();
			 prvit != provides.end(); ++prvit)
		    {
			if ((*obsit) != (*prvit))
			    continue;

		        DBG << "matching provides";

			if (suse_vendor (installed))
			{
			    (*it)->appl_set_install ();
			    count++;
			}
			else
			    noinstall_r.push_back (installed);
		    } // provides loop

		} // not yet installed
	
		// re-test install flag, the above provides check might have changed it

		if ((*it)->to_install())		// if selected for installion
		{					// remove obsoletes
		    DBG << "delete!";
		    if (suse_vendor (installed))
		    {
			obsslc->appl_set_delete();
			count++;
		    }
		    else
			nodelete_r.push_back (installed);
		}

	    } // obsoletes is installed

	} // obsoletes loop

    } // selectable loop

    return count;
}

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
void
PMPackageManager::doUpdate (std::list<PMPackagePtr>& noupdate_r, std::list<PMPackagePtr>& unknowns_r)
{
    noupdate_r.clear();
    unknowns_r.clear();

    DBG << "doUpdate..." << endl;

    for ( PMSelectableVec::iterator it = begin(); it != end(); ++it )
    {
	DBG << (*it)->name() << ": ";

	//-----------------------------------------------------------------
	// pre check: candidate ? taboo ? user ? non-suse ?

	if ((*it)->has_installed_only())
	{
	    DBG << "no candidate" << endl;
	    noupdate_r.push_back ((*it)->installedObj());
	    continue;
	}
	if ((*it)->is_taboo())			// skip taboo
	{
	    DBG << "taboo" << endl;
	    continue;
	}
	if ((*it)->by_user())			// skip user
	{
	    DBG << "user" << endl;
	    continue;
	}

	PMPackagePtr installed = (*it)->installedObj();
	string vendor = installed->vendor();
	if (vendor.empty()
	    || (vendor.size() < 4)
	    || (vendor.substr (0,4) != "SuSE"))
	{
	    DBG << "vendor '" << vendor << "'" << endl;
	    unknowns_r.push_back (installed);
	}

	PMPackagePtr candidate = (*it)->candidateObj();
	if (!candidate)
	{
	    DBG << "no candidate" << endl;
	    continue;
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
		ERR << "Bad !" << endl;
		continue;
	    }
	    // check if rpm exists in target which provides the file
	    string rpmname = Y2PM::instTarget().belongsTo (Pathname ((*splitit).substr (colonpos+1)));
	    if (rpmname == (*splitit).substr (0, colonpos))
	    {
		DBG << "Yes !" << endl;
		(*it)->appl_set_install ();
		break;
	    }

	}

	//-----------------------------------------------------------------
	// check for newer candidate

#warning TDB checks edition only
	if (installed->edition() < candidate->edition())
	{
	    DBG << "Edition " << PkgEdition::toString(installed->edition()) << " < " << PkgEdition::toString(candidate->edition()) << endl;
	    (*it)->appl_set_install ();
	}
	else if (installed->buildtime() < candidate->buildtime())
	{
	    DBG << "Buildtime " << installed->buildtime() << " < " << candidate->buildtime() << endl;
	    (*it)->appl_set_install ();
	}

	//-----------------------------------------------------------------
	// check for obsoletes

	const PMSolvable::PkgRelList_type obsoletes = candidate->obsoletes();
	for (PMSolvable::PkgRelList_const_iterator obsit = obsoletes.begin();
	     obsit != obsoletes.end(); ++obsit)
	{
	    DBG << "obsoletes '" << obsit->name() << "' ";
	    PMSelectablePtr obsslc = getItem (obsit->name());
	    if (!obsslc)				// unknown obsolete
	    {
		DBG << "not found" << endl;
		continue;
	    }
	    if (obsslc->has_candidate_only())				// not installed
	    {
		DBG << "not installed" << endl;
		continue;
	    }
	    if (obsslc->is_taboo()
		|| obsslc->by_user())
	    {
		DBG << "taboo/user" << endl;
		continue;
	    }
	    DBG << "delete!" << endl;
	    obsslc->appl_set_delete();

	} // obsoletes loop

    } // selectable loop

    return;
}

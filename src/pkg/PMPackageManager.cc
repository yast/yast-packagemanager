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
#include <y2pm/PkgSet.h>

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
//	METHOD TYPE : void
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
//	METHOD TYPE : int
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
int PMPackageManager::doUpdate( list<PMPackagePtr> & noinstall_r, list<PMPackagePtr> & nodelete_r )
{
  noinstall_r.clear();
  nodelete_r.clear();

  map<string,set<PMPackagePtr> > pass_two;

  MIL << "doUpdate start..." << endl;

  ///////////////////////////////////////////////////////////////////
  // Reset all auto states and build available set
  ///////////////////////////////////////////////////////////////////
  PkgSet available; // candidates available for install (no matter if selected for install or not)

  for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {
    (*it)->auto_unset();

    if ( (*it)->to_delete() ) {
      D__ << "doUpdate available: SKIP to delete " << (*it) << endl;
      continue;
    }

    if ( ! (*it)->has_candidate() ) {
      D__ << "doUpdate available: SKIP no candidate " << (*it) << endl;
      continue;
    }

    // if installed not SuSE -> not available ???
    available.add( (*it)->candidateObj() );

    // get splitted

  }

  DBG << "doUpdate available: " << available.size() << " candidates available " << endl;

  ///////////////////////////////////////////////////////////////////
  // Now iterate installed packages, not selected to delete, and
  // figure out what might be an appropriate replacement.
  ///////////////////////////////////////////////////////////////////
  MIL << "doUpdate pass 1..." << endl;

  for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {

    if ( ! (*it)->has_installed() ) {
      continue;
    }

    if ( (*it)->to_delete() ) {
      DBG << "SKIP to delete: " << (*it)->installedObj() << endl;
      continue;
    }

    PMSelectablePtr state( *it );
    PMPackagePtr    installed( (*it)->installedObj() );
    PMPackagePtr    candidate( (*it)->candidateObj() );

    DBG << "REPLACEMENT FOR " << state << endl;
    //DBG << "REPLACEMENT FOR " << installed << endl;

    // if installed not SuSE -> no action ???

    // Taboo - currently an installed package can't be taboo,
    // but if -> -> no action ???

    ///////////////////////////////////////////////////////////////////
    // figure out replacement
    ///////////////////////////////////////////////////////////////////
    if ( candidate ) {
      if ( ! state->to_install() ) {
	// check for new version
	if ( installed->edition() < candidate->edition() ) {
	  state->appl_set_install();
	  DBG << " ==> REPLACE (new version): " << candidate << endl;
	} else {
	  DBG << " ==> (candidate older)" << endl;
	}
      } else {
	DBG << " ==> REPLACE (is to install): " << candidate << endl;
      }
    } else {
      // if unique provides exists add now, otherwise remember for 2nd pass.
      const PkgSet::RevRelList_type & provided = available.provided()[installed->name()];
      set<PMPackagePtr> mpkg;

      if ( provided.size() ) {
	DBG << "lookup " << provided.size() << " provides for " << installed->name() << endl;

	for( PkgSet::RevRelList_const_iterator pit = provided.begin(); pit != provided.end(); ++pit ) {
	  if ( pit->relation().matches( installed ) ) {
	    DBG << "  relation match: " << pit->relation() << " ==> " << pit->pkg() << endl;
	    mpkg.insert( pit->pkg() );
	  } else {
	    DBG << "  NO relation match: " << pit->relation() << " ==> " << pit->pkg() << endl;
	  }
	}
      }

      switch ( mpkg.size() ) {
      case 0:
	DBG << " ==> (not provided)" << endl;
	break;
      case 1:
        (*mpkg.begin())->getSelectable()->appl_set_install();
        DBG << " ==> ADD (unique provided): " << (*mpkg.begin()) << endl;
	break;
      default:
	pass_two.insert( map<string,set<PMPackagePtr> >::value_type( installed->nameEdArch(), mpkg ) );
	DBG << " ==> pass 2 (" << mpkg.size() << " times provided)" << endl;
	break;
      }
    }

    ///////////////////////////////////////////////////////////////////
    // anyway check for packages split off
    ///////////////////////////////////////////////////////////////////

  }

  ///////////////////////////////////////////////////////////////////
  // Now check the remembered non unique provided. Maybe one of them
  // was somehow selected. Otherwise we have to guess one.
  ///////////////////////////////////////////////////////////////////
  MIL << "doUpdate pass 2..." << endl;

  for ( map<string,set<PMPackagePtr> >::iterator it = pass_two.begin(); it != pass_two.end(); ++it ) {
    DBG << "GET ONE OUT OF " << it->second.size() << " for " << it->first << endl;

    PMPackagePtr guess;
    set<PMPackagePtr> & gset( it->second );
    for ( set<PMPackagePtr>::iterator git = gset.begin(); git != gset.end(); ++git ) {
      if ( (*git)->getSelectable()->to_install() ) {
	DBG << " ==> (meanwhile set to instll): " << (*git) << endl;
	guess = 0;
	break;
      } else {
	// Be prepared to guess.
	// Most common situation for guessing is something like:
	//   qt-devel
	//   qt-devel-experimental
	//   qt-devel-japanese
	// That's why currently the shortest package name wins.
	if ( !guess || guess->name()->size() > (*git)->name()->size() ) {
	  guess = (*git);
	}
      }
    }

    if ( guess ) {
      guess->getSelectable()->appl_set_install();
      DBG << " ==> ADD (guessed): " << guess << endl;
    }
  }

  ///////////////////////////////////////////////////////////////////
  // done
  ///////////////////////////////////////////////////////////////////
  return 0;
}

#if 0
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
#endif

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

  typedef map<string,set<PMPackagePtr> >       PassTwoMap;
  typedef map<PkgSplit,set<PMPackagePtr> >     SplitMap;
  typedef map<PMPackagePtr,set<PMPackagePtr> > SplitPkgMap;

  PassTwoMap  pass_two;
  SplitMap    splitmap;
  SplitPkgMap splitpkgmap;


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

    PMPackagePtr candidate( (*it)->candidateObj() );

    // if installed not SuSE -> not available ???
    available.add( candidate );

    // remember any splitprovides to packages actually installed
    PkgSplitSet splits( candidate->splitprovides() );
    for ( PkgSplitSet::iterator sit = splits.begin(); sit != splits.end(); ++sit ) {
      PMSelectablePtr item = getItem( sit->ipkg() );
      if ( item && item->has_installed() ) {
	splitmap[*sit].insert( candidate );
      }
    }
  }
  MIL << "doUpdate: " << available.size() << " candidates available" << endl;

  MIL << "doUpdate: going to check " << splitmap.size() << " probabely splitted packages" << endl;
  {
    ///////////////////////////////////////////////////////////////////
    // splitmap entries are gouped by ipkg (we know this). So get the
    // filelist as an new ipkg occurres, and use it for consecutive entries.
    //
    // On the fly buld SplitPkgMap from splits that do apply (i.e. file is
    // in ipkg's filelist).
    ///////////////////////////////////////////////////////////////////
    PMPackagePtr cpkg;
    list<string> cflist;
    for ( SplitMap::iterator it = splitmap.begin(); it != splitmap.end(); ++it ) {

      if ( !cpkg || it->first.ipkg() != cpkg->name() ) {
	// acquire new filelist
	cpkg   = getItem( it->first.ipkg() )->installedObj(); // != NULL the way we constructed splitmap
	cflist = cpkg->filenames();
      }

      // lookup
      string lookup( it->first.file().asString() );
      bool   found = false;
      for ( list<string>::iterator fit = cflist.begin(); fit != cflist.end(); ++fit ) {
	if ( *fit == lookup ) {
	  found = true;
	  break;
	}
      }

      if ( !found ) {
	DBG << "  " << it-> first << " does not apply" << endl;
      } else {
	DBG << "  " << it->second.size() << " package(s) for " << it->first << endl;
	splitpkgmap[cpkg].insert( it->second.begin(), it->second.end() );
	DBG << "  split count for " << cpkg->name() << " now " << splitpkgmap[cpkg].size() << endl;
      }
    }

    splitmap.clear();
  }

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
	pass_two[installed->nameEdArch()] = mpkg;
	DBG << " ==> pass 2 (" << mpkg.size() << " times provided)" << endl;
	break;
      }
    }

    ///////////////////////////////////////////////////////////////////
    // anyway check for packages split off
    ///////////////////////////////////////////////////////////////////

    SplitPkgMap::iterator sit = splitpkgmap.find( installed );
    if ( sit != splitpkgmap.end() ) {
      set<PMPackagePtr> & toadd( sit->second );
      if ( !toadd.size() ) {
	INT << "Empty SplitPkgMap entry for " << installed << endl;
      }
      for ( set<PMPackagePtr>::iterator ait = toadd.begin(); ait != toadd.end(); ++ait ) {
	(*ait)->getSelectable()->appl_set_install();
	DBG << " ==> ADD (splited): " << (*ait) << endl;
      }
    }

  }

  ///////////////////////////////////////////////////////////////////
  // Now check the remembered non unique provided. Maybe one of them
  // was somehow selected. Otherwise we have to guess one.
  ///////////////////////////////////////////////////////////////////
  MIL << "doUpdate pass 2..." << endl;

  for ( PassTwoMap::iterator it = pass_two.begin(); it != pass_two.end(); ++it ) {
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

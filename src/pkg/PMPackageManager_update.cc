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

  File:       PMPackageManager_update.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#undef  Y2LOG
#define Y2LOG "PM_Update"

#include <y2pm/PMPackageManager.h>
#include <y2pm/PMPackageManager_update.h>
#include <y2pm/PkgSet.h>

using namespace std;

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

  PMUpdateStats ustats;

  typedef set<PMPackagePtr>            PackageSet;
  typedef map<PkgSplit,PackageSet>     SplitMap;
  typedef map<PMPackagePtr,PackageSet> TodoMap;

  SplitMap    splitmap;
  TodoMap     applyingSplits;
  TodoMap     addSplitted;
  TodoMap     addProvided;
  TodoMap     addMultiProvided;

  MIL << "doUpdate start..." << endl;

  ///////////////////////////////////////////////////////////////////
  // Reset all auto states and build PkgSet of available candidates.
  // On the fly rememeber splitprovides and afterward check, which
  // of them do apply.
  ///////////////////////////////////////////////////////////////////
  PkgSet available; // candidates available for install (no matter if selected for install or not)

  for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {
    (*it)->auto_unset();

    if ( (*it)->to_delete() ) {
      D__ << "doUpdate available: SKIP to delete " << (*it) << endl;
      ++ustats.pre_todel;
      continue;
    }

    if ( ! (*it)->has_candidate() ) {
      D__ << "doUpdate available: SKIP no candidate " << (*it) << endl;
      ++ustats.pre_nocand;
      continue;
    }

    PMPackagePtr candidate( (*it)->candidateObj() );
    ++ustats.pre_avcand;

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
  MIL << "doUpdate: " << ustats.pre_todel  << " packages tagged to delete" << endl;
  MIL << "doUpdate: " << ustats.pre_nocand << " packages without candidate (foreign, renamed or droped)" << endl;
  MIL << "doUpdate: " << ustats.pre_avcand << " packages available for update" << endl;

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
	applyingSplits[cpkg].insert( it->second.begin(), it->second.end() );
	DBG << "  split count for " << cpkg->name() << " now " << applyingSplits[cpkg].size() << endl;
      }
    }
    splitmap.clear();
  }

  ///////////////////////////////////////////////////////////////////
  // Now iterate installed packages, not selected to delete, and
  // figure out what might be an appropriate replacement. Current
  // packages state is changed immediately. Additional packages are
  // reported but set to install later.
  ///////////////////////////////////////////////////////////////////
  MIL << "doUpdate pass 1..." << endl;

  for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {

    if ( ! (*it)->has_installed() ) {
      continue;
    }
    ++ustats.chk_installed_total;

    if ( (*it)->to_delete() ) {
      DBG << "SKIP to delete: " << (*it)->installedObj() << endl;
      ++ustats.chk_already_todel;
      continue;
    }

    PMSelectablePtr state( *it );
    PMPackagePtr    installed( (*it)->installedObj() );
    PMPackagePtr    candidate( (*it)->candidateObj() );

    DBG << "REPLACEMENT FOR " << installed << endl;

    // if installed not SuSE -> no action ???

    // Taboo - currently an installed package can't be taboo,
    // but if -> no action ???

    ///////////////////////////////////////////////////////////////////
    // figure out replacement
    ///////////////////////////////////////////////////////////////////
    if ( candidate ) {

      if ( ! state->to_install() ) {
	if ( installed->edition() < candidate->edition() ) {
	  // new version
	  state->appl_set_install();
	  DBG << " ==> INSTALL (new version): " << candidate << endl;
	  ++ustats.chk_to_update;
	} else {
	  // check whether to downgrade:
	  // both must have vendor 'SuSE' and candidates buildtime must be
	  // newer.
	  if (    installed->vendor().isSuSE()
	       && candidate->vendor().isSuSE()
	       && installed->buildtime() < candidate->buildtime() ) {
	    state->appl_set_install();
	    DBG << " ==> INSTALL (SuSE version downgrade): " << candidate << endl;
	    ++ustats.chk_to_downgrade;
	  } else {
	    DBG << " ==> (candidate older)" << << candidate endl;
	    ++ustats.chk_to_keep_old;
	  }
	}
      } else {
	DBG << " ==> INSTALL (preselected): " << candidate << endl;
	++ustats.chk_already_toins;
      }

    } else {

      if ( ! installed->vendor().isSuSE() ) {
	DBG << " ==> (keep non SuSE package)" << endl;
	++ustats.chk_keep_foreign;
	continue; // no check for splits
      }

      // renamed or droped (ayway there's no candidate for this!)
      // If unique provides exists check if obsoleted (rename).
      // Remember new package for 2nd pass.
      const PkgSet::RevRelList_type & provided = available.provided()[installed->name()];
      PackageSet mpkg;

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

#warning Should autodelete discontinued packages ?
      state->appl_set_delete();

      switch ( mpkg.size() ) {
      case 0:
	DBG << " ==> (dropped)" << endl;
	++ustats.chk_dropped;
	break;
      case 1:
        addProvided[installed] = mpkg;
	// must check obsoletes ?
	DBG << " ==> RENAMED to: " << (*mpkg.begin()) << endl;
	++ustats.chk_renamed;
	break;
      default:
	addMultiProvided[installed] = mpkg;
	DBG << " ==> pass 2 (" << mpkg.size() << " times provided)" << endl;
	// count stats later
	break;
      }
    }

    ///////////////////////////////////////////////////////////////////
    // anyway check for packages split off
    ///////////////////////////////////////////////////////////////////

    TodoMap::iterator sit = applyingSplits.find( installed );
    if ( sit != applyingSplits.end() ) {
      PackageSet & toadd( sit->second );
      if ( !toadd.size() ) {
	INT << "Empty SplitPkgMap entry for " << installed << endl;
      } else {
	for ( PackageSet::iterator ait = toadd.begin(); ait != toadd.end(); ++ait ) {
	  DBG << " ==> ADD (splited): " << (*ait) << endl;
	}
	addSplitted[installed] = toadd;
      }
      // count stats later
    }
  }

  ///////////////////////////////////////////////////////////////////
  // Now check the remembered packages and check non unique provided.
  // Maybe one of themwas somehow selected. Otherwise we have to guess
  // one.
  ///////////////////////////////////////////////////////////////////
  MIL << "doUpdate pass 2..." << endl;

  for ( TodoMap::iterator it = addProvided.begin(); it != addProvided.end(); ++it ) {
    PackageSet & tset( it->second );
    for ( PackageSet::iterator sit = tset.begin(); sit != tset.end(); ++sit ) {
      (*sit)->getSelectable()->appl_set_install();
    }
  }

  for ( TodoMap::iterator it = addSplitted.begin(); it != addSplitted.end(); ++it ) {
    PackageSet & tset( it->second );
    for ( PackageSet::iterator sit = tset.begin(); sit != tset.end(); ++sit ) {
      if ( ! (*sit)->getSelectable()->to_install() ) {
	(*sit)->getSelectable()->appl_set_install();
	++ustats.chk_add_split;
      }
    }
  }

  for ( TodoMap::iterator it = addMultiProvided.begin(); it != addMultiProvided.end(); ++it ) {
    DBG << "GET ONE OUT OF " << it->second.size() << " for " << it->first << endl;

    PMPackagePtr guess;
    PackageSet & gset( it->second );
    for ( PackageSet::iterator git = gset.begin(); git != gset.end(); ++git ) {
      if ( (*git)->getSelectable()->to_install() ) {
	DBG << " ==> (pass 2: meanwhile set to install): " << (*git) << endl;
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
      DBG << " ==> RENAMED to: (pass 2: guessed): " << guess << endl;
      ++ustats.chk_renamed_guessed;
    }
  }

  ///////////////////////////////////////////////////////////////////
  // done
  ///////////////////////////////////////////////////////////////////
  MIL << ustats << endl;
  return 0;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : std::ostream &
**
**	DESCRIPTION :
*/
std::ostream & operator<<( std::ostream & str, const PMUpdateStats & obj )
{
  str << "====================================================" << endl;
  str << "pre_todel           " << obj.pre_todel << endl;
  str << "pre_nocand          " << obj.pre_nocand << endl;
  str << "pre_avcand          " << obj.pre_avcand << endl;
  str << endl;
  str << "chk_installed_total " << obj.chk_installed_total << endl;
  str << "chk_already_todel   " << obj.chk_already_todel << endl;
  str << "chk_already_toins   " << obj.chk_already_toins << endl;
  str << endl;
  str << "chk_to_update       " << obj.chk_to_update << endl;
  str << "chk_to_downgrade    " << obj.chk_to_downgrade << endl;
  str << "chk_to_keep_old     " << obj.chk_to_keep_old << endl;
  str << endl;
  str << "chk_keep_foreign    " << obj.chk_keep_foreign << endl;
  str << "chk_dropped         " << obj.chk_dropped << endl;
  str << "chk_renamed         " << obj.chk_renamed << endl;
  str << "chk_renamed_guessed " << obj.chk_renamed_guessed << endl;
  str << "chk_add_split       " << obj.chk_add_split << endl;
  str << "====================================================" << endl;

  return str;
}


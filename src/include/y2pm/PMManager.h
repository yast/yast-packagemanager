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

   File:       PMManager.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMManager_h
#define PMManager_h

#include <iosfwd>
#include <vector>
#include <map>

#include <y2util/BitField.h>

#include <y2pm/PkgName.h>
#include <y2pm/PMSelectablePtr.h>
#include <y2pm/PMObjectContainerIter.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMManager
/**
 *
 **/
class PMManager {

  friend std::ostream & operator<<( std::ostream & str, const PMManager & obj );

  PMManager & operator=( const PMManager & );
  PMManager            ( const PMManager & );

  public:

    typedef std::vector<PMSelectablePtr>  PMSelectableVec;

  private:

    typedef std::map<PkgName,PMSelectablePtr> PMSelectablePool;

    PMSelectablePool _itemPool;
    PMSelectableVec _items;

    BitField _installed;

  private:

    virtual PMSelectablePtr newSelectable( const PkgName & name_r ) const;

  private:

    PMSelectablePtr poolLookup( const PkgName & name_r ) const;

  public:

    PMManager();
    virtual ~PMManager();

  public:

    void poolSetInstalled( PMObjectContainerIter iter_r );
    void poolAddCandidates( PMObjectContainerIter iter_r );
    void poolRemoveCandidates( PMObjectContainerIter iter_r );
};

///////////////////////////////////////////////////////////////////

#endif // PMManager_h

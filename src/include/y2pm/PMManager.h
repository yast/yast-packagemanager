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

#include <y2pm/PMObject.h>
#include <y2pm/PMSelectable.h>
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

    typedef std::map<std::string,PMSelectablePtr> PMSelectablePool;

    PMSelectablePool _itemPool;
    PMSelectableVec _items;

    BitField _bf_installed;

  private:

    virtual PMSelectablePtr newSelectable( const PkgName & name_r ) const;

    PMSelectablePtr poolLookup( unsigned idx_r ) const;
    PMSelectablePtr poolLookup( const std::string & name_r ) const;
    PMSelectablePtr poolProvide( const std::string & name_r );

    void clearAll();

    void checkPool() const;

  public:

    PMManager();
    virtual ~PMManager();

    void REINIT(); // dont use it

  public:

    /**
     * Called from TargetSystem providing all(!) installed objects
     **/
    void poolSetInstalled( PMObjectContainerIter iter_r );

    /**
     * Called from InstSrc to add provided Objects
     **/
    void poolAddCandidates( PMObjectContainerIter iter_r );

    /**
     * Called from InstSrc to remove the formerly added objects.
     **/
    void poolRemoveCandidates( PMObjectContainerIter iter_r );

  public:

    /**
     * The number of Selectables within this Manager.
     **/
    unsigned size() const { return _items.size(); }

    /**
     * True if Manager does not contain any Selectable
     **/
    bool empty() const { return _items.empty(); }

    /**
     * Iterator for Selectables within this Manager.
     **/
    PMSelectableVec::const_iterator begin() const { return _items.begin(); }

    /**
     * Iterator for Selectables within this Manager.
     **/
    PMSelectableVec::const_iterator end() const { return _items.end(); }

    /**
     * Iterator for Selectables within this Manager.
     **/
    PMSelectableVec::const_reverse_iterator rbegin() const { return _items.rbegin(); }

    /**
     * Iterator for Selectables within this Manager.
     **/
    PMSelectableVec::const_reverse_iterator rend() const { return _items.rend(); }

    /**
     * PMSelectablePtr to the Selectable with the given index, or NULL if index is invalid.
     **/
    PMSelectablePtr getItem( const unsigned idx_r ) const { return poolLookup( idx_r ); }

    /**
     * PMSelectablePtr to the Selectable with the given name, or NULL if there is none.
     **/
    PMSelectablePtr getItem( const std::string & name_t ) const { return poolLookup( name_t ); }

    /**
     * PMSelectablePtr to the Selectable with the given index, or NULL if index is invalid.
     **/
    PMSelectablePtr operator [] ( const unsigned idx_r ) const { return getItem( idx_r ); }

    /**
     * PMSelectablePtr to the Selectable with the given name, or NULL if there is none.
     **/
    PMSelectablePtr operator [] ( const std::string & name_r ) const { return getItem( name_r ); }
};

///////////////////////////////////////////////////////////////////

#endif // PMManager_h

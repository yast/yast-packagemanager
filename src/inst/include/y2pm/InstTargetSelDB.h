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

  File:       InstTargetSelDB.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef InstTargetSelDB_h
#define InstTargetSelDB_h

#include <iosfwd>

#include <y2util/Pathname.h>

#include <y2pm/InstTargetSelDBPtr.h>
#include <y2pm/InstTargetError.h>

#include <y2pm/PMSelectionPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstTargetSelDB
/**
 *
 **/
class InstTargetSelDB : virtual public Rep {
  REP_BODY(InstTargetSelDB);

  public:

    /**
     * default error class
     **/
    typedef InstTargetError Error;

  private:

    static const Pathname _db_path;

  private:

    const Pathname _db;

    std::list<PMSelectionPtr> _sellist;

  private:

    PMError assert_open() const;

    PMError check_file( const Pathname & selfile_r ) const;

    Pathname db_file( const Pathname & selfile_r ) const;

    PMError rescan();

  public:

    InstTargetSelDB();

    ~InstTargetSelDB();

  public:

    PMError open( const Pathname & system_root_r = "/", const bool create_r = false );

    bool isOpen() const { return !_db.empty(); }

    const Pathname & dbPath() const { return _db; }

    const std::list<PMSelectionPtr> & getSelections() const { return _sellist; }

  public:

    bool isInstalled( const Pathname & selfile_r ) const;

    PMError install( const Pathname & selfile_r );

    PMError remove( const Pathname & selfile_r );

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // InstTargetSelDB_h

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

  File:       InstTargetProdDB.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef InstTargetProdDB_h
#define InstTargetProdDB_h

#include <iosfwd>
#include <list>
#include <map>

#include <y2util/Pathname.h>

#include <y2pm/InstTargetProdDBPtr.h>
#include <y2pm/InstTargetError.h>

#include <y2pm/InstSrcDescrPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstTargetProdDB
/**
 *
 **/
class InstTargetProdDB : public CountedRep {
  REP_BODY(InstTargetProdDB);

  public:

    /**
     * default error class
     **/
    typedef InstTargetError Error;

  private:

    static const Pathname    _db_path;
    static const std::string _db_stem;
    static const unsigned    _db_nwidth;

  private:

    const Pathname _db;
    unsigned       _nextIdx;

    std::map<unsigned,constInstSrcDescrPtr> _prodmap;
    std::list<constInstSrcDescrPtr>         _prodlist;

  private:

    PMError assert_open() const;

    std::string db_file_name( unsigned num_r ) const;
    bool        is_db_file_name( const std::string & fname_r ) const;

    Pathname    db_file( const std::string & fname_r ) const;

    PMError     read_db_file( const std::string & fname_r,
			      unsigned & idx_r, InstSrcDescrPtr & ndescr_r ) const;

  public:

    InstTargetProdDB();

    ~InstTargetProdDB();

  public:

    PMError open( const Pathname & system_root_r = "/" );

    bool isOpen() const { return _nextIdx; }

    const Pathname & dbPath() const { return _db; }

    const std::list<constInstSrcDescrPtr> & getProducts() const { return _prodlist; }

  public:

    bool isInstalled( const constInstSrcDescrPtr & isd_r ) const;

    PMError install( const constInstSrcDescrPtr & isd_r );

    PMError remove( const constInstSrcDescrPtr & isd_r );

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // InstTargetProdDB_h


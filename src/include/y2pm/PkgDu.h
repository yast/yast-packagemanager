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

  File:       PkgDu.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef PkgDu_h
#define PkgDu_h

#include <iosfwd>
#include <string>
#include <set>

#include <y2util/FSize.h>

#include <y2pm/PMPackagePtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgDuMaster
/**
 *
 **/
class PkgDuMaster {

  PkgDuMaster & operator=( const PkgDuMaster & ); // no assign
  PkgDuMaster            ( const PkgDuMaster & ); // no copy

  public:

    ///////////////////////////////////////////////////////////////////
    //
    //	CLASS NAME : PkgDuMaster::MountPoint
    //
    class MountPoint {
      public:
	// vital data (must not be modified)
	const std::string _mountpoint;
	const FSize       _blocksize;
      public:
	// additional MountPoint data
	mutable FSize     _total;
	mutable FSize     _used;
      public:
	// statistics counted by add/sub
	mutable FSize     _pkgusage;
      public:
	MountPoint( const std::string & mountpoint_r,
		    const FSize & blocksize_r = 1024,
		    const FSize & total_r = 0,
		    const FSize & used_r  = 0 )
	  : _mountpoint( mountpoint_r )
	  , _blocksize( blocksize_r )
	  , _total( total_r )
	  , _used( used_r )
	  , _pkgusage( 0 )
	{}
	~MountPoint() {}
      public:
	bool assignData( const MountPoint & rhs ) const;
      public:
	friend std::ostream & operator<<( std::ostream & str, const MountPoint & obj );
      public:
	bool operator==( const MountPoint & rhs ) const { return( _mountpoint == rhs._mountpoint ); }
	bool operator<( const MountPoint & rhs )  const { return( _mountpoint < rhs._mountpoint ); }
    };
    ///////////////////////////////////////////////////////////////////

  private:

    static unsigned _counter;

    unsigned _count;

    void newcount() { _count = ++_counter; }

  private:

    std::set<MountPoint> _mountpoints;

  private:

    friend class PkgDu;

    void add( FSize * data_r );
    void sub( FSize * data_r );

  public:

    PkgDuMaster();
    ~PkgDuMaster();

  public:

    unsigned sync_count() const { return _count; }

    unsigned resetStats();

    void setMountPoints( const std::set<MountPoint> & mountpoints_r );

    const std::set<MountPoint> & mountpoints() const { return _mountpoints; }

  public:

    friend std::ostream & operator<<( std::ostream & str, const PkgDuMaster & obj );
    friend std::ostream & operator<<( std::ostream & str, const std::set<MountPoint> & obj );
};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgDu
/**
 *
 **/
class PkgDu {

  PkgDu & operator=( const PkgDu & ); // no assign
  PkgDu            ( const PkgDu & ); // no copy

  private:

    friend class PMPackage;

    PkgDu();
    ~PkgDu();

    typedef PkgDuMaster::MountPoint MountPoint;

  private:

    /**
     * Last PkgDuMaster synccount
     **/
    mutable unsigned _count;

    /**
     * Data computed according to last PkgDuMaster
     **/
    mutable FSize * _data;

    /**
     * Ckeck whether in sync with PkgDuMaster. If not recompute data.
     * Return true if there are data to contribute.
     **/
    bool sync( const PMPackage & pkg_r, PkgDuMaster & master_r ) const;

    /**
     * Sync, and add available data to PkgDuMasters statistics.
     * Return true if there were data to contribute.
     **/
    bool add( const PMPackage & pkg_r, PkgDuMaster & master_r ) const;

    /**
     * Sync, and subtract available data from PkgDuMasters statistics.
     * Return true if there were data to contribute.
     **/
    bool sub( const PMPackage & pkg_r, PkgDuMaster & master_r ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PkgDu_h

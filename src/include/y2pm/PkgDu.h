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

  Purpose: Helper classes to collect package disk usage info.

/-*/
#ifndef PkgDu_h
#define PkgDu_h

#include <iosfwd>
#include <string>
#include <list>
#include <set>

#include <y2util/FSize.h>

#include <y2pm/PMPackagePtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgDu
/**
 * @short Helper classe to collect package diskspace usage info.
 *
 * <code>PkgDu</code> maintains a set of <code>PkgDu::Entry</code> to
 * keep inforamtion about diskspace usage in several directories.
 *
 * A <code>PkgDu::Entry</code> is identified by a directory name (expected to be
 * an absolute path, a trailing '/' is appended if not present). Values for size
 * and number of files within this directory are mutable.
 *
 * As the diskspace usage depends on the filesystems blocksize, you'll probabely
 * get the closest result counting filesizes rounded up to full Kb (@ref FSize::fillBlock)
 * and the number of files. Diskspace usage calculation will then add additional
 * half blocksize for each file.
 **/
class PkgDu {

  public:

    ///////////////////////////////////////////////////////////////////
    //
    //	CLASS NAME : PkgDu::Entry
    /**
     * @short Helper classe to collect package diskspace usage info.
     **/
    struct Entry {

      friend std::ostream & operator<<( std::ostream & str, const Entry & obj );

      /**
       * Directory name (absolute path, trailing '/' is appended if missing)
       **/
      std::string _dirname;
      /**
       * Ammount of byte within this directory
       **/
      mutable FSize    _size;
      /**
       * Nunber of files within this directory
       **/
      mutable unsigned _files;

      /**
       * Constructor
       **/
      Entry( const std::string & dirname_r, const FSize & size_r = 0, const unsigned & files_r = 0 )
	: _dirname( dirname_r ), _size( size_r ), _files( files_r )
      {
	if ( _dirname.size() && _dirname[_dirname.size()-1] != '/' ) {
	  _dirname += '/';
	}
      }

      /**
       * Default constructor
       **/
      Entry() : _files( 0 ) {}
      /**
       * Test for equality based on directory name.
       **/
      bool operator==( const Entry & rhs ) const {
        return 	_dirname == rhs._dirname;
      }
      /**
       * Order based on directory name.
       **/
      bool operator<( const Entry & rhs ) const {
        return 	_dirname < rhs._dirname;
      }

      /**
       * Return true if this entry denotes a directory equal to or below rhs._dirname.
       **/
      bool isBelow( const Entry & rhs ) const {
	// whether _dirname has prefix rhs._dirname
        return(	_dirname.compare( 0, rhs._dirname.size(), rhs._dirname ) == 0 );
      }
      /**
       * Return true if this entry denotes a directory equal to or below dirname_r.
       **/
      bool isBelow( const std::string & dirname_r ) const {
        return 	isBelow( Entry( dirname_r ) );
      }

      /**
       * Numerical operation based on size and files values.
       **/
      const Entry & operator+=( const Entry & rhs ) const {
	_size  += rhs._size;
	_files += rhs._files;
	return *this;
      }
      /**
       * Numerical operation based on size and files values.
       **/
      const Entry & operator-=( const Entry & rhs ) const {
	_size  -= rhs._size;
	_files -= rhs._files;
	return *this;
      }
    };

    ///////////////////////////////////////////////////////////////////

  private:

    typedef std::set<Entry> EntrySet;

    EntrySet _dirs;

  public:

    /**
     * Constructor
     **/
    PkgDu() {}

    /**
     * Add an entry. If already present, sum up the new entries size and files value.
     **/
    void add( const Entry & newent_r ) {
      std::pair<EntrySet::iterator,bool> res = _dirs.insert( newent_r );
      if ( !res.second ) {
	*res.first += newent_r;
      }
    }

    /**
     * Add an entry. If already present, sum up the new entries size and files value.
     **/
    void add( const std::string & dirname_r, const FSize & size_r = 0, const unsigned & files_r = 0 ) {
      add( Entry( dirname_r, size_r, files_r ) );
    }

    /**
     * Number of entries
     **/
    unsigned size() const { return _dirs.size(); }

    /**
     * Clear EntrySet
     **/
    void clear() { _dirs.clear(); }

  public:

    /**
     * Add entries parsed from a packages.DU file
     **/
    void addFrom( const std::list<std::string> & dudata_r );

    /**
     * Set entries parsed from a packages.DU file
     **/
    void setFrom( const std::list<std::string> & dudata_r ) {
      clear();
      addFrom( dudata_r );
    }

  public:

    /**
     * Sum up any entries for dirname_r and its descendants and remove them
     * on the fly. Return the result.
     **/
    Entry extract( const std::string & dirname_r );

  public:

   typedef EntrySet::iterator               iterator;
   typedef EntrySet::reverse_iterator       reverse_iterator;

   /**
    * Forward iterator pointing to the first entry (if any)
    **/
   iterator begin() { return _dirs.begin(); }
   /**
    * Forward iterator pointing behind the last entry.
    **/
   iterator end() { return _dirs.end(); }
   /**
    * Reverse iterator pointing to the last entry (if any)
    **/
   reverse_iterator rbegin() { return _dirs.rbegin(); }
   /**
    * Reverse iterator pointing before the first entry.
    **/
   reverse_iterator rend() { return _dirs.rend(); }

   typedef EntrySet::const_iterator         const_iterator;
   typedef EntrySet::const_reverse_iterator const_reverse_iterator;

   /**
    * Forward const iterator pointing to the first entry (if any)
    **/
   const_iterator begin() const { return _dirs.begin(); }
   /**
    * Forward const iterator pointing behind the last entry.
    **/
   const_iterator end() const { return _dirs.end(); }
   /**
    * Reverse const iterator pointing to the last entry (if any)
    **/
   const_reverse_iterator rbegin() const { return _dirs.rbegin(); }
   /**
    * Reverse const iterator pointing before the first entry.
    **/
   const_reverse_iterator rend()const { return _dirs.rend(); }

  public:

   friend std::ostream & operator<<( std::ostream & str, const PkgDu & obj );
};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgDuMaster
/**
 * @short PMPackageManager helper to collect disk usage info.
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
	// contains the change of diskusage
	mutable FSize     _pkgusage;
      public:
	const std::string & mountpoint()        const { return _mountpoint; }
	FSize               total()             const { return _total; }
	// current usage without packages taken into accout
	FSize               initial_used()      const { return _used; }
	FSize               initial_available() const { return total() - initial_used(); }
	int                 initial_u_percent() const { return( total() ? initial_used() * 100 / total() : 0 ); }
	// current usage with packages taken into accout
	FSize               pkg_diff()          const { return _pkgusage; }
	FSize               pkg_used()          const { return _used + _pkgusage; }
	FSize               pkg_available()     const { return total() - pkg_used(); }
	int                 pkg_u_percent()     const { return( total() ? pkg_used() * 100 / total() : 0 ); }
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

    friend class PkgDuSlave;

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

  private:

    MountPoint _overall;

  public:

    /**
     * Overall stats (not per partition)
     **/
    FSize               total()             const { return _overall.total(); }
    // current usage without packages taken into accout
    FSize               initial_used()      const { return _overall.initial_used(); }
    FSize               initial_available() const { return _overall.initial_available(); }
    int                 initial_u_percent() const { return _overall.initial_u_percent(); }
    // current usage with packages taken into accout
    FSize               pkg_diff()          const { return _overall.pkg_diff(); }
    FSize               pkg_used()          const { return _overall.pkg_used(); }
    FSize               pkg_available()     const { return _overall.pkg_available(); }
    int                 pkg_u_percent()     const { return _overall.pkg_u_percent(); }

  public:

    void add( const FSize & szs_r ) { _overall._pkgusage += szs_r; }
    void sub( const FSize & szs_r ) { _overall._pkgusage -= szs_r; }

  public:

    friend std::ostream & operator<<( std::ostream & str, const PkgDuMaster & obj );
    friend std::ostream & operator<<( std::ostream & str, const std::set<MountPoint> & obj );
};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgDuSlave
/**
 * @short PMPackage helper to calculate and provide du information.
 **/
class PkgDuSlave {

  PkgDuSlave & operator=( const PkgDuSlave & ); // no assign
  PkgDuSlave            ( const PkgDuSlave & ); // no copy

  private:

    friend class PMPackage;

    PkgDuSlave();
    ~PkgDuSlave();

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

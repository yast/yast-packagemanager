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

  File:       binHeaderCache.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef binHeaderCache_h
#define binHeaderCache_h

#include <iosfwd>

#include <y2util/Pathname.h>

#include <y2pm/binHeaderPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : binHeaderCache
/**
 *
 **/
class binHeaderCache {

  friend std::ostream & operator<<( std::ostream & str, const binHeaderCache & obj );

  binHeaderCache & operator=( const binHeaderCache & );
  binHeaderCache            ( const binHeaderCache & );

  public:

    typedef unsigned pos;

    static const pos npos;

  private:

    static const unsigned BHC_MAGIC_SZE;

    class Cache;

    Cache & _c;

  private:

    int _cReadMagic();

  protected:

    Pathname _cpath;

  protected:

    std::string _cmagic;

    time_t _cdate;

    pos _cheaderStart;


  protected:

    virtual bool magicOk() { return _cmagic.empty(); }

  public:

    binHeaderCache( const Pathname & cache_r );

    virtual ~binHeaderCache();

  public:

    bool open();

    void close();

    bool isOpen() const;

    const Pathname & cpath() const { return  _cpath; }

    const std::string & cmagic() const { return _cmagic; }

    time_t cdate() const { return _cdate; }

    pos tell() const;

    pos seek( const pos pos_r );

    unsigned readData( void * buf_r, unsigned count_r );

    binHeaderPtr readHeader( bool magicp = true );
};

///////////////////////////////////////////////////////////////////

#endif // binHeaderCache_h

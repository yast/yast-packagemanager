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

   File:       PMError.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMError_h
#define PMError_h

#include <iosfwd>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMError
/**
 *
 **/
class PMError {

  static const int _range = 1000;

  public:

    enum Reporter {
      E_OK           = 0,
      E_INST_SRC_MGR = 10*_range,
      E_INST_SRC     = 11*_range
    };

  protected:

    int _errval;

  public:

    PMError( const int e = E_OK ) { _errval = e; }

    operator int() const { return _errval; }

    std::ostream & dumpOn( std::ostream & str ) const;

    friend std::ostream & operator<<( std::ostream & str, const PMError & obj ) {
      return obj.dumpOn( str );
    }
};

///////////////////////////////////////////////////////////////////

#endif // PMError_h

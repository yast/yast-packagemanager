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

  File:       Timecount.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef Timecount_h
#define Timecount_h

#include <iosfwd>
#include <string>

#include <y2util/Date.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : Timecount
/**
 *
 **/
class Timecount {

  Timecount & operator=( const Timecount & );
  Timecount            ( const Timecount & );

  private:

    time_t      _start;
    std::string _msg;

  public:

    Timecount( const char * msg_r = 0, const bool autostart_r = true );
    ~Timecount();

    void start( const char * msg_r = 0 );
    void stop();
};

///////////////////////////////////////////////////////////////////

#endif // Timecount_h

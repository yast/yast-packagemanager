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

  Purpose: Count and log elapsed time for debug purpose

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
 * @short Count and log elapsed time for debug purpose
 **/
class Timecount {

  Timecount & operator=( const Timecount & );
  Timecount            ( const Timecount & );

  private:

    time_t      _start;
    std::string _msg;

  public:

    /**
     * Optional set message to use in loglines Call start() per default
     **/
    Timecount( const char * msg_r = 0, const bool autostart_r = true );

    /**
     * Calls stop()
     **/
    ~Timecount();

    /**
     * Start timer and optional reset message
     **/
    void start( const char * msg_r = 0 );

    /**
     * Stop timer and write logline showing message and elapsed time
     **/
    void stop();
};

///////////////////////////////////////////////////////////////////

#endif // Timecount_h

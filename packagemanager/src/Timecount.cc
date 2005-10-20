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

  File:       Timecount.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Count and log elapsed time for debug purpose

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/Timecount.h>

using namespace std;

#undef Y2LOG
#define Y2LOG "Timecount"

#define TC_DOLOG DBG

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Timecount::Timecount
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
Timecount::Timecount( const char * msg_r, const bool autostart_r )
    : _start  ( 0 )
    , _msg    ( msg_r ? msg_r : "" )
{
  if ( autostart_r )
    start();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Timecount::~Timecount
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
Timecount::~Timecount()
{
  stop();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Timecount::start
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void Timecount::start( const char * msg_r )
{
  stop();
  if ( msg_r )
    _msg = msg_r;
  _start = Date::now();
  TC_DOLOG << "START Timecount(" << _msg << ")" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Timecount::stop
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void Timecount::stop()
{
  if ( _start ) {
    TC_DOLOG << "STOP  Timecount(" << _msg << ") after " << (Date::now() - _start) << " sec." << endl;
    _start = 0;
  }
}


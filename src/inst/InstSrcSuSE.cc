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

   File:       InstSrcSuSE.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcSuSE.h>
#include <y2pm/InstSrcDataCommonPkd.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcSuSE
//
///////////////////////////////////////////////////////////////////

IMPL_HANDLES(InstSrcSuSE);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcSuSE::InstSrcSuSE
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
InstSrcSuSE::InstSrcSuSE( MediaInfoPtr media_r )
  : InstSrc( media_r )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcSuSE::~InstSrcSuSE
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstSrcSuSE::~InstSrcSuSE()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcSuSE::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & InstSrcSuSE::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}


int InstSrcSuSE::numItems()
{
  if(!_activated) return 0;
  return _pkglist.size();
}

bool InstSrcSuSE::Activate()
{
  if(_activated) return true;
  _activated=true;
  _data = new InstSrcDataCommonPkd(_media);
  return true;
}

bool InstSrcSuSE::Deactivate()
{
  if(!_activated) return true;
  _data = NULL;
  _activated=false;
  return true;
}

bool InstSrcSuSE::Erase()
{
  D__ << "not implemented" << std::endl;
  return true;
}

std::list<PMPackagePtr> InstSrcSuSE::getPackages()
{
  D__ << endl;
  if(!_activated || _data == NULL)
  {
    D__ << "not activated or no data" << endl;
    std::list<PMPackagePtr> empty;
    return empty;
  }
  _pkglist = _data->getPackages();
  return _pkglist;
}

std::list<PMPackagePtr> InstSrcSuSE::Immediate(const std::list<PMPackagePtr>& l)
{
  if(l.empty())
  {
    return _pkglist;
  }
  else
  {
    return l;
  }
}

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

  File:       InstSrcData.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/InstSrcData.h>

#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/MediaAccess.h>

#include <Y2PM.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstSrcDataPtr
//	CLASS NAME : constInstSrcDataPtr
///////////////////////////////////////////////////////////////////
IMPL_BASE_POINTER(InstSrcData);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData::InstSrcData
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
InstSrcData::InstSrcData()
    : _propagating( false )
{
// -> *this calls dumpOn which is *NOT* ready yet  MIL << "New InstSrcData " << *this << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData::~InstSrcData
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstSrcData::~InstSrcData()
{
  MIL << "Delete InstSrcData " << *this << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData::_instSrc_atach
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void InstSrcData::_instSrc_attach( const InstSrcPtr & instSrc_r )
{
  if ( _instSrc || _propagating ) {
    INT << "SUSPICIOUS: instSrc " << _instSrc << " _propagating " << _propagating << endl;
  }

  _instSrc = instSrc_r;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData::_instSrc_detach
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void InstSrcData::_instSrc_detach()
{
  if ( !_instSrc || _propagating ) {
    INT << "SUSPICIOUS: instSrc " << _instSrc << " _propagating " << _propagating << endl;
    if ( _propagating )
      _instSrc_withdraw();
  }

  _instSrc = 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData::_instSrc_propagate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void InstSrcData::_instSrc_propagate()
{
  if ( !_instSrc || _propagating ) {
    INT << "SUSPICIOUS: instSrc " << _instSrc << " _propagating " << _propagating << endl;
  }

  propagateObjects();
  _propagating = true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData::_instSrc_withdraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void InstSrcData::_instSrc_withdraw()
{
  if ( !_instSrc || !_propagating ) {
    INT << "SUSPICIOUS: instSrc " << _instSrc << " _propagating " << _propagating << endl;
  }

  withdrawObjects();
  _propagating = false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData::propagateObjects
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void InstSrcData::propagateObjects()
{
  if ( getPackages().size() ) {
    Y2PM::packageManager().poolAddCandidates( getPackages() );
  }
  if ( getSelections().size() ) {
    Y2PM::selectionManager().poolAddCandidates( getSelections() );
  }
  if ( getPatches().size() ) {
    Y2PM::youPatchManager().poolAddCandidates( getPatches() );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData::withdrawObjects
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void InstSrcData::withdrawObjects()
{
  if ( getPackages().size() ) {
    Y2PM::packageManager().poolRemoveCandidates( getPackages() );
  }
  if ( getSelections().size() ) {
    Y2PM::selectionManager().poolRemoveCandidates( getSelections() );
  }
  if ( getPatches().size() ) {
    Y2PM::youPatchManager().poolRemoveCandidates( getPatches() );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData::writeCache
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcData::writeCache( const Pathname & cache_dir_r ) const
{
  MIL << *this << " does not support data cache." << endl;
  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData::dumpOn
//	METHOD TYPE : std::ostream &
//
//	DESCRIPTION :
//
std::ostream & InstSrcData::dumpOn( std::ostream & str ) const
{
  Rep::dumpOn( str ) << "(";
  str << "(" << ( _propagating ? "propagating" : "withdrawn" );
  str << " sel:" << getSelections().size();
  str << " pkg:" << getPackages().size();
  str << " you:" << getPatches().size();
  return str << ")";
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData::tryGetDescr
//	METHOD TYPE : PMError
//
//	DESCRIPTION : JUST A TEMPLATE
//
PMError InstSrcData::tryGetDescr( InstSrcDescrPtr & ndescr_r,
				  MediaAccessPtr media_r, const Pathname & produduct_dir_r )
{
  ndescr_r = 0;
  PMError err;

  InstSrcDescrPtr ndescr( new InstSrcDescr );

  ///////////////////////////////////////////////////////////////////
  // parse InstSrcDescr from media_r and fill ndescr
  ///////////////////////////////////////////////////////////////////

  // TBD
  err = Error::E_error;

  ///////////////////////////////////////////////////////////////////
  // done
  ///////////////////////////////////////////////////////////////////
  if ( !err ) {
    ndescr_r = ndescr;
  }
  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData::tryGetData
//	METHOD TYPE : PMError
//
//	DESCRIPTION : JUST A TEMPLATE
//
PMError InstSrcData::tryGetData( InstSrcDataPtr & ndata_r,
				 MediaAccessPtr media_r, const Pathname & descr_dir_r )
{
  ndata_r = 0;
  PMError err;

  // InstSrcDataPtr ndata( new InstSrcData );

  ///////////////////////////////////////////////////////////////////
  // parse InstSrcData from media_r and fill ndescr
  ///////////////////////////////////////////////////////////////////

  // TBD
  err = Error::E_error;

  ///////////////////////////////////////////////////////////////////
  // done
  ///////////////////////////////////////////////////////////////////
  //  if ( !err ) {
  //  ndata_r = ndata;
  // }
  return err;
}

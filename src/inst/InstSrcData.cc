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
  MIL << "New InstSrcData " << *this << endl;
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
void InstSrcData::_instSrc_atach( const InstSrcPtr & instSrc_r )
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
    const std::list<PMPackagePtr>& packages = getPackages();
    Y2PM::packageManager().poolAddCandidates( packages );
    const std::list<PMSelectionPtr>& selections = getSelections();
    Y2PM::selectionManager().poolAddCandidates( selections );
    const std::list<PMYouPatchPtr>& patches = getPatches();
    Y2PM::youPatchManager().poolAddCandidates( patches );
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

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & InstSrcData::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
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

const Pathname
InstSrcData::writeCache (const Pathname &descrpathname)
{
    return InstData::writeCache (descrpathname);
}

#if 0
/**
 * generate PMSelection objects for each Item on the source
 * @return list of PMSelectionPtr on this source
 * */
const std::list<PMSelectionPtr>&
InstSrcData::getSelections() const
{
    return std::list<PMSelectionPtr>();
}

/**
 * generate PMPackage objects for each Item on the source
 * @return list of PMPackagePtr on this source
 * */
const std::list<PMPackagePtr>&
InstSrcData::getPackages() const
{
    MIL << "InstSrcData::getPackages(" << _packages.size() << ")" << endl;
    return _packages;
}

/**
 * generate PMSolvable objects for each patch on the target
 * @return list of PMSolvablePtr on this target
 */
const std::list<PMYouPatchPtr>&
InstSrcData::getPatches (void) const
{
    MIL << "InstSrcData::getPatches()" << endl;
    return _patches;
}

#endif

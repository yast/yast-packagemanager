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
//	DESCRIPTION : initialization with new media
//
InstSrcData::InstSrcData(MediaAccessPtr media_r)
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData::InstSrcData
//	METHOD TYPE : Constructor
//
//	DESCRIPTION : initialization with known media
//
InstSrcData::InstSrcData(const Pathname & contentcachefile)
{
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
//	DESCRIPTION :
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
  //err = Error::E_error;

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
//	DESCRIPTION :
//
PMError InstSrcData::tryGetData( InstSrcDataPtr & ndata_r,
				 MediaAccessPtr media_r, const Pathname & descr_dir_r )
{
  ndata_r = 0;
  PMError err;

  InstSrcDataPtr ndata( new InstSrcData );

  ///////////////////////////////////////////////////////////////////
  // parse InstSrcData from media_r and fill ndescr
  ///////////////////////////////////////////////////////////////////

  // TBD
  //err = Error::E_error;

  ///////////////////////////////////////////////////////////////////
  // done
  ///////////////////////////////////////////////////////////////////
  if ( !err ) {
    ndata_r = ndata;
  }
  return err;
}

/**
 * generate PMSelection objects for each Item on the source
 * @return list of PMSelectionPtr on this source
 * */
const std::list<PMSelectionPtr> *
InstSrcData::getSelections() const
{
    D__ << __FUNCTION__ << std::endl;
    if (!_data)
    {
	cerr << "InstSrcData::getSelections() no _data" << endl;
	return 0;
    }
    return _data->getSelections();
}

/**
 * generate PMPackage objects for each Item on the source
 * @return list of PMPackagePtr on this source
 * */
const std::list<PMPackagePtr> *
InstSrcData::getPackages() const
{
    D__ << __FUNCTION__ << std::endl;
    if (!_data)
    {
	cerr << "InstSrcData::getPackages() no _data" << endl;
	return 0;
    }
    return _data->getPackages();
}

/**
 * find list of packages
 * @return list of PMPackagePtr matching name ,[version] ,[release] ,[architecture]
 */
const std::list<PMPackagePtr> *
InstSrcData::findPackages (const string& name, const string& version, const string& release, const string& arch) const
{
    D__ << __FUNCTION__ << std::endl;
    if (!_data)
    {
	cerr << "InstSrcData::findPackages() no _data" << endl;
	return 0;
    }
    return InstData::findPackages (getPackages(), name, version, release, arch);
}

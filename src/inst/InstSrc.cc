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

   File:       InstSrc.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

    Purpose:	Class for installation sources
		Defines (provides access to)
		- media access (a pointer to MediaAccess)
		- source description (product, version, vendor, ...)
		- contents (list of package, list of selections, ...)

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/InstSrc.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrc
//
///////////////////////////////////////////////////////////////////

//IMPL_HANDLES(InstSrc);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::InstSrc
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
    /**
     * constructor
     *   used for access to new/unknown media
     * @param: URL specification for media access
     *
     * keeps the media open 
     */
InstSrc::InstSrc ( const std::string & mediaurl )
{
    _media = new MediaAccess ();

    if (_media == 0)
	abort();

    // open media by url
    if (_media->open (mediaurl) != 0)
    {
	D__ << "_media.open (" << mediaurl << ") failed." << std::endl;
	return;
    }

    // retrieve description data from media
    _descr = new InstSrcDescr (media);
    if (_descr == 0)
	abort();

    return;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::InstSrc
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
    /**
     * constructor
     *   used for access to known media
     * @param: Pathname specification of content file
     *
     * does not open the media
     */
InstSrc::InstSrc ( const Pathname & contentfile )
{
    _descr = new InstSrcDescr (contentfile);
    if (_descr == 0)
	abort();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::~InstSrc
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstSrc::~InstSrc()
{
    if (_media)
	_media->close ();
}

//-----------------------------
// general functions

/**
 * clean up, e.g. remove all caches
 */
bool
InstSrc::Erase()
{
    D__ << __FUNCTION__ << std::endl;
    return false;
}

/**
 * @return description of Installation source
 * This is needed by the InstSrcMgr
 */
const InstSrcDescr *
InstSrc::getDescription() const
{
    D__ << __FUNCTION__ << std::endl;
    return _descr;
}

/**
 * register this source (store cache files etc)
 * return pathname of saved content file
 */
const Pathname
InstSrc::registerSource (void) const
{
    D__ << __FUNCTION__ << std::endl;
    return _descr->writeCache ();
}

//-----------------------------
// activation status

/**
 * return activation status
 */
bool
InstSrc::getActivation() const
{
    D__ << __FUNCTION__ << std::endl;
    return _descr->getActivation();
}


/**
 * temporary (de)activate source
 */
void
InstSrc::setActivation (bool yesno)
{
    D__ << __FUNCTION__ << std::endl;
    return _descr->setActivation (yesno);
}

//-----------------------------
// source content access

/**
 * return the number of selections on this source
 */
int
InstSrc::numSelections() const
{
    D__ << __FUNCTION__ << std::endl;
    return _data->numSelections();
}


/**
 * return the number of packages on this source
 */
int
InstSrc::numPackages() const
{
    D__ << __FUNCTION__ << std::endl;
    return _data->numPackages();
}


/**
 * return the number of patches on this source
 */
int
InstSrc::numPatches() const
{
    D__ << __FUNCTION__ << std::endl;
    return _data->numPatches();
}


/**
 * generate PMSolvable objects for each selection on the source
 * @return list of PMSolvablePtr on this source
 */
std::list<PMSolvablePtr>
InstSrc::getSelections()
{
    D__ << __FUNCTION__ << std::endl;
    return _data->getSelections();
}

/**
 * generate PMPackage objects for each Item on the source
 * @return list of PMPackagePtr on this source
 * */
std::list<PMPackagePtr>
InstSrc::getPackages()
{
    D__ << __FUNCTION__ << std::endl;
    return _data->getPackages();
}

/**
 * generate PMSolvable objects for each patch on the source
 * @return list of PMSolvablePtr on this source
 */
std::list<PMSolvablePtr>
InstSrc::getPatches()
{
    D__ << __FUNCTION__ << std::endl;
    return _data->getPatches();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream &
InstSrc::dumpOn( ostream & str ) const
{
    Rep::dumpOn( str );
    return str;
}


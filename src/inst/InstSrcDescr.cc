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

   File:       InstSrcDescr.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

   Purpose:	Installation source description
		Keeps all information to identify and describe an installation
		source, but does not have knowledge about its contents (i.e the
		packages, selectins, etc ). See InstSrcData for the contents.

/-*/

#include <iostream>

#include <y2pm/InstSrcDescr.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcDescr
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(InstSrcDescr);

///////////////////////////////////////////////////////////////////

const std::string InstSrcDescr::_cache_file( "description" );

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::InstSrcDescr
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
InstSrcDescr::InstSrcDescr()
    : _type               ( InstSrc::T_UNKNOWN )
    , _default_activate   ( false )
    , _content_product    ( PkgName(), PkgEdition() )
    , _content_baseproduct( PkgName(), PkgEdition() )
    , _content_requires   ( PkgName(), NONE, PkgEdition() )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::~InstSrcDescr
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstSrcDescr::~InstSrcDescr()
{
}

#if 0
void
InstSrcDescr::parseSuSEFile (const Pathname & mountpoint, const Pathname & susefile, bool new_media)
{
    char buf[101];
    const char *sptr = susefile.asString().c_str();
    char *dot;

    sptr += 14;		// skip ".S.u.S.E-disk-"

    // find CD number "NNN."

    _number = 0;
    while (isdigit (*sptr))
    {
	_number *= 10;
	_number += (*sptr - '0');
	sptr++;
    }

    if (*sptr != '.')		// '.' is separator between number and ID
	return;
    sptr++;
    _id = string (sptr);

    if (!new_media)
	return;

    // if not CD1, dont expect further info
    if (_number != 1)
	return;

    // CD1, look for suse/setup/descr/info

    FILE *info = fopen ((mountpoint + "suse/setup/descr/info").asString().c_str(), "r");

fprintf (stderr, "parseSuSEFile(%s) = %p\n", filename.asString().c_str(), info);
    if (info == 0)
	return;

    while (!feof (info))
    {
	char *ptr;
	char *value;

	if (fgets (buf, 100, info) != buf)
	{
	    break;
	}
	ptr = buf;
	while (*ptr)			// skip keywork
	{
	    if ((*ptr == ' ')
		|| (*ptr == '\t'))
	    {
		*ptr++ = 0;
		break;
	    }
	    ptr++;
	}
	while ((*ptr == ' ')		// skip whitespace
		|| (*ptr == '\t'))
	{
	    ptr++;
	}
	value = ptr;

	while (*ptr)
	{
	    if (*ptr == '\n')		// delete trailing \n
	    {
		*ptr = 0;
		break;
	    }
	    ptr++;
	}

	// check keys, copy value

	if (strcmp (key, "PRODUKT_NAME") == 0)
	{
	    _product = value;
	}
	else if (strcmp (key, "PRODUKT_VERSION") == 0)
	{
	    _version = value;
	}
	else if (strcmp (key, "DISTRIBUTION_RELEASE") == 0)
	{
	    _release = atoi (value);
	}
	else if (strcmp (key, "DIST_STRING") == 0)
	{
	    _vendor = value;
	}
	else if (strcmp (key, "DISTRIBUTION_NAME") == 0)
	{
	    _label = value;
	}
    } // while

    fclose (info);

    return;
}


/**
 * write media description to cache file
 * @return pathname of written cache
 * writes private data to an ascii file
 */
const Pathname
InstSrcDescr::writeCache (void)
{
    return Pathname ("");	// empty == error
}

#endif


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & InstSrcDescr::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str ) << "(";
  str << " type:" << _type;
  str << " url:" << _url;
  str << " product:" << _content_product;
  str << " vendor:" << _content_vendor;
  return str << ")";
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::vconstruct
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcDescr::readCache( InstSrcDescrPtr & ndescr_r, const Pathname & cache_dir_r )
{
  ndescr_r = 0;
  PMError err;

  InstSrcDescrPtr ndescr( new InstSrcDescr );

  ///////////////////////////////////////////////////////////////////
  // parse _cache_file and fill into ndescr
  ///////////////////////////////////////////////////////////////////

#warning TBD InstSrcDescr cache read

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
//	METHOD NAME : InstSrcDescr::writeCache
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcDescr::writeCache( const Pathname & cache_dir_r ) const
{
#warning TBD InstSrcDescr cache write

  return Error::E_error;
}


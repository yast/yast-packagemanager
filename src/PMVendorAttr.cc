/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                    (C) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:       PMVendorAttr.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Manage vendor attributes

/-*/

#include <iostream>

#include "PMRcValues.h"
#include <y2pm/PMVendorAttr.h>

using namespace std;

///////////////////////////////////////////////////////////////////
namespace PMVendorAttr {
;//////////////////////////////////////////////////////////////////

typedef map<Vendor,bool> TrustMap;

static TrustMap trustMap;

static bool trusted( const Vendor & vendor_r ) {
  TrustMap::value_type val( vendor_r, false );
  pair<TrustMap::iterator, bool> res = trustMap.insert( val );

  if ( res.second ) {
    // check the new vendor in map
    for ( set<string>::const_iterator it = PM::rcValues().trustedVendors.begin();
	  it != PM::rcValues().trustedVendors.end(); ++it ) {
	if ( stringutil::toLower( res.first->first->substr( 0, it->size() ) )
	     == stringutil::toLower( *it ) ) {
	  // match
	  res.first->second = true;
	  break;
	}
    }
  }

  return res.first->second;
}

bool isKnown( const Vendor & vendor_r )
{
  return trusted( vendor_r );
}

bool autoProtect( const Vendor & vendor_r )
{
  return ! trusted( vendor_r );
}

///////////////////////////////////////////////////////////////////
} // namespace PMVendorAttr
///////////////////////////////////////////////////////////////////

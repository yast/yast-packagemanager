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

  File:       PMVendorAttr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Manage vendor attributes

/-*/
#ifndef PMVendorAttr_h
#define PMVendorAttr_h

#include <iosfwd>

#include <y2util/Vendor.h>

///////////////////////////////////////////////////////////////////
namespace PMVendorAttr {
;//////////////////////////////////////////////////////////////////

/**
 * Return whether it's a known vendor
 **/
extern bool isKnown( const Vendor & vendor_r );

/**
 * Return whether this vendors packages should be protected by
 * default.
 **/
extern bool autoProtect( const Vendor & vendor_r );

///////////////////////////////////////////////////////////////////
} // namespace PMVendorAttr
///////////////////////////////////////////////////////////////////

#endif // PMVendorAttr_h

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

  File:       ModulePkgError.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Hook for "PkgModule" error values.

/-*/
#ifndef ModulePkgError_h
#define ModulePkgError_h

#include <iosfwd>

#include <y2pm/PMError.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : ModulePkgError
/**
 * @short Hook for "PkgModule" error values.
 * @see PMError
 *
 * PMError reserves an error class for the PkgModule. In presence
 * of the PkgModule, PkgModuleError will redirect errtext to
 * it's own version.
 **/
class ModulePkgError {

  private:

    friend class PMError;

    static const std::string errclass;

    static std::string errtext( const unsigned e );

  protected:

    typedef std::string (*errtextFnc)( const unsigned );

    static errtextFnc errtextfnc;
};

///////////////////////////////////////////////////////////////////

#endif // ModulePkgError_h


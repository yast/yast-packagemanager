/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                     (C) 2002 SuSE AG |
\----------------------------------------------------------------------/

   File:       FileDeps.h
   Purpose:    Defines for file requirements
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/


#ifndef FileDeps_h
#define FileDeps_h

#include <set>
#include <y2pm/PkgName.h>

namespace FileDeps
{
    
/** set of Filenames */
typedef std::set<PkgName> FileNames;

}

#endif

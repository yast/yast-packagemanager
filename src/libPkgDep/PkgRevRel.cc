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

   File:       CommonPkdParser.cc
   Purpose:    Implements reverse relation
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/

#include <y2pm/PkgRevRel.h>

const PkgRelation PkgRevRel::relation() const
{
    return _relation ? *_relation : _pkg->self_provides();
}

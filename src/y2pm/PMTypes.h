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

  File:       PMTypes.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef PMTypes_h
#define PMTypes_h

#include <set>
#include <map>
#include <list>
#include <vector>

#include <y2util/LangCode.h>

#include <y2pm/PMError.h>
#include <y2pm/PkgArch.h>

#include <y2pm/InstSrcPtr.h>

class Y2PM;;
class InstTarget;
class InstSrcManager;
class PMPackageManager;
class PMYouPatchManager;
class PMSelectionManager;

namespace PM {

  ///////////////////////////////////////////////////////////////////
  // Y2PM
  ///////////////////////////////////////////////////////////////////

  typedef std::set<LangCode>  LocaleSet;
  typedef std::list<LangCode> LocaleOrder;

  typedef std::list<PkgArch>  ArchSet;

  ///////////////////////////////////////////////////////////////////
  // InstSrc / InstSrcManager
  ///////////////////////////////////////////////////////////////////

  typedef unsigned          NumericISrcID;

  typedef constInstSrcPtr   ISrcId;
  typedef std::list<ISrcId> ISrcIdList;

  typedef std::vector<NumericISrcID> InstOrder;

  typedef std::pair<NumericISrcID, bool> SrcState;
  typedef std::vector<SrcState>          SrcStateVector;


  ///////////////////////////////////////////////////////////////////
  //
  ///////////////////////////////////////////////////////////////////

} // namespace PM

#endif // PMTypes_h

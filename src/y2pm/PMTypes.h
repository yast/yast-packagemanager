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

#include <y2pm/PMError.h>
#include <y2pm/PkgArch.h>
#include <y2pm/PMLangCode.h>


#include <y2pm/InstSrcPtr.h>

class Y2PM;;
class InstTarget;
class InstSrcManager;
class PMPackageManager;
class PMYouPatchManager;
class PMSelectionManager;
class PMLanguageManager;

namespace PM {

  ///////////////////////////////////////////////////////////////////
  // Y2PM
  ///////////////////////////////////////////////////////////////////

  typedef std::list<PkgArch>  ArchSet;

  ///////////////////////////////////////////////////////////////////
  // InstSrc / InstSrcManager
  ///////////////////////////////////////////////////////////////////

  typedef unsigned          NumericISrcID;

  typedef constInstSrcPtr   ISrcId;
  typedef std::list<ISrcId> ISrcIdList;

  typedef std::vector<NumericISrcID> InstOrder;

  struct SrcState
  {
    NumericISrcID _id;
    bool          _autoenable;
    bool          _autorefresh;

    SrcState();         // impl. in InstSrcManager.cc
    explicit
    SrcState( ISrcId ); // impl. in InstSrcManager.cc
  };
  typedef std::vector<SrcState> SrcStateVector;


  ///////////////////////////////////////////////////////////////////
  // Selectable
  ///////////////////////////////////////////////////////////////////

  enum CandidateOrder {
    CO_DEFAULT, CO_AVS, CO_ASV
  };

  ///////////////////////////////////////////////////////////////////
  //
  ///////////////////////////////////////////////////////////////////

} // namespace PM

#endif // PMTypes_h

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

  File:       PMRcValues.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef PMRcValues_h
#define PMRcValues_h

#include <iosfwd>

#include <y2pm/PMTypes.h>

///////////////////////////////////////////////////////////////////
namespace PM {
;//////////////////////////////////////////////////////////////////

struct RcValues {
  PM::LocaleSet         initialRequestedLocales;
  PM::LocaleSet         requestedLocales;
  PM::CandidateOrder    candidateOrder;
  std::set<std::string> trustedVendors;
};

extern RcValues & rcValues(); // Y2PM.rcvalue.cc

///////////////////////////////////////////////////////////////////
} // namespace PM
///////////////////////////////////////////////////////////////////

#endif // PMRcValues_h

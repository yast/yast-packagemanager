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

  File:       PMPackageImEx.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef PMPackageImEx_h
#define PMPackageImEx_h

#include <iosfwd>
#include <set>

#include <y2pm/PMPackageImExPtr.h>
#include <y2pm/PMSelectablePtr.h>
#include <y2pm/PkgName.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageImEx
/**
 *
 **/
class PMPackageImEx : virtual public Rep {
  REP_BODY(PMPackageImEx);

  private:

    std::set<PkgNameEd> _onSystemPkg;
    std::set<PkgNameEd> _onSystemTabooPkg;
    std::set<PkgName>   _offSystemPkg;
    std::set<PkgName>   _offSystemTabooPkg;

    void reset();

    bool collect_Pkg( const constPMSelectablePtr & sel_r );

  public:

    PMPackageImEx();

    virtual ~PMPackageImEx();

  public:

    std::istream & doImport( std::istream & str );
    std::ostream & doExport( std::ostream & str ) const;

    void getPMState();
    void setPMState();

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMPackageImEx_h


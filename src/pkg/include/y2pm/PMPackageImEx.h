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

#include <y2util/Pathname.h>
#include <y2util/LangCode.h>

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

    struct Magic {
      static const std::string _magic;
      PkgEdition               _version;
      Magic( const PkgEdition & vers_r = PkgEdition::UNSPEC ) : _version( vers_r ) {}
      std::istream & readFrom( std::istream & str );
      std::ostream & writeOn( std::ostream & str ) const;
    };

    static const Magic _ImExMagic;

  private:

    std::list<LangCode> _requestedLocales;

    std::set<PkgNameEd> _onSystemSel;
    std::set<PkgName>   _offSystemSel;

    std::set<PkgNameEd> _onSystemPkg;
    std::set<PkgNameEd> _onSystemTabooPkg;
    std::set<PkgName>   _offSystemPkg;
    std::set<PkgName>   _offSystemTabooPkg;

    void reset();

    bool collect_Sel( const constPMSelectablePtr & sel_r );
    bool collect_Pkg( const constPMSelectablePtr & sel_r );

  public:

    PMPackageImEx();

    virtual ~PMPackageImEx();

  public:

    void getPMState();
    void setPMState();

  public:

    std::istream & doImport( std::istream & str );
    std::ostream & doExport( std::ostream & str ) const;

    bool doImport( const Pathname & path_r );
    bool doExport( const Pathname & path_r ) const;

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMPackageImEx_h


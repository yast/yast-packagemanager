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

  Purpose: Package/Selection import and export.

/-*/
#ifndef PMPackageImEx_h
#define PMPackageImEx_h

#include <iosfwd>
#include <set>
#include <map>

#include <y2util/Pathname.h>
#include <y2util/LangCode.h>

#include <y2pm/PMPackageImExPtr.h>
#include <y2pm/PMSelectablePtr.h>
#include <y2pm/PkgName.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageImEx
/**
 * @short Package/Selection import and export.
 *
 * <code>PMPackageImEx</code> is able to remember the current systems
 * packages/selections. This information might be written to and restored
 * from file.
 *
 * Previously remember information may be used to arrange package/selection
 * managers settings, to restore the systems content as close as possible.
 **/
class PMPackageImEx : public CountedRep {
  REP_BODY(PMPackageImEx);

  private:

    ///////////////////////////////////////////////////////////////////
    //
    //	CLASS NAME : PMPackageImEx::Magic
    /**
     * @short Helper to handle PMPackageImEx file magic.
     **/
    struct Magic {
      static const std::string _magic;
      PkgEdition               _version;
      Magic( const PkgEdition & vers_r = PkgEdition::UNSPEC ) : _version( vers_r ) {}
      std::istream & readFrom( std::istream & str );
      std::ostream & writeOn( std::ostream & str ) const;
    };

    static const Magic _ImExMagic;

  public:

    typedef std::set<PkgName>            NameSet;
    typedef std::map<PkgName,PkgEdition> NameEdSet;

  private:

    std::set<LangCode> _requestedLocales;

    NameEdSet _onSystemSel;
    NameSet   _offSystemSel;

    NameEdSet _onSystemPkg;
    NameEdSet _onSystemTabooPkg;
    NameSet   _offSystemPkg;
    NameSet   _offSystemTabooPkg;

    /**
     * Froget previously remembered data.
     **/
    void reset();

    /**
     * Decides whether to remember data for the given selection Selectable.
     * If so, data are stored.
     **/
    bool collect_Sel( const constPMSelectablePtr & sel_r );
    /**
     * Decides whether to remember data for the given package Selectable.
     * If so, data are stored.
     **/
    bool collect_Pkg( const constPMSelectablePtr & sel_r );

  public:

    /**
     * Constructor
     **/
    PMPackageImEx();

    /**
     * Destructor
     **/
    virtual ~PMPackageImEx();

  public:

    /**
     * Remember the current Package/SelectionManagers state.
     **/
    void getPMState();
    /**
     * Restore Package/SelectionManagers state according to the
     * remembered data..
     **/
    void setPMState();

  public:

    /**
     * Read back PMPackageImEx data from stream. On any error
     * (reading or parsing) the streams FAIL and/or BAD bit is set.
     **/
    std::istream & doImport( std::istream & str );
    /**
     * Write currently remembered PMPackageImEx data to stream.
     * On any error the streams FAIL and/or BAD bit is set.
     **/
    std::ostream & doExport( std::ostream & str ) const;

    /**
     * Expect path_r to be a readable PMPackageImEx file, and
     * read it's data. Return false on any error.
     **/
    bool doImport( const Pathname & path_r );
    /**
     * Expect path_r to denote a writable PMPackageImEx file, and
     * store currently remembered PMPackageImEx data in it. Return
     * false on any error. (we do not unlink any files on error!)
     **/
    bool doExport( const Pathname & path_r ) const;

  public:

    /**
     * Dump some debug lines.
     **/
    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMPackageImEx_h


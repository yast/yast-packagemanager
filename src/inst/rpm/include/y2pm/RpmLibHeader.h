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

  File:       RpmLibHeader.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef RpmLibHeader_h
#define RpmLibHeader_h

#include <iosfwd>
#include <list>

#include <y2pm/RpmLibHeaderPtr.h>
#include <y2pm/PMPackage.h>
#include <y2pm/FileDeps.h>
#include <y2pm/PkgChangelog.h>

class PkgDu;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : RpmLibHeader
/**
 * @short Wrapper class for rpm header struct.
 *
 * <code>RpmLibHeader</code> provides methods to query the content
 * of a rpm header struct retrieved from the RPM database or by reading
 * the rpm header of a package on disk.
 *
 * The rpm header contains all data associated with a package. So you
 * probabely do not want to permanently store too many of them.
 *
 * <B>NEVER create <code>RpmLibHeader</code> from a NULL <code>Header</code>! </B>
 **/
class RpmLibHeader : virtual public Rep {
  REP_BODY(RpmLibHeader); // includes no cpoy, no assign

  private:

    typedef int tag;
    typedef struct headerToken * Header;

    class filemode;

  private:

    Header _h;

  private:

    bool has_tag( tag tag_r ) const;

  private:

    class intList;
    unsigned int_list( tag tag_r, intList & lst_r ) const;

    class stringList;
    unsigned string_list( tag tag_r, stringList & lst_r ) const;

  private:

    int int_val( tag tag_r ) const;

    std::string string_val( tag tag_r ) const;

    std::list<std::string> stringList_val( tag tag_r ) const;

    PMSolvable::PkgRelList_type PkgRelList_val( tag tag_r, FileDeps::FileNames * freq_r = 0 ) const;

  public:

    /**
     * <B>NEVER create <code>RpmLibHeader</code> from a NULL <code>Header</code>! </B>
     **/
    RpmLibHeader( Header h );

    virtual ~RpmLibHeader();

  public:

    PkgName    tag_name()        const;
    PkgEdition tag_edition()     const;
    PkgArch    tag_arch()        const;

    Date       tag_installtime() const;
    Date       tag_buildtime()   const;

    /**
     * Dependencies referencing the package itself are filtered out.
     * If <code>freq_r</code> is not NULL, file dependencies found are inserted.
     **/
    PMSolvable::PkgRelList_type tag_provides ( FileDeps::FileNames * freq_r = 0 ) const;
    /**
     * @see #tag_provides
     **/
    PMSolvable::PkgRelList_type tag_requires ( FileDeps::FileNames * freq_r = 0 ) const;
    /**
     * @see #tag_provides
     **/
    PMSolvable::PkgRelList_type tag_conflicts( FileDeps::FileNames * freq_r = 0 ) const;
    /**
     * @see #tag_provides
     **/
    PMSolvable::PkgRelList_type tag_obsoletes( FileDeps::FileNames * freq_r = 0 ) const;

    FSize tag_size()        const;
    FSize tag_archivesize() const;

    std::string tag_summary()      const;
    std::string tag_description()  const;
    std::string tag_group()        const;
    std::string tag_vendor()       const;
    std::string tag_distribution() const;
    std::string tag_license()      const;
    std::string tag_buildhost()    const;
    std::string tag_packager()     const;
    std::string tag_url()          const;
    std::string tag_os()           const;
    std::string tag_prein()        const;
    std::string tag_postin()       const;
    std::string tag_preun()        const;
    std::string tag_postun()       const;
    std::string tag_sourcerpm()    const;

    std::list<std::string> tag_filenames() const;

    PkgChangelog tag_changelog() const;

    /**
     * Returns reference to arg <code>dudata_r</code>.
     **/
    PkgDu & tag_du( PkgDu & dudata_r ) const;

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // RpmLibHeader_h


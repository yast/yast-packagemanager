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

   File:       InstSrcSuSE.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef InstSrcSuSE_h
#define InstSrcSuSE_h

#include <iosfwd>

#include <y2pm/InstSrcSuSEPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcSuSE
/**
 *
 **/
class InstSrcSuSE: virtual public Rep, public InstSrc {
  REP_BODY(InstSrcSuSE)

  public:

    InstSrcSuSE( MediaInfoPtr media_r );

    virtual ~InstSrcSuSE();

  public:
  
    virtual int numItems();

    virtual bool Activate();

    virtual bool Deactivate();

    virtual bool Erase();

    virtual std::list<PMPackagePtr> getPackages();
    
    virtual std::list<PMPackagePtr> Immediate(const std::list<PMPackagePtr>& l);

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // InstSrcSuSE_h


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

  File:       PMLanguage.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef PMLanguage_h
#define PMLanguage_h

#include <iosfwd>

#include <y2pm/PMLanguagePtr.h>
#include <y2pm/PMObject.h>
#include <y2pm/PMLangCode.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMLanguage
/**
 *
 **/
class PMLanguage : public PMObject {
  REP_BODY(PMLanguage);

  private:

    LangCode _lang;

  public:

    PMLanguage( const LangCode & lang_r );

    virtual ~PMLanguage();

    LangCode langCode() const { return _lang; }

  public:

    /**
     * PMObject attributes that should be realized by each concrete Object.
     * Mostly because the UI likes to have some data to show. If there are
     * no data a concrete Object could provide, call PMObjects default
     * implementation.
     **/
    virtual std::string            summary()         const;
    virtual std::list<std::string> description()     const;
    virtual std::list<std::string> insnotify()       const;
    virtual std::list<std::string> delnotify()       const;
    virtual FSize                  size()            const;
    virtual bool                   providesSources() const;

    virtual std::string            instSrcLabel()    const;
    virtual Vendor                 instSrcVendor()   const;
    virtual unsigned               instSrcRank()     const; // rank == 0 is highest!

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMLanguage_h


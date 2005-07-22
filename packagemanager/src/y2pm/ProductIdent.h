/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                    (C) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:       ProductIdent.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef ProductIdent_h
#define ProductIdent_h

#include <iosfwd>

#include <y2pm/PkgName.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : ProductIdent
/**
 * @short Store product name and version
 *
 * Handle constraints on setting/comparing ProductIdents:
 *
 * - It's illegal to have a name without version or vice versa!
 * Some broken contentfiles define name without version instead of
 * nothing at all. Constructor maps these forms to 'undefined product'.
 * (see @ref _initCheck)
 *
 * - Actually a product lines product name should not be changed!
 * However 'SuSE' was changed to 'SUSE' and 'SUSE CORE' replaces
 * 'UnitedLinux' :(
 * (see @ref isLine)
 *
 * <B>NOTE:</B> @ref asPkgNameEd is provided for convenience and backwards
 * compatibility. It's ok to retirieve a products name and version, but
 * comparison of two product names/versions should be done by calling the
 * methods provided here.
 **/
class ProductIdent {

  friend std::ostream & operator<<( std::ostream & str, const ProductIdent & obj );

  private:

    PkgNameEd _product;

    void _initCheck();

  public:

    ProductIdent();
    ProductIdent( const PkgNameEd & rhs );
    ProductIdent( const PkgName & n, const PkgEdition & e );

    ~ProductIdent();

  public:

    /**
     * Is an empty ProductIdent
     **/
    bool undefined() const {
      return( _product.name->empty() );
    }

    /**
     * Is product line name_r.
     **/
    bool isLine( std::string name_r ) const;

    /**
     * Same product line
     **/
    bool sameLine( const ProductIdent & rhs ) const {
      return isLine( rhs._product.name.asString() );
    }

    /**
     * Equal version
     **/
    bool sameVersion( const ProductIdent & rhs ) const {
      return( _product.edition == rhs._product.edition  );
    }

    /**
     * Same line and equal version
     **/
    bool sameProduct( const ProductIdent & rhs ) const {
      return( sameLine( rhs ) && sameVersion( rhs ) );
    }

    /**
     * This obsoletes rhs when installed in productDB
     **/
    bool obsoletes( const ProductIdent & rhs ) const;

    /**
     * String representation
     **/
    std::string asString() const { return _product.asString(); }

    /**
     * PkgNameEd representation
     **/
    PkgNameEd asPkgNameEd() const { return _product; }

  public:

    /**
     * Convert ProductIdent to string (on save to file).
     * <b>Keep it compatible with fromString.</b>
     **/
    static std::string toString( const ProductIdent & t ) {
      return PkgNameEd::toString( t._product );
    }

    /**
     * Restore ProductIdent from string (on restore from file).
     * <b>Keep it compatible with toString.</b>
     **/
    static ProductIdent fromString( std::string s ) {
      return PkgNameEd::fromString( s );
    }

};

///////////////////////////////////////////////////////////////////

#endif // ProductIdent_h

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

   File:       PMObjectContainerIter.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMObjectContainerIter_h
#define PMObjectContainerIter_h

#include <iosfwd>
#include <vector>
#include <list>
#include <set>

#include <y2pm/PMObjectPtr.h>
#include <y2pm/PMPackagePtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMObjectContainerIter
/**
 * A PMObjectContainerIter allows to iterate various container classes
 * containing PMObjectPtr or derived classes. The concrete container to
 * iterate is passed as const & to the constructor and hidden inside a
 * pivate helperclass.
 *
 * The public interface allows to iterate all PMObjectPtr stored in the
 * container whithout need to know the concrete containers type.
 *
 * Example:
 * <PRE>
 * void DoSomething( PMObjectContainerIter iter_r )
 * {
 *   if ( iter_r.size() == 0 )
 *     return; // container is empty
 *   for ( iter_r.setBegin(); !iter_r.atEnd(); iter_r.setNext() ) {
 *     PMObjectPtr elem = *iter_r;
 *     // do something with elem
 *   }
 * }
 * ...
 * {
 *   std::list<PMObjectPtr> objlist;
 *   std::set<PMPackagePtr> pkgset;
 *   ...
 *   DoSomething( objlist );
 *   DoSomething( pkgset );
 *   ...
 * }
 * </PRE>
 *
 * @short Helperclass to iterate vector/list/set of PMObjectPtr.
 **/
class PMObjectContainerIter {

  friend std::ostream & operator<<( std::ostream & str, const PMObjectContainerIter & obj );

  private:

    /**
     * @short Iteration interface to be provided by helper classes hiding a container.
     **/
    class ContBase {
      public:
	ContBase() {}
	virtual ~ContBase() {}
	virtual ContBase * clone() const = 0;
      public:
	virtual void setBegin() = 0;
	virtual void setNext() = 0;
	virtual bool atEnd() const = 0;
	virtual PMObjectPtr get() const = 0;
	virtual unsigned size() const = 0;
    };

    /**
     * @short Helper class hiding a std::vector, list or set.
     **/
    template <class Ptr> class Cont : public ContBase {
      private:
	const Ptr &                  _cont;
	typename Ptr::const_iterator _iter;
      public:
	Cont( const Ptr & cont_r ) : _cont( cont_r ) {
	  _iter = cont_r.begin();
	}
	virtual ~Cont() {}
	virtual ContBase * clone() const {
	  return new Cont( *this );
	}
      public:
 	virtual void setBegin() { _iter = _cont.begin(); }
	virtual void setNext() { ++_iter; }
	virtual bool atEnd() const { return ( _iter == _cont.end() ); }
	virtual PMObjectPtr get() const { return *_iter; }
	virtual unsigned size() const { return _cont.size(); }
   };

  private:

    ContBase * _cont;

  public:

    PMObjectContainerIter( const std::vector<PMObjectPtr> & cont_r ) {
      _cont = new Cont<std::vector<PMObjectPtr> >( cont_r );
    }
    PMObjectContainerIter( const std::list<PMObjectPtr> & cont_r ) {
      _cont = new Cont<std::list<PMObjectPtr> >( cont_r );
    }
    PMObjectContainerIter( const std::set<PMObjectPtr> & cont_r ) {
      _cont = new Cont<std::set<PMObjectPtr> >( cont_r );
    }

    PMObjectContainerIter( const std::vector<PMPackagePtr> & cont_r ) {
      _cont = new Cont<std::vector<PMPackagePtr> >( cont_r );
    }
    PMObjectContainerIter( const std::list<PMPackagePtr> & cont_r ) {
      _cont = new Cont<std::list<PMPackagePtr> >( cont_r );
    }
    PMObjectContainerIter( const std::set<PMPackagePtr> & cont_r ) {
      _cont = new Cont<std::set<PMPackagePtr> >( cont_r );
    }


    PMObjectContainerIter( const PMObjectContainerIter & rhs ) : _cont( 0 ) {
      if ( rhs._cont ) {
	_cont = rhs._cont->clone();
      }
    }

    PMObjectContainerIter & operator = ( const PMObjectContainerIter & rhs ) {
      if ( &rhs != this ) {
	delete _cont;
	if ( rhs._cont ) {
	  _cont = rhs._cont->clone();
	} else {
	  _cont = 0;
	}
      }
      return *this;
    }

    ~PMObjectContainerIter() {
      delete _cont;
    }

  public:

    /**
     * Set iterator to the fist element.
     **/
    void setBegin() { _cont->setBegin(); }

    /**
     * Advance iterator.
     **/
    void setNext() { _cont->setNext(); }

    /**
     * Return true if the iterator reached the end of the container.
     **/
    bool atEnd() const { return _cont->atEnd(); }

    /**
     * Return the PMObjectPtr located at the current iterator position.
     * If atEnd() is true, the behaviour is undefined.
     **/
    PMObjectPtr operator * () const { return _cont->get(); }

    PMObjectPtr operator -> () const { return _cont->get(); }

    /**
     * Return the containers size.
     **/
    unsigned size() const { return _cont->size(); }

    /**
     * Return true if the container is empty.
     **/
    bool empty() const { return ( size() == 0 ); }
};

///////////////////////////////////////////////////////////////////

#endif // PMObjectContainerIter_h

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

#include <vector>
#include <list>
#include <set>

#include <y2pm/PMObjectPtr.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/PMSelectionPtr.h>
#include <y2pm/PMYouPatchPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMObjectContainerIter
/**
 * A PMObjectContainerIter allows to iterate various container classes
 * containing PMObjectPtr or derived classes. The concrete container to
 * iterate is passed as const & to the constructor and hidden inside a
 * pivate helper class.
 *
 * The public interface allows to iterate all PMObjectPtr stored in the
 * container whithout need to know the concrete containers type.
 *
 * Example:
 *
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
 *   std::list&lt;PMObjectPtr> objlist;
 *   std::set&lt;PMPackagePtr> pkgset;  // PMPackagePtr inherits PMObjectPtr
 *   ...
 *   DoSomething( objlist );
 *   DoSomething( pkgset );
 *   ...
 * }
 * </PRE><P>
 *
 * <B>Adding a new container type</B> is quite simple.<CODE>class PMObjectContainerIter::ContBase</CODE>
 * defines the interface any helper class must provide. Create, or use an appropriate existing,
 * helper class derived from <CODE>PMObjectContainerIter::ContBase</CODE>. Provide a constructor
 * that takes the new container type as argument, creates the helper class from it, and stores it
 * in <CODE>_cont</CODE>.
 *
 * @short Helperclass to iterate various container types of PMObjectPtr.
 **/
class PMObjectContainerIter {

  private:

    /**
     * @short Iteration interface to be provided by helper classes hiding a container.
     **/
    class ContBase {
      public:
	ContBase() {}
	virtual ~ContBase() {}
	virtual ContBase *  clone()    const = 0;
      public:
	virtual void        setBegin()       = 0;
	virtual void        setNext()        = 0;
	virtual bool        atEnd()    const = 0;
	virtual PMObjectPtr get()      const = 0;
	virtual unsigned    size()     const = 0;
    };

    /**
     * @short Helper class hiding a std::vector, list or set.
     **/
    template <class Ptr> class StdCont : public ContBase {
      private:
	const Ptr &                  _cont;
	typename Ptr::const_iterator _iter;

	template <typename Tp>
	PMObjectPtr _get( const Tp & ) const { return *_iter; }

	template <typename Tp,class ObjPtr>
	PMObjectPtr _get( const std::map<Tp,ObjPtr> & ) const { return _iter->second; }

      public:
	StdCont( const Ptr & cont_r ) : _cont( cont_r ) { _iter = cont_r.begin(); }
	virtual ~StdCont() {}
	virtual ContBase *  clone()    const { return new StdCont( *this ); }
      public:
 	virtual void        setBegin()       { _iter = _cont.begin(); }
	virtual void        setNext()        { ++_iter; }
	virtual bool        atEnd()    const { return ( _iter == _cont.end() ); }
	virtual PMObjectPtr get()      const { return _get( _cont ); }
	virtual unsigned    size()     const { return _cont.size(); }
   };


  private:

    ContBase * _cont;

  public:
    template <class ObjPtr>
    PMObjectContainerIter( const std::vector<ObjPtr> & cont_r ) {
      _cont = new StdCont<std::vector<ObjPtr> >( cont_r );
    }
    template <class ObjPtr>
    PMObjectContainerIter( const std::list<ObjPtr> & cont_r ) {
      _cont = new StdCont<std::list<ObjPtr> >( cont_r );
    }
    template <class ObjPtr>
    PMObjectContainerIter( const std::set<ObjPtr> & cont_r ) {
      _cont = new StdCont<std::set<ObjPtr> >( cont_r );
    }
    template <typename Tp,class ObjPtr>
    PMObjectContainerIter( const std::map<Tp,ObjPtr> & cont_r ) {
      _cont = new StdCont<std::map<Tp,ObjPtr> >( cont_r );
    }

#if 0
    PMObjectContainerIter( const std::vector<PMObjectPtr> & cont_r ) {
      _cont = new StdCont<std::vector<PMObjectPtr> >( cont_r );
    }
    PMObjectContainerIter( const std::list<PMObjectPtr> & cont_r ) {
      _cont = new StdCont<std::list<PMObjectPtr> >( cont_r );
    }
    PMObjectContainerIter( const std::set<PMObjectPtr> & cont_r ) {
      _cont = new StdCont<std::set<PMObjectPtr> >( cont_r );
    }

    PMObjectContainerIter( const std::vector<PMPackagePtr> & cont_r ) {
      _cont = new StdCont<std::vector<PMPackagePtr> >( cont_r );
    }
    PMObjectContainerIter( const std::list<PMPackagePtr> & cont_r ) {
      _cont = new StdCont<std::list<PMPackagePtr> >( cont_r );
    }
    PMObjectContainerIter( const std::set<PMPackagePtr> & cont_r ) {
      _cont = new StdCont<std::set<PMPackagePtr> >( cont_r );
    }

    PMObjectContainerIter( const std::vector<PMSelectionPtr> & cont_r ) {
      _cont = new StdCont<std::vector<PMSelectionPtr> >( cont_r );
    }
    PMObjectContainerIter( const std::list<PMSelectionPtr> & cont_r ) {
      _cont = new StdCont<std::list<PMSelectionPtr> >( cont_r );
    }
    PMObjectContainerIter( const std::set<PMSelectionPtr> & cont_r ) {
      _cont = new StdCont<std::set<PMSelectionPtr> >( cont_r );
    }

    PMObjectContainerIter( const std::vector<PMYouPatchPtr> & cont_r ) {
      _cont = new StdCont<std::vector<PMYouPatchPtr> >( cont_r );
    }
    PMObjectContainerIter( const std::list<PMYouPatchPtr> & cont_r ) {
      _cont = new StdCont<std::list<PMYouPatchPtr> >( cont_r );
    }
    PMObjectContainerIter( const std::set<PMYouPatchPtr> & cont_r ) {
      _cont = new StdCont<std::set<PMYouPatchPtr> >( cont_r );
    }
#endif

  public:

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
     * Advance iterator to next element.
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

    /**
     * Access the PMObject referenced by the PMObjectPtr located at the current
     * iterator position. If atEnd() is true, the behaviour is undefined.
     * It's a shortcut for <CODE>(*iter)-></CODE>.
     *
     * Keep in mind that <CODE>operator -></CODE>is special. The <CODE>PMObjectPtr</CODE>
     * returned is in fact an intermediate result to which the base semantics of
     * <CODE>-></CODE> is then applied, yielding a result.
     **/
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

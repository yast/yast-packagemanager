#define _GLIBCXX_CONCEPT_CHECKS
#include <iomanip>
#include <fstream>
#include <string>

#include <y2util/Y2SLog.h>

#include <y2pm/YUMParserData.h>
#include <y2pm/YUMRepomdParser.h>
#include <y2pm/YUMPrimaryParser.h>
#include <y2pm/YUMGroupParser.h>
#include <y2pm/YUMFileListParser.h>
#include <y2pm/YUMOtherParser.h>

using namespace std;
using namespace YUM;

#undef Y2LOG
#define Y2LOG "MA-TEST"

#if 0
#define _IsUnused __attribute__ ((__unused__))

template <class _Concept>
  inline void __function_requires()
  {
    void (_Concept::*__x)() _IsUnused = &_Concept::__constraints;
  }

template <class _Tp>
  struct _CopyConstructibleConcept
  {
    void __constraints() {
      _Tp __a(__b);                     // require copy constructor
      _Tp* __ptr _IsUnused = &__a;      // require address of operator
      __const_constraints(__a);
    }
    void __const_constraints(const _Tp& __a) {
      _Tp __c _IsUnused(__a);           // require const copy constructor
      const _Tp* __ptr _IsUnused = &__a; // require const address of operator
    }
    _Tp __b;
  };
#endif
///////////////////////////////////////////////////////////////////
template<typename _Tp>
  struct _Verbose
  {
    friend std::ostream & operator<<( std::ostream & str, const _Verbose & obj_r )
    { return str << __PRETTY_FUNCTION__ << obj_r._obj; }
  public:
    explicit
    _Verbose( const _Tp & obj_r )
    : _obj( obj_r )
    { __glibcxx_function_requires( _CopyConstructibleConcept<_Tp> ); }
  private:
    const _Tp & _obj;
  };

template<typename _Tp>
  _Verbose<_Tp> mverbose( const _Tp & obj_r )
  {
    return _Verbose<_Tp>( obj_r ); }

///////////////////////////////////////////////////////////////////

std::ostream & operator<<( std::ostream & str, const _Verbose<ifstream> & obj_r )
{ return str << __PRETTY_FUNCTION__ << 13;}





/******************************************************************
**
**
**	FUNCTION NAME : main
**	FUNCTION TYPE : int
**
**	DESCRIPTION :
*/
int main( int argc, char * argv[] )
{
  set_log_filename( "-" );
  MIL << "START" << endl;
  --argc;
  ++argv;
  string fin( "" );
  if ( argc )
    fin = argv[0];

  list<YUMDirSize> L;
  L.push_back( YUM::YUMDirSize("foo","baa","kah") );

  YUM::YUMDirSize __a;
  YUM::YUMDirSize __b;
  __a = __b;

  const YUM::YUMDirSize & __c( __a );
  YUM::YUMDirSize __d( __c );
  __a = __c;



  YUM::YUMDirSize a("foo","baa","kah");
  YUM::YUMDirSize b = a;
  MIL << a << endl;
  MIL << b << endl;
  ifstream in( "/Local/EXPORT/YUM-9.3/repodata/repomd.xml" );
  for ( YUMRepomdParser iter(in,"");
        !iter.atEnd();
        ++iter)
    {
      cout << **iter;
    }
  //SEC << mverbose(in) << endl;
  in.close();


  SEC << "STOP" << endl;
  return 0;
}



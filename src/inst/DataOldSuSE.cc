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

   File:       DataOldSuSE.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

    Purpose:	helper calls for InstSrcData
		is able to read and parse 'old format' SuSE media
		content descriptions
		namely suse/setup/descr/common.pkd for packages
		and suse/setup/descr/ *.sel for selections

		a possibly extension is reading
		update/<arch>/<product>/<version>/ from an ftp patch repository
/-*/

#include <iostream>
#include <fstream>
#include <string>

#include <y2util/CommonPkdParser.h>
#include <y2util/Y2SLog.h>
#include <y2pm/DataOldSuSE.h>
#include <y2pm/PkgName.h>
#include <y2pm/PkgEdition.h>
#include <y2pm/PkgRelation.h>
#include <y2pm/PMPackage.h>

using namespace std;

//--------------------------------------------------------------------
// suse/setup/descr/common.pkd handling

#define CREATETAG(tagname,num) \
t = new CommonPkdParser::Tag(tagname,CommonPkdParser::Tag::ACCEPTONCE); \
this->addTag(t); \
addTagByIndex(num,t);

class CommonPkdTagSet : public CommonPkdParser::TagSet
{
public:
    enum Tags {
	FILENAME,
	SERIES,
	RPMNAME,
	INSTPATH,
	SIZE,
	BUILDTIME,
	BUILTFROM,
	FLAG,
	RPMGROUP,
	COPYRIGHT,
	AUTHORNAME,
	VERSION,
	OBSOLETES,
	REQUIRES,
	PROVIDES,
	CONFLICTS,
	ARCH,
	NUM_TAGS
    };
public:
    CommonPkdTagSet() : TagSet()
    {
	CommonPkdParser::Tag* t;
	CREATETAG("Filename",FILENAME)
	CREATETAG("Series",SERIES)
	CREATETAG("RpmName",RPMNAME)
	CREATETAG("InstPath",INSTPATH)
	CREATETAG("Size",SIZE)
	CREATETAG("Buildtime",BUILDTIME)
	CREATETAG("BuiltFrom",BUILTFROM)
	CREATETAG("Flag",FLAG)
	CREATETAG("RpmGroup",RPMGROUP)
	CREATETAG("Copyright",COPYRIGHT)
	CREATETAG("AuthorName",AUTHORNAME)
	CREATETAG("Version",VERSION)
	CREATETAG("Obsoletes",OBSOLETES)
	CREATETAG("Requires",REQUIRES)
	CREATETAG("Provides",PROVIDES)
	CREATETAG("Conflicts",CONFLICTS)
	CREATETAG("Architecture",ARCH)
    }
};


// TODO ugly, needs to be class member

///////////////////////////////////////////////////////////////////
//
// Check wheter a string defines a valid DepCompare. ('!=' is not supported by rpm)
//
inline rel_op
string2DepCompare( const string & str_tr )
{

  enum DepCompare { // ('!=' is not supported by rpm)
    DNONE = 0x00,
    DEQ   = 0x01,
    DLT   = 0x10,
    DGT   = 0x20,
    DLTE  = DLT|DEQ,
    DGTE  = DGT|DEQ,
  };

  unsigned ret_ei = NONE;
  switch( str_tr.size() ) {
  case 2:
    switch ( str_tr[1] ) {
    case '=': ret_ei |= DEQ; break;
    case '<': ret_ei |= DLT; break;
    case '>': ret_ei |= DGT; break;
    default:  return NONE;
    }
    // fall through
  case 1:
    switch ( str_tr[0] ) {
    case '=': ret_ei |= DEQ; break;
    case '<': ret_ei |= DLT; break;
    case '>': ret_ei |= DGT; break;
    default:  return NONE;
    }
    break;
  default:
    return NONE;
    break;
  }
  if(ret_ei == (DLT|DGT))
    ret_ei = NONE;

  switch(ret_ei)
  {
    case DEQ:
      return EQ;
    case DLT:
      return LT;
    case DGT:
      return GT;
    case DLTE:
      return LE;
    case DGTE:
      return GE;
    default:
      return NONE;
  }
}

/** parse dep string as found in common.pkd
 * e.g. groff >= 1.17 less /bin/sh
 * */
static PMSolvable::PkgRelList_type
asDependList( const string & data_tr )
{
  PMSolvable::PkgRelList_type ret_VCi;
  vector<string> data_Vti( TagParser::split2words( data_tr ) );

  struct 
  {
    string name;
    rel_op compare;
    string version;
    void clear()
    {
      name = version.erase();
      compare=NONE;
    }
  } cdep_Ci;

  cdep_Ci.compare=NONE;
  rel_op depOp_ei = NONE;

  for ( unsigned i = 0; i < data_Vti.size(); ++i ) {
    depOp_ei = string2DepCompare( data_Vti[i] );

    if ( depOp_ei == NONE ) {
       // string value
      if ( cdep_Ci.name.empty() ) {           // no previous. remember new name
	cdep_Ci.name = data_Vti[i];
      } else if ( cdep_Ci.compare != NONE ) { // remember version after op and store
	cdep_Ci.version = data_Vti[i];
	PkgRelation dep(cdep_Ci.name.c_str(),cdep_Ci.compare,cdep_Ci.version.c_str());
	ret_VCi.push_back( dep );
	cdep_Ci.clear();
      } else {                                // store previous and remember new name
	PkgRelation dep(cdep_Ci.name.c_str(),cdep_Ci.compare,cdep_Ci.version.c_str());
	ret_VCi.push_back( dep );
	cdep_Ci.clear();
	cdep_Ci.name = data_Vti[i];
      }
    } else {
      // operator value
      if ( cdep_Ci.name.empty() || cdep_Ci.compare != NONE ) {
	ERR << "Missplaced operator " << op_str[depOp_ei] << " in dependency of "
	  << cdep_Ci.name << " (" << data_tr << ")" << endl;
	cdep_Ci.clear();
	break;
      } else {
	cdep_Ci.compare = depOp_ei;
      }
    }
  }

  if ( cdep_Ci.name.size() ) {
    if ( cdep_Ci.compare == NONE || cdep_Ci.version.size() ) {
      PkgRelation dep(cdep_Ci.name.c_str(),cdep_Ci.compare,cdep_Ci.version.c_str());
      ret_VCi.push_back( dep );
    } else {
	ERR << "Missplaced operator " << op_str[depOp_ei] << " in dependency of "
	  << cdep_Ci.name << " (" << data_tr << ")" << endl;
    }
  }

  return ret_VCi;
}


// *** comment missing

static inline PMPackagePtr
createPMPackageFromTagset( CommonPkdParser::TagSet* t)
{
  //FIXME error checking
  PkgName name = t->getTagByIndex(CommonPkdTagSet::RPMNAME)->Data();
  string verandrelstr = t->getTagByIndex(CommonPkdTagSet::VERSION)->Data();
  string arch = t->getTagByIndex(CommonPkdTagSet::ARCH)->Data();
  string ver;
  string rel;
  // seperate version from release
  string::size_type pos = verandrelstr.rfind('-');
  if(pos != string::npos)
  {
    rel = verandrelstr.substr(pos+1,string::npos);
  }
  ver = verandrelstr.substr(0,pos);
  PkgEdition edi(ver.c_str(),rel.c_str());
//		      PMSolvable::PkgRelList_type emptylist;
//		      // FIXME Architecture!
  PMPackagePtr p = new PMPackage(name,edi,arch);
  
  PMSolvable::PkgRelList_type requires =
	asDependList(t->getTagByIndex(CommonPkdTagSet::REQUIRES)->Data());

  p->setRequires(requires);

  return p;
}


/*
 * read suse/setup/descr/common.pkd and parse files
 */
std::list<PMPackagePtr> *
readCommonPkd (const Pathname &commonpkdpath)
{
    std::list<PMPackagePtr> *pkglist = new std::list<PMPackagePtr>;

    TagParser parser;
    std::string tagstr;

    MIL << "open " << commonpkdpath.asString() << std::endl;
    std::ifstream commonpkdstream(commonpkdpath.asString().c_str());
    if(!commonpkdstream)
    {
	ERR << commonpkdpath.asString() << ": file not found" << std::endl;
	return 0;
    }

    CommonPkdParser::TagSet* tagset;
    tagset = new CommonPkdTagSet();

    bool repeatassign = false;
    bool parse = true;
    while( parse && parser.lookupTag(commonpkdstream))
    {
	tagstr = parser.startTag();

	do
	{
	    switch(tagset->assign(tagstr.c_str(),parser,commonpkdstream))
	    {
		case CommonPkdParser::Tag::ACCEPTED:
		    repeatassign = false;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOMATCH:
		    repeatassign = false;
		    break;
		case CommonPkdParser::Tag::REJECTED_FULL:
		    if( tagstr != "Filename" )
		    {
			std::cerr << "syntax error" << std::endl;
			parse = false;
		    }
		    {
		      PMPackagePtr p = createPMPackageFromTagset(tagset);
		      if(p != NULL)
			pkglist->push_back(p);
		    }
		    tagset->clear();
		    repeatassign = true;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOENDTAG:
		    repeatassign = false;
		    parse = false;
		    break;
	    }
	} while(repeatassign);
    }

    if(!parse)
      WAR << "parsing was aborted" << std::endl;
    else
    {
      MIL << "parsing finished" << std::endl;
      PMPackagePtr p = createPMPackageFromTagset(tagset);
      if(p != NULL)
	pkglist->push_back(p);
    }

    return pkglist;
}

//--------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : DataOldSuSE::DataOldSuSE
//	METHOD TYPE : Constructor
//
//	DESCRIPTION : read content data from media and
//			fill _selections, _packages, and _patches
//
DataOldSuSE::DataOldSuSE (MediaAccess *media)
{
    bool we_attached = false;

    // attach media at /var/adm/mount, if not already done

    Pathname attachpoint = media->getAttachPoint();
    if (attachpoint.empty())
    {
	attachpoint = Pathname ("/var/adm/mount");
	media->attachTo (attachpoint);
	we_attached = true;
    }

    /*
     * retrieve package data
     */

    Pathname commonpkdpath = Pathname ("suse/setup/descr/common.pkd");

    // provide file below attachpoint

    if (media->provideFile (commonpkdpath) == 0)
    {
	// read and parse local file copy
	_packages = readCommonPkd (attachpoint + commonpkdpath);
	// clean up local copy
	media->cleanUp (commonpkdpath);
    }

    /*
     * retrieve selections
     */

    Pathname seldirpath = Pathname ("suse/setup/descr");
    const std::list<std::string> *seldirnames = media->dirInfo (seldirpath);

    _selections = 0;

    for (std::list<std::string>::const_iterator name = seldirnames->begin();
	 name != seldirnames->end();
	 name++)
    {
	// check if name matches *.sel

	// parse files matching to PmSolvable

	// fill _selections
    }
    delete seldirnames;	// clean up memory

    /*
     * retrieve patches
     */

    // to be implemented

    _patches = 0;

    // release media if we attached it

    if (we_attached)
    {
	media->release ();
    }

    D__ << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : DataOldSuSE::~DataOldSuSE
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
DataOldSuSE::~DataOldSuSE()
{
}

//-----------------------------
// source content access

/**
 * generate PMSolvable objects for each selection on the source
 * @return list of PMSolvablePtr on this source
 */
const std::list<PMSolvablePtr> *
DataOldSuSE::getSelections() const
{
    return _selections;
}


/**
 * generate PMPackage objects for each Item on the source
 * @return list of PMPackagePtr on this source
 * */
const std::list<PMPackagePtr> *
DataOldSuSE::getPackages() const
{
    return _packages;
}


/**
 * generate PMSolvable objects for each patch on the source
 * @return list of PMSolvablePtr on this source
 */
const std::list<PMSolvablePtr> *
DataOldSuSE::getPatches() const
{
    return _patches;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : DataOldSuSE::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream &
DataOldSuSE::dumpOn( ostream & str ) const
{
    str << "DataOldSuSE::dumpOn()" << endl;
    return str;
}


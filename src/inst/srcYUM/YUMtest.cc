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

File:       YUMtest.cc

Author:     Michael Radziej <mir@suse.de>
Maintainer: Michael Radziej <mir@suse.de>

Purpose:    main() to test the YUM parsers
/-*/

#include <y2pm/YUMParser.h>
#include <y2util/RepDef.h>
#include <y2util/Y2SLog.h>

using namespace YUM;
using namespace std;

namespace {
  void usage() {
    cerr << "YUMtest usage: "<< endl
    << "YUMtest TYPE" << endl
    << "TYPE: repomd|primary|group|filelist|other" << endl;
  }
}


int main(int argc, char **argv)
{
  if (argc < 2) {
    usage();
    return 2;
  }

  set_log_filename("-");

  try {
    if (!strcmp(argv[1],"repomd")) {
      YUMRepomdParser iter(cin,"");
      for (;
           !iter.atEnd();
           ++iter) {
             cout << **iter;
           }
      if (iter.errorStatus())
        throw *iter.errorStatus();
    }
    else if (!strcmp(argv[1],"primary")) {
      YUMPrimaryParser iter(cin,"");
      for (;
           !iter.atEnd();
           ++iter) {
             cout << **iter;
           }
      if (iter.errorStatus())
        throw *iter.errorStatus();
    }
    else if (!strcmp(argv[1],"group")) {
      YUMGroupParser iter(cin,"");
      for (;
           !iter.atEnd();
           ++iter) {
             cout << **iter;
           }
      if (iter.errorStatus())
        throw *iter.errorStatus();
    }
    else if (!strcmp(argv[1],"filelist")) {
      YUMFileListParser iter(cin,"");
      for (;
           !iter.atEnd();
           ++iter) {
             cout << **iter;
           }
      if (iter.errorStatus())
        throw *iter.errorStatus();
    }
    else if (!strcmp(argv[1],"other")) {
      YUMOtherParser iter(cin,"");
      for (;
           !iter.atEnd();
           ++iter) {
             cout << **iter;
           }
      if (iter.errorStatus())
        throw *iter.errorStatus();
    }
    else {
      usage();
      return 2;
    }
  }  
  catch (XMLParserError& err) {
    cerr << "ouch" << endl;
  cerr << "syntax error encountered in XML input:" << endl
      << err.msg() << " " << err.position() << endl;
    return 1;
  }
  
           return 0;
}

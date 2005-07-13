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

#include <YUMRepomdParser.h>
#include <YUMPrimaryParser.h>
#include <YUMGroupParser.h>
#include <YUMFileListParser.h>
#include <YUMOtherParser.h>
#include <y2util/RepDef.h>
#include <YUMParserData.h>

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

  if (!strcmp(argv[1],"repomd")) {
    for (YUMRepomdParser iter(cin,"");
         !iter.atEnd();
         ++iter) {
           cout << **iter;
         }
  }
  else if (!strcmp(argv[1],"primary")) {
    for (YUMPrimaryParser iter(cin,"");
         !iter.atEnd();
         ++iter) {
           cout << **iter;
         }
  }
  else if (!strcmp(argv[1],"group")) {
    for (YUMGroupParser iter(cin,"");
         !iter.atEnd();
         ++iter) {
           cout << **iter;
         }
  }
  else if (!strcmp(argv[1],"filelist")) {
    for (YUMFileListParser iter(cin,"");
         !iter.atEnd();
         ++iter) {
           cout << **iter;
         }
  }
  else if (!strcmp(argv[1],"other")) {
    for (YUMOtherParser iter(cin,"");
         !iter.atEnd();
         ++iter) {
           cout << **iter;
         }
  }
  else {
    usage();
    return 2;
  }

  return 0;
}

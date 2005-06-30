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

  File:       YUMRepomdData.h

  Author:     Michael Radziej <mir@suse.de>
  Maintainer: Michael Radziej <mir@suse.de>

  Purpose: Parses <data> elements from a YUM repomd file
           and makes them accessible via iterator

*/

#include <stream>

class YUMRepomdParser {

public:
  YumRepomdParser (std::istream &istream);
  virtual ~YumRepomdParser();
  XMLNodeIterator<YUMRepomdDataPtr> parse(istream &is);
}




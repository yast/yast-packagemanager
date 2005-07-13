
CXXFLAGS = -ggdb -Wall -I. -I/usr/include/libxml2 -I/usr/include/YaST2 -l stdc++ -l xml2

# All:	YUMRepomdDataIterator.o YUMRepomdData.o LibXMLHelper.o test-YUMRepomdData.o

All: YUMtest

YUMtest:	YUMtest.o YUMParserData.o YUMRepomdParser.o YUMPrimaryParser.o YUMGroupParser.o YUMFileListParser.o YUMOtherParser.o LibXMLHelper.o
			gcc -ggdb -Wall -I/usr/include/libxml2 -l stdc++ -l xml2 -l y2util YUMtest.o YUMParserData.o YUMRepomdParser.o YUMPrimaryParser.o YUMGroupParser.o YUMFileListParser.o YUMOtherParser.o LibXMLHelper.o -o YUMtest


YUMtest.o:	YUMtest.cc YUMParserData.h YUMRepomdParser.h YUMPrimaryParser.h YUMGroupParser.h YUMFileListParser.h YUMOtherParser.h

LibXMLHelper.o:		LibXMLHelper.cc LibXMLHelper.h

YUMParserData.o:	YUMParserData.cc YUMParserData.h

YUMRepomdParser.o:	YUMRepomdParser.cc YUMRepomdParser.h YUMParserData.h LibXMLHelper.h YUMParserData.h

YUMPrimaryParser.o:	YUMPrimaryParser.cc YUMPrimaryParser.h YUMParserData.h  LibXMLHelper.h YUMParserData.h

YUMGroupParser.o:	YUMGroupParser.cc YUMGroupParser.h YUMParserData.h  LibXMLHelper.h YUMParserData.h

YUMFileListParser.o:	YUMFileListParser.cc YUMFileListParser.h YUMParserData.h LibXMLHelper.h YUMParserData.h

YUMOtherParser.o:	YUMOtherParser.cc YUMOtherParser.h YUMParserData.h LibXMLHelper.h YUMParserData.h



CXXFLAGS = -ggdb -Wall -I/usr/include/YaST2 -l stdc++ -l xml2

# All:	YUMRepomdDataIterator.o YUMRepomdData.o LibXMLHelper.o test-YUMRepomdData.o

All: YUMtest

YUMtest:	YUMtest.o YUMParserData.o YUMRepomdParser.o YUMPrimaryParser.o YUMGroupParser.o YUMFileListParser.o YUMOtherParser.o LibXMLHelper.o
			gcc -ggdb -Wall -I/usr/include/libxml2 -l stdc++ -l xml2 -l y2util YUMtest.o YUMParserData.o YUMRepomdParser.o YUMPrimaryParser.o YUMGroupParser.o YUMFileListParser.o YUMOtherParser.o LibXMLHelper.o -o YUMtest


YUMtest.o:	YUMtest.cc ../../y2pm/YUMParserData.h ../../y2pm/YUMRepomdParser.h ../../y2pm/YUMPrimaryParser.h ../../y2pm/YUMGroupParser.h ../../y2pm/YUMFileListParser.h ../../y2pm/YUMOtherParser.h

LibXMLHelper.o:		LibXMLHelper.cc ../../y2pm/LibXMLHelper.h

YUMParserData.o:	YUMParserData.cc ../../y2pm/YUMParserData.h

YUMRepomdParser.o:	YUMRepomdParser.cc ../../y2pm/YUMRepomdParser.h ../../y2pm/YUMParserData.h ../../y2pm/LibXMLHelper.h ../../y2pm/YUMParserData.h

YUMPrimaryParser.o:	YUMPrimaryParser.cc ../../y2pm/YUMPrimaryParser.h ../../y2pm/YUMParserData.h  ../../y2pm/LibXMLHelper.h ../../y2pm/YUMParserData.h

YUMGroupParser.o:	YUMGroupParser.cc ../../y2pm/YUMGroupParser.h ../../y2pm/YUMParserData.h  ../../y2pm/LibXMLHelper.h ../../y2pm/YUMParserData.h

YUMFileListParser.o:	YUMFileListParser.cc ../../y2pm/YUMFileListParser.h ../../y2pm/YUMParserData.h ../../y2pm/LibXMLHelper.h ../../y2pm/YUMParserData.h

YUMOtherParser.o:	YUMOtherParser.cc ../../y2pm/YUMOtherParser.h ../../y2pm/YUMParserData.h ../../y2pm/LibXMLHelper.h ../../y2pm/YUMParserData.h


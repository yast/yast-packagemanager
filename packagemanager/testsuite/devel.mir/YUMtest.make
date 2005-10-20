
CXXFLAGS = -ggdb -Wall -I/usr/include/YaST2 -I/usr/include/libxml2 -l stdc++ -l xml2
YUMsrc = ../../src/inst/srcYUM
incl = ../../src/y2pm

# All:	YUMRepomdDataIterator.o YUMRepomdData.o LibXMLHelper.o test-YUMRepomdData.o

All: YUMtest

YUMtest:	YUMtest.o $(YUMsrc)/YUMParserData.o $(YUMsrc)/YUMRepomdParser.o $(YUMsrc)/YUMPrimaryParser.o $(YUMsrc)/YUMGroupParser.o $(YUMsrc)/YUMFileListParser.o $(YUMsrc)/YUMOtherParser.o $(YUMsrc)/LibXMLHelper.o $(YUMsrc)/XMLNodeIterator.o
			gcc -ggdb -Wall -I/usr/include/libxml2 -l stdc++ -l xml2 -l y2util YUMtest.o $(YUMsrc)/YUMParserData.o $(YUMsrc)/YUMRepomdParser.o $(YUMsrc)/YUMPrimaryParser.o $(YUMsrc)/YUMGroupParser.o $(YUMsrc)/YUMFileListParser.o $(YUMsrc)/YUMOtherParser.o $(YUMsrc)/LibXMLHelper.o $(YUMsrc)/XMLNodeIterator.o -o YUMtest


YUMtest.o:	YUMtest.cc $(incl)/YUMParserData.h $(incl)/YUMRepomdParser.h $(incl)/YUMPrimaryParser.h $(incl)/YUMGroupParser.h $(incl)/YUMFileListParser.h $(incl)/YUMOtherParser.h

$(YUMsrc)/LibXMLHelper.o:		$(YUMsrc)/LibXMLHelper.cc $(incl)/LibXMLHelper.h

$(YUMsrc)/YUMParserData.o:	$(YUMsrc)/YUMParserData.cc $(incl)/YUMParserData.h

$(YUMsrc)/YUMRepomdParser.o:	$(YUMsrc)/YUMRepomdParser.cc $(incl)/YUMRepomdParser.h $(incl)/YUMParserData.h $(incl)/LibXMLHelper.h $(incl)/YUMParserData.h

$(YUMsrc)/YUMPrimaryParser.o:	$(YUMsrc)/YUMPrimaryParser.cc $(incl)/YUMPrimaryParser.h $(incl)/YUMParserData.h  $(incl)/LibXMLHelper.h $(incl)/YUMParserData.h

$(YUMsrc)/YUMGroupParser.o:	$(YUMsrc)/YUMGroupParser.cc $(incl)/YUMGroupParser.h $(incl)/YUMParserData.h  $(incl)/LibXMLHelper.h $(incl)/YUMParserData.h

$(YUMsrc)/YUMFileListParser.o:	$(YUMsrc)/YUMFileListParser.cc $(incl)/YUMFileListParser.h $(incl)/YUMParserData.h $(incl)/LibXMLHelper.h $(incl)/YUMParserData.h

$(YUMsrc)/YUMOtherParser.o:	$(YUMsrc)/YUMOtherParser.cc $(incl)/YUMOtherParser.h $(incl)/YUMParserData.h $(incl)/LibXMLHelper.h $(incl)/YUMParserData.h

$(YUMsrc)/XMLNodeIterator.cc:	$(YUMsrc)/XMLNodeIterator.cc $(incl)/XMLNodeIterator.h 

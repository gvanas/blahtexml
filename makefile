# blahtex: a TeX to MathML converter designed with MediaWiki in mind
# blahtexml: an extension of blahtex with XML processing in mind
# http://gva.noekeon.org/blahtexml
#
# Copyright (c) 2006, David Harvey
# Copyright (c) 2009, Gilles Van Assche
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
#     * Neither the names of the authors nor the names of their affiliation may be used to endorse or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


SOURCES = \
	source/main.cpp \
	source/mainPng.cpp \
	source/md5.c \
	source/md5Wrapper.cpp \
	source/Messages.cpp \
	source/UnicodeConverter.cpp \
	source/BlahtexCore/InputSymbolTranslation.cpp \
	source/BlahtexCore/Interface.cpp \
	source/BlahtexCore/LayoutTree.cpp \
	source/BlahtexCore/MacroProcessor.cpp \
	source/BlahtexCore/Manager.cpp \
	source/BlahtexCore/Parser.cpp \
	source/BlahtexCore/ParseTree1.cpp \
	source/BlahtexCore/ParseTree2.cpp \
	source/BlahtexCore/ParseTree3.cpp \
	source/BlahtexCore/MathmlNode.cpp \
	source/BlahtexCore/XmlEncode.cpp

SOURCES_XMLIN = $(SOURCES) \
	source/BlahtexXMLin/AttributesImpl.cpp \
	source/BlahtexXMLin/BlahtexFilter.cpp \
	source/BlahtexXMLin/SAX2Output.cpp \
	source/BlahtexXMLin/XercesString.cpp
	
HEADERS = \
	source/mainPng.h \
	source/md5.h \
	source/md5Wrapper.h \
	source/UnicodeConverter.h \
	source/BlahtexCore/InputSymbolTranslation.h \
	source/BlahtexCore/Interface.h \
	source/BlahtexCore/LayoutTree.h \
	source/BlahtexCore/MacroProcessor.h \
	source/BlahtexCore/Manager.h \
	source/BlahtexCore/Misc.h \
	source/BlahtexCore/Parser.h \
	source/BlahtexCore/ParseTree.h \
	source/BlahtexCore/MathmlNode.h \
	source/BlahtexCore/XmlEncode.h

HEADERS_XMLIN = $(HEADERS) \
	source/BlahtexXMLin/AttributesImpl.h \
	source/BlahtexXMLin/BlahtexFilter.h \
	source/BlahtexXMLin/SAX2Output.h \
	source/BlahtexXMLin/XercesString.h

BINDIR = bin-blahtex

$(BINDIR):
	mkdir -p $(BINDIR)

BINDIR_XMLIN = bin-blahtexml

$(BINDIR_XMLIN):
	mkdir -p $(BINDIR_XMLIN)

OBJECTS = $(addprefix $(BINDIR)/, $(notdir $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))))

OBJECTS_XMLIN = $(addprefix $(BINDIR_XMLIN)/, $(notdir $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES_XMLIN)))))

source/BlahtexCore/InputSymbolTranslation.inc: source/BlahtexCore/InputSymbolTranslation.xml
	xsltproc -o $@ source/BlahtexCore/ISTtoCpp.xslt $<

source/BlahtexCore/InputSymbolTranslation.cpp: source/BlahtexCore/InputSymbolTranslation.inc

$(BINDIR)/InputSymbolTranslation.o: InputSymbolTranslation.cpp InputSymbolTranslation.inc

$(BINDIR_XMLIN)/InputSymbolTranslation.o: InputSymbolTranslation.cpp InputSymbolTranslation.inc

#default targets are still blahtex (not blahtexml)
linux: blahtex-linux
mac: blahtex-mac

CFLAGS = -O2

VPATH = source:source/BlahtexCore:source/BlahtexXMLin

INCLUDES=-I. -Isource -Isource/BlahtexCore -Isource/BlahtexXMLin

$(BINDIR)/%.o:%.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(BINDIR)/%.o:%.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(BINDIR_XMLIN)/%.o:%.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) -DBLAHTEXML_USING_XERCES -c $< -o $@

$(BINDIR_XMLIN)/%.o:%.c
	$(CC) $(INCLUDES) $(CFLAGS) -DBLAHTEXML_USING_XERCES -c $< -o $@

blahtex-linux:  $(BINDIR) $(OBJECTS)  $(HEADERS)
	$(CXX) $(CFLAGS) -o blahtex $(OBJECTS)

blahtex-mac: $(BINDIR) $(OBJECTS)  $(HEADERS)
	$(CXX) $(CFLAGS) -o blahtex -liconv $(OBJECTS)

blahtexml-linux:  $(BINDIR_XMLIN) $(OBJECTS_XMLIN)  $(HEADERS_XMLIN)
	$(CXX) $(CFLAGS) -o blahtexml $(OBJECTS_XMLIN) -lxerces-c

blahtexml-mac: $(BINDIR_XMLIN) $(OBJECTS_XMLIN)  $(HEADERS_XMLIN)
	$(CXX) $(CFLAGS) -o blahtexml -liconv $(OBJECTS_XMLIN) -lxerces-c

clean:
	rm -f blahtex $(OBJECTS) blahtexml $(OBJECTS_XMLIN)

# Documentation

doc: manual.pdf

manual.pdf: manual.tex InputSymbolTranslation.tex
	pdflatex manual
	pdflatex manual

InputSymbolTranslation.tex: source/BlahtexCore/InputSymbolTranslation.xml
	xsltproc -o $@ ISTtoTeX.xslt $<

########## end of file ##########

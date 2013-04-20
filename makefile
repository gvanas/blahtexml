# blahtex: a TeX to MathML converter designed with MediaWiki in mind
# blahtexml: an extension of blahtex with XML processing in mind
# http://gva.noekeon.org/blahtexml
#
# Copyright (c) 2006, David Harvey
# Copyright (c) 2010, Gilles Van Assche
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
#     * Neither the names of the authors nor the names of their affiliation may be used to endorse or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

.PHONY: help

help:
	@echo "Type 'make doc' to make the documentation"
	@echo "Type 'make blahtex-linux' to make blahtex for Linux"
	@echo "Type 'make blahtexml-linux' to make blahtexml for Linux"
	@echo "Type 'make blahtex-mac' to make blahtex for Mac"
	@echo "Type 'make blahtexml-mac' to make blahtexml for Mac"

SOURCES = \
	Source/main.cpp \
	Source/mainPng.cpp \
	Source/md5.c \
	Source/md5Wrapper.cpp \
	Source/Messages.cpp \
	Source/UnicodeConverter.cpp \
	Source/BlahtexCore/InputSymbolTranslation.cpp \
	Source/BlahtexCore/Interface.cpp \
	Source/BlahtexCore/LayoutTree.cpp \
	Source/BlahtexCore/MacroProcessor.cpp \
	Source/BlahtexCore/Manager.cpp \
	Source/BlahtexCore/Parser.cpp \
	Source/BlahtexCore/ParseTree1.cpp \
	Source/BlahtexCore/ParseTree2.cpp \
	Source/BlahtexCore/ParseTree3.cpp \
	Source/BlahtexCore/MathmlNode.cpp \
	Source/BlahtexCore/Token.cpp \
	Source/BlahtexCore/XmlEncode.cpp

SOURCES_XMLIN = $(SOURCES) \
	Source/BlahtexXMLin/AttributesImpl.cpp \
	Source/BlahtexXMLin/BlahtexFilter.cpp \
	Source/BlahtexXMLin/SAX2Output.cpp \
	Source/BlahtexXMLin/XercesString.cpp
	
HEADERS = \
	Source/mainPng.h \
	Source/md5.h \
	Source/md5Wrapper.h \
	Source/UnicodeConverter.h \
	Source/BlahtexCore/InputSymbolTranslation.h \
	Source/BlahtexCore/Interface.h \
	Source/BlahtexCore/LayoutTree.h \
	Source/BlahtexCore/MacroProcessor.h \
	Source/BlahtexCore/Manager.h \
	Source/BlahtexCore/Misc.h \
	Source/BlahtexCore/Parser.h \
	Source/BlahtexCore/ParseTree.h \
	Source/BlahtexCore/MathmlNode.h \
	Source/BlahtexCore/Token.h \
	Source/BlahtexCore/XmlEncode.h

HEADERS_XMLIN = $(HEADERS) \
	Source/BlahtexXMLin/AttributesImpl.h \
	Source/BlahtexXMLin/BlahtexFilter.h \
	Source/BlahtexXMLin/SAX2Output.h \
	Source/BlahtexXMLin/XercesString.h

BINDIR = bin-blahtex

$(BINDIR):
	mkdir -p $(BINDIR)

BINDIR_XMLIN = bin-blahtexml

$(BINDIR_XMLIN):
	mkdir -p $(BINDIR_XMLIN)

OBJECTS = $(addprefix $(BINDIR)/, $(notdir $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))))

OBJECTS_XMLIN = $(addprefix $(BINDIR_XMLIN)/, $(notdir $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES_XMLIN)))))

Source/BlahtexCore/InputSymbolTranslation.inc: Source/BlahtexCore/InputSymbolTranslation.xml
	xsltproc -o $@ Source/BlahtexCore/ISTtoCpp.xslt $<

Source/BlahtexCore/InputSymbolTranslation.cpp: Source/BlahtexCore/InputSymbolTranslation.inc

$(BINDIR)/InputSymbolTranslation.o: InputSymbolTranslation.cpp InputSymbolTranslation.inc

$(BINDIR_XMLIN)/InputSymbolTranslation.o: InputSymbolTranslation.cpp InputSymbolTranslation.inc

CFLAGS = -O2

VPATH = Source:Source/BlahtexCore:Source/BlahtexXMLin

INCLUDES=-I. -ISource -ISource/BlahtexCore -ISource/BlahtexXMLin

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

doc: Documentation/manual.pdf

Documentation/manual.pdf: Documentation/manual.tex Documentation/InputSymbolTranslation.tex
	cd Documentation ; pdflatex manual
	cd Documentation ; pdflatex manual

Documentation/InputSymbolTranslation.tex: Source/BlahtexCore/InputSymbolTranslation.xml
	xsltproc -o $@ Documentation/ISTtoTeX.xslt $<

########## end of file ##########

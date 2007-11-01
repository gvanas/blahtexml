#
# makefile
#
# blahtex (version 0.4.4)
# a TeX to MathML converter designed with MediaWiki in mind
# Copyright (C) 2006, David Harvey
#
# blahtexml = XML input for blahtex (version 0.4.4)
# Copyright (C) 2007, Gilles Van Assche
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#


SOURCES = \
	source/main.cpp \
	source/mainPng.cpp \
	source/md5.c \
	source/md5Wrapper.cpp \
	source/Messages.cpp \
	source/UnicodeConverter.cpp \
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

#default targets are still blahtex (not blahtexml)
linux: blahtex-linux
mac: blahtex-mac

blahtex-linux : CFLAGS = -O3
blahtex-mac : CFLAGS = -O3 -DBLAHTEX_ICONV_CONST
blahtexml-linux : CFLAGS = -O3
blahtexml-mac : CFLAGS = -O3 -DBLAHTEX_ICONV_CONST

VPATH = source:source/BlahtexCore:source/BlahtexXMLin

INCLUDES=-I. -Isource -Isource/BlahtexCore -Isource/BlahtexXMLin

$(BINDIR)/%.o:%.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(BINDIR)/%.o:%.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(BINDIR_XMLIN)/%.o:%.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) -DBLAHTEXML_USING_XERCES -O3 -c $< -o $@

$(BINDIR_XMLIN)/%.o:%.c
	$(CC) $(INCLUDES) $(CFLAGS) -DBLAHTEXML_USING_XERCES -O3 -c $< -o $@

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

########## end of file ##########

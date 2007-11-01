// File "SAX2Output.h"
//
// blahtexml = XML input for blahtex (version 0.4.4)
// Copyright (C) 2007, Gilles Van Assche
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include <list>
#include <utility>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include "XercesString.h"

XERCES_CPP_NAMESPACE_USE

class SAX2Output : public DefaultHandler, private XMLFormatTarget
{
public:
    typedef enum { DoctypeNone=0, DoctypeSystem, DoctypePublic } Doctype;
protected:
    ostream& out;
    std::list<std::pair<XercesString, XercesString> > prefixMappings;
    XMLFormatter format;
    Doctype doctype;
    XercesString publicID, DTD;
    bool rootElementEncountered;
public:
    SAX2Output(ostream& aOut, const char* const encodingName, const XMLFormatter::UnRepFlags unRepFlags);
    SAX2Output(ostream& aOut, const char* const encodingName, const XMLFormatter::UnRepFlags unRepFlags,
        Doctype aDoctype, const XercesString& aPublicID, const XercesString& aDTD);
    ~SAX2Output();
    virtual void characters(const XMLCh* const chars, const unsigned int length);
    virtual void endElement( const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);
    virtual void error(const SAXParseException& e);
    virtual void fatalError(const SAXParseException& e);
    virtual void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);
    virtual void processingInstruction(const XMLCh* const target, const XMLCh* const data);
    virtual void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const Attributes& attributes);
    virtual void startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri);
    virtual void warning(const SAXParseException& e);
    virtual void writeChars(const XMLByte* const toWrite, const unsigned int count, XMLFormatter* const formatter);
private:
    void xmlDeclaration(const char* const encodingName);
    void doctypeDeclaration(const XMLCh* const qname);
};

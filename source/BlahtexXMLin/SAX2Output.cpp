// File "SAX2Output.cpp"
//
// blahtexml (version 0.6)
// Copyright (C) 2007-2008, Gilles Van Assche
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

#include <iostream>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include "SAX2Output.h"
#include "XercesString.h"

XERCES_CPP_NAMESPACE_USE
using namespace std;

SAX2Output::SAX2Output(ostream& aOut, const char* const encodingName,
    const XMLFormatter::UnRepFlags unRepFlags)
        : out(aOut), format(encodingName, 0, this, XMLFormatter::NoEscapes, unRepFlags),
            doctype(DoctypeNone), publicID(), DTD(), rootElementEncountered(false)
{
    xmlDeclaration(encodingName);
}

SAX2Output::SAX2Output(ostream& aOut, const char* const encodingName,
    const XMLFormatter::UnRepFlags unRepFlags,
    Doctype aDoctype, const XercesString& aPublicID, const XercesString& aDTD)
        : out(aOut), format(encodingName, 0, this, XMLFormatter::NoEscapes, unRepFlags),
            doctype(aDoctype), publicID(aPublicID), DTD(aDTD), rootElementEncountered(false)
{
    xmlDeclaration(encodingName);
}

SAX2Output::~SAX2Output()
{
}

void SAX2Output::writeChars(const XMLByte* const toWrite,
    const unsigned int count, XMLFormatter* const formatter)
{
    out.write((const char*)toWrite, count);
    out << std::flush;
}

void SAX2Output::error(const SAXParseException& e)
{
    cerr << endl
        << "!!! Parsing error at " << XercesString(e.getSystemId()) << ":" << e.getLineNumber()
        << ":" << e.getColumnNumber() << " :  " << XercesString(e.getMessage()) << endl;
}

void SAX2Output::fatalError(const SAXParseException& e)
{
    cerr << endl
        << "!!! Fatal parsing error at " << XercesString(e.getSystemId()) << ":" << e.getLineNumber()
        << ":" << e.getColumnNumber() << " :  " << XercesString(e.getMessage()) << endl;
}

void SAX2Output::warning(const SAXParseException& e)
{
    cerr << endl
        << "!!! Parsing warning at " << XercesString(e.getSystemId()) << ":" << e.getLineNumber()
        << ":" << e.getColumnNumber() << " :  " << XercesString(e.getMessage()) << endl;
}

void SAX2Output::characters(const XMLCh* const chars,
    const unsigned int length)
{
    format.formatBuf(chars, length, XMLFormatter::CharEscapes);
}

void SAX2Output::endElement(const XMLCh* const uri,
    const XMLCh* const localname, const XMLCh* const qname)
{
    format << XMLFormatter::NoEscapes << chOpenAngle << chForwardSlash << qname << chCloseAngle;
}

void SAX2Output::ignorableWhitespace(const XMLCh* const chars,
    const unsigned int length)
{
    format.formatBuf(chars, length, XMLFormatter::NoEscapes);
}

void SAX2Output::processingInstruction(const XMLCh* const target,
    const XMLCh* const data)
{
    format << XMLFormatter::NoEscapes << chOpenAngle << chQuestion << target;
    if (data) format << chSpace << data;
    format << XMLFormatter::NoEscapes << chQuestion << chCloseAngle;
}

void SAX2Output::startElement(const XMLCh* const uri, const XMLCh* const localname,
    const XMLCh* const qname, const Attributes& attributes)
{
    if(!rootElementEncountered) {
        doctypeDeclaration(qname);
        rootElementEncountered = true;
    }
    format  << XMLFormatter::NoEscapes << chOpenAngle ;
    format << qname ;

    XercesString xmlns("xmlns");
    for(list<pair<XercesString, XercesString> >::const_iterator i=prefixMappings.begin(); i!=prefixMappings.end(); ++i) {
        format << XMLFormatter::NoEscapes << chSpace;
        if ((*i).first.size() > 0)
            format << xmlns.c_str() << chColon << (*i).first.c_str();
        else
            format << xmlns.c_str();
        format << chEqual << chDoubleQuote << XMLFormatter::AttrEscapes << (*i).second.c_str()
            << XMLFormatter::NoEscapes << chDoubleQuote;
    }
    prefixMappings.clear();

    unsigned int len = attributes.getLength();
    for (unsigned int index=0; index<len; ++index) {
        format  << XMLFormatter::NoEscapes << chSpace << attributes.getQName(index)
            << chEqual << chDoubleQuote
            << XMLFormatter::AttrEscapes << attributes.getValue(index)
            << XMLFormatter::NoEscapes << chDoubleQuote;
    }
    format << chCloseAngle;
}

void SAX2Output::startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri)
{
    prefixMappings.push_back(pair<XercesString, XercesString>(XercesString(prefix), XercesString(uri)));
}

void SAX2Output::xmlDeclaration(const char* const encodingName)
{
    static XercesString xmlDeclaration1("<?xml version=\"1.0\" encoding=\"");
    XercesString xmlDeclaration2(encodingName);
    static XercesString xmlDeclaration3("\"?>");
    format << xmlDeclaration1.c_str() << xmlDeclaration2.c_str() << xmlDeclaration3.c_str() << chLF;
}

void SAX2Output::doctypeDeclaration(const XMLCh* const qname)
{
    static XercesString doctypeDeclaration("<!DOCTYPE");
    static XercesString systemDeclaration("SYSTEM");
    static XercesString publicDeclaration("PUBLIC");
    if((doctype == DoctypeSystem) || (doctype == DoctypePublic)) {
        format << XMLFormatter::NoEscapes << doctypeDeclaration.c_str() << chSpace
            << qname << chSpace;
        if (doctype == DoctypeSystem)
            format << XMLFormatter::NoEscapes << systemDeclaration.c_str() << chSpace;
        else {
            format << XMLFormatter::NoEscapes << publicDeclaration.c_str() << chSpace;
            format << XMLFormatter::NoEscapes << chDoubleQuote << publicID.c_str() << chDoubleQuote << chSpace;
        }
        format << XMLFormatter::NoEscapes << chDoubleQuote << DTD.c_str() << chDoubleQuote << chCloseAngle << chLF;
    }
}

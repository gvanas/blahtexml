/*
blahtex: a TeX to MathML converter designed with MediaWiki in mind
blahtexml: an extension of blahtex with XML processing in mind
http://gva.noekeon.org/blahtexml

Copyright (c) 2010, Gilles Van Assche
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the names of the authors nor the names of their affiliation may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
#if _XERCES_VERSION >= 30000
    virtual void characters(const XMLCh* const chars, const XMLSize_t length);
#else
    virtual void characters(const XMLCh* const chars, const unsigned int length);
#endif
    virtual void endElement( const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);
    virtual void error(const SAXParseException& e);
    virtual void fatalError(const SAXParseException& e);
#if _XERCES_VERSION >= 30000
    virtual void ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length);
#else
    virtual void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);
#endif
    virtual void processingInstruction(const XMLCh* const target, const XMLCh* const data);
    virtual void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const Attributes& attributes);
    virtual void startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri);
    virtual void warning(const SAXParseException& e);
#if _XERCES_VERSION >= 30000
    virtual void writeChars(const XMLByte* const toWrite, const XMLSize_t count, XMLFormatter* const formatter);
#else
    virtual void writeChars(const XMLByte* const toWrite, const unsigned int count, XMLFormatter* const formatter);
#endif
private:
    void xmlDeclaration(const char* const encodingName);
    void doctypeDeclaration(const XMLCh* const qname);
};

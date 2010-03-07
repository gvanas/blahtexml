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

#ifndef ATTRIBUTESIMPL_H
#define ATTRIBUTESIMPL_H

#include <string>
#include <vector>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/XMLString.hpp>
#include "XercesString.h"

XERCES_CPP_NAMESPACE_USE
using namespace std;

struct Attribute {
    XercesString qName;
    XercesString uri;
    XercesString localPart;
    XercesString value;
    XercesString type;
};

class AttributesImpl : public Attributes
{
private:
    vector<Attribute> theAttributes;
public:
    AttributesImpl();
    AttributesImpl(const Attributes& attributes);
#if _XERCES_VERSION >= 30000
    virtual XMLSize_t getLength() const;
    virtual const XMLCh* getURI(const XMLSize_t index) const;
    virtual const XMLCh* getLocalName(const XMLSize_t index) const;
    virtual const XMLCh* getQName(const XMLSize_t index) const;
    virtual const XMLCh* getType(const XMLSize_t index) const;
    virtual const XMLCh* getValue(const XMLSize_t index) const;
    virtual bool getIndex(const XMLCh* const uri, const XMLCh* const localPart, XMLSize_t& index) const;
    virtual bool getIndex(const XMLCh* const qName, XMLSize_t& index) const;
#else
    virtual unsigned int getLength() const;
    virtual const XMLCh* getURI(const unsigned int index) const;
    virtual const XMLCh* getLocalName(const unsigned int index) const;
    virtual const XMLCh* getQName(const unsigned int index) const;
    virtual const XMLCh* getType(const unsigned int index) const;
    virtual const XMLCh* getValue(const unsigned int index) const;
#endif
    virtual int getIndex(const XMLCh* const uri, const XMLCh* const localPart ) const;
    virtual int getIndex(const XMLCh* const qName ) const;
    virtual const XMLCh* getType(const XMLCh* const uri, const XMLCh* const localPart ) const;
    virtual const XMLCh* getType(const XMLCh* const qName) const;
    virtual const XMLCh* getValue(const XMLCh* const uri, const XMLCh* const localPart ) const;
    virtual const XMLCh* getValue(const XMLCh* const qName) const;
    void removeAttribute(const unsigned int index);
    void addAttribute(const XercesString& qName, const XercesString& uri, const XercesString& localPart, const XercesString& value, const XercesString& type);
};

#endif

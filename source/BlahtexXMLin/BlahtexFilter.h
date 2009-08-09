/*
blahtex: a TeX to MathML converter designed with MediaWiki in mind
blahtexml: an extension of blahtex with XML processing in mind
http://gva.noekeon.org/blahtexml

Copyright (c) 2009, Gilles Van Assche
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the names of the authors nor the names of their affiliation may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <list>
#include <string>
#include <utility>
#include <xercesc/parsers/SAX2XMLFilterImpl.hpp>
#include "../BlahtexCore/Interface.h"

XERCES_CPP_NAMESPACE_USE

typedef list<pair<wstring, wstring> > list_wstring2;

class BlahtexFilter : public SAX2XMLFilterImpl
{
public:
    typedef enum { PrefixAuto, PrefixNone, PrefixAdd } PrefixType;
protected:
    blahtex::Interface& interface;
    list_wstring2 namespaceContext;
    int numberOfErrors;
    PrefixType desiredMathMLPrefixType;
    wstring desiredMathMLPrefix;
public:
    BlahtexFilter(SAX2XMLReader* parent, blahtex::Interface& anInterface);
    ~BlahtexFilter();
    virtual void startElement(const XMLCh* const uri, const XMLCh* const localname,
        const XMLCh* const qname, const Attributes& attributes);
    virtual void startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri);
    virtual void endPrefixMapping(const XMLCh* const prefix);
    int getNumberOfErrors();
    void setDesiredMathMLPrefixType(PrefixType aPrefixType, const wstring& aPrefix);
protected:
    bool getMathMLprefix(wstring& prefix);
};

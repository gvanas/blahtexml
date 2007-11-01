// File "BlahtexFilter.cpp"
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

#include "AttributesImpl.h"
#include "BlahtexFilter.h"
#include "XercesString.h"
#include <iostream>
#include <xercesc/framework/StdInInputSource.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/parsers/SAX2XMLFilterImpl.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

using namespace std;

extern wstring GetErrorMessage(const blahtex::Exception& e);

BlahtexFilter::BlahtexFilter(SAX2XMLReader* parent, blahtex::Interface& anInterface)
    : SAX2XMLFilterImpl(parent), interface(anInterface), numberOfErrors(0)
{
}

BlahtexFilter::~BlahtexFilter()
{
}

void BlahtexFilter::startElement(const XMLCh* const uri, const XMLCh* const localname,
    const XMLCh* const qname, const Attributes& attributes)
{
    static AttributesImpl emptyAttributes;
    static XercesString empty;
    static XercesString blahtexmlNamespace("http://gva.noekeon.org/blahtexml");
    static XercesString m("m");
    static XercesString inlin("inline");
    static XercesString block("block");
    static XercesString error("error");
    static XercesString display("display");

    bool blockMode = false;
    bool encloseInMathTag = false;
    int eqAttrIndex = attributes.getIndex(blahtexmlNamespace.c_str(), m.c_str());
    if (eqAttrIndex < 0) {
        eqAttrIndex = attributes.getIndex(blahtexmlNamespace.c_str(), inlin.c_str());
        encloseInMathTag = (eqAttrIndex >= 0);
    }
    if (eqAttrIndex < 0) {
        eqAttrIndex = attributes.getIndex(blahtexmlNamespace.c_str(), block.c_str());
        encloseInMathTag = (eqAttrIndex >= 0);
        blockMode = (eqAttrIndex >= 0);
    }
    if (eqAttrIndex >= 0) {
        try {
            XercesString inputXerces(attributes.getValue(eqAttrIndex));
            wstring input = inputXerces.convertTowstring();
            interface.ProcessInput(input);

            AttributesImpl newAttributes(attributes);
            newAttributes.removeAttribute(eqAttrIndex);
            SAX2XMLFilterImpl::startElement(uri, localname, qname, newAttributes);

            XercesString MathMLnamespace("http://www.w3.org/1998/Math/MathML");
            XercesString unprefixedMath(L"math");
            wstring MathMLprefix;
            bool MathMLexistingNamespace = getMathMLprefix(MathMLprefix);
            if ((desiredMathMLPrefixType == PrefixNone) && (!(MathMLprefix == L""))) {
                MathMLexistingNamespace = false;
                MathMLprefix = L"";
            }
            if ((desiredMathMLPrefixType == PrefixAdd) && (!(MathMLprefix == desiredMathMLPrefix))) {
                MathMLexistingNamespace = false;
                MathMLprefix = desiredMathMLPrefix;
            }
            XercesString MathMLprefix_(MathMLprefix);

            if (!MathMLexistingNamespace)
                SAX2XMLFilterImpl::startPrefixMapping(MathMLprefix_.c_str(), MathMLnamespace.c_str());
            XercesString prefixedMath((MathMLprefix == L"") ? L"math" : (MathMLprefix + L":math"));
            if (encloseInMathTag) {
                AttributesImpl mathAttributes;
                if (blockMode)
                    mathAttributes.addAttribute(display.c_str(), empty.c_str(), display.c_str(), block.c_str(), empty.c_str());
                SAX2XMLFilterImpl::startElement(MathMLnamespace.c_str(), unprefixedMath.c_str(), prefixedMath.c_str(), mathAttributes);
            }
            interface.PrintAsSAX2(*this, MathMLprefix, true);
            if (encloseInMathTag)
                SAX2XMLFilterImpl::endElement(MathMLnamespace.c_str(), unprefixedMath.c_str(), prefixedMath.c_str());
            if (!MathMLexistingNamespace)
                SAX2XMLFilterImpl::endPrefixMapping(MathMLprefix_.c_str());
        }
        catch (blahtex::Exception& e) {
            wstring output = GetErrorMessage(e);
            XercesString outputXerces(output);
            SAX2XMLFilterImpl::startElement(uri, localname, qname, attributes);
            SAX2XMLFilterImpl::startPrefixMapping(empty.c_str(), blahtexmlNamespace.c_str());
            SAX2XMLFilterImpl::startElement(blahtexmlNamespace.c_str(), error.c_str(), error.c_str(), emptyAttributes);
            SAX2XMLFilterImpl::characters(outputXerces.data(), outputXerces.length());
            SAX2XMLFilterImpl::endElement(blahtexmlNamespace.c_str(), error.c_str(), error.c_str());
            SAX2XMLFilterImpl::endPrefixMapping(empty.c_str());
            numberOfErrors++;
        }
	}
    else {
        SAX2XMLFilterImpl::startElement(uri, localname, qname, attributes);
    }
}

void BlahtexFilter::startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri)
{
    XercesString prefixXerces(prefix), uriXerces(uri);
    pair<wstring, wstring> context(prefixXerces.convertTowstring(), uriXerces.convertTowstring());
    namespaceContext.push_front(context);
    SAX2XMLFilterImpl::startPrefixMapping(prefix, uri);
}

void BlahtexFilter::endPrefixMapping(const XMLCh* const prefix)
{
    XercesString prefixXerces(prefix);
    wstring prefixwstring=prefixXerces.convertTowstring();
    for(list_wstring2::iterator i=namespaceContext.begin(); i != namespaceContext.end(); ++i) {
        if ((*i).first == prefixwstring) {
            namespaceContext.erase(i);
            break;
        }
    }
    SAX2XMLFilterImpl::endPrefixMapping(prefix);
}

bool BlahtexFilter::getMathMLprefix(wstring& prefix)
{
    for(list_wstring2::iterator i=namespaceContext.begin(); i != namespaceContext.end(); ++i) {
        if ((*i).second == L"http://www.w3.org/1998/Math/MathML") {
            prefix = (*i).first;
            return true;
        }
    }
    prefix = L"";
    return false;
}

int BlahtexFilter::getNumberOfErrors()
{
    return numberOfErrors;
}

void BlahtexFilter::setDesiredMathMLPrefixType(PrefixType aPrefixType, const wstring& aPrefix)
{
    desiredMathMLPrefixType = aPrefixType;
    desiredMathMLPrefix = aPrefix;
}

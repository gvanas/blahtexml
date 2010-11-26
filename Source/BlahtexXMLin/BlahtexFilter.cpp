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

#include "AttributesImpl.h"
#include "BlahtexFilter.h"
#include "mainPng.h"
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
            interface.ProcessInput(input, blockMode);

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
            if (encloseInMathTag && (annotatePNG || annotateTeX)) {
                static XercesString unprefixedSemantics("semantics");
                XercesString prefixedSemantics((MathMLprefix == L"") ? L"semantics" : (MathMLprefix + L":semantics"));
                static XercesString unprefixedMrow("mrow");
                XercesString prefixedMrow((MathMLprefix == L"") ? L"mrow" : (MathMLprefix + L":mrow"));
                static XercesString unprefixedAnnotation("annotation");
                XercesString prefixedAnnotation((MathMLprefix == L"") ? L"annotation" : (MathMLprefix + L":annotation"));
                static XercesString encoding("encoding");
                SAX2XMLFilterImpl::startElement(MathMLnamespace.c_str(), unprefixedSemantics.c_str(), prefixedSemantics.c_str(), emptyAttributes);
                SAX2XMLFilterImpl::startElement(MathMLnamespace.c_str(), unprefixedMrow.c_str(), prefixedMrow.c_str(), emptyAttributes);
                interface.PrintAsSAX2(*this, MathMLprefix, true);
                SAX2XMLFilterImpl::endElement(MathMLnamespace.c_str(), unprefixedMrow.c_str(), prefixedMrow.c_str());
                if (annotateTeX) {
                    static XercesString TeX("TeX");
                    AttributesImpl annotationAttributes;
                    annotationAttributes.addAttribute(encoding.c_str(), empty.c_str(), encoding.c_str(), TeX.c_str(), empty.c_str());
                    SAX2XMLFilterImpl::startElement(MathMLnamespace.c_str(), unprefixedAnnotation.c_str(), prefixedAnnotation.c_str(), annotationAttributes);
                    XercesString purifiedTex(interface.GetPurifiedTexOnly());
                    SAX2XMLFilterImpl::characters(purifiedTex.data(), purifiedTex.size());
                    SAX2XMLFilterImpl::endElement(MathMLnamespace.c_str(), unprefixedAnnotation.c_str(), prefixedAnnotation.c_str());
                }
                if (annotatePNG) {
                    static XercesString PNG("image-file-PNG");
                    wstring purifiedTex = interface.GetPurifiedTex();
                    PngInfo info = MakePngFile(purifiedTex, "", pngParams);
                    AttributesImpl annotationAttributes;
                    annotationAttributes.addAttribute(encoding.c_str(), empty.c_str(), encoding.c_str(), PNG.c_str(), empty.c_str());
                    SAX2XMLFilterImpl::startElement(MathMLnamespace.c_str(), unprefixedAnnotation.c_str(), prefixedAnnotation.c_str(), annotationAttributes);
                    XercesString fileName(info.fullFileName.c_str());
                    SAX2XMLFilterImpl::characters(fileName.data(), fileName.size());
                    SAX2XMLFilterImpl::endElement(MathMLnamespace.c_str(), unprefixedAnnotation.c_str(), prefixedAnnotation.c_str());
                }
                SAX2XMLFilterImpl::endElement(MathMLnamespace.c_str(), unprefixedSemantics.c_str(), prefixedSemantics.c_str());
            }
            else
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

void BlahtexFilter::setAnnotatePNG(bool anAnnotatePNG)
{
    annotatePNG = anAnnotatePNG;
}

void BlahtexFilter::setAnnotateTeX(bool anAnnotateTeX)
{
    annotateTeX = anAnnotateTeX;
}

void BlahtexFilter::setPngParams(const PngParams& aPngParams)
{
    pngParams = aPngParams;
}

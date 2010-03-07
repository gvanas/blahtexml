/*
blahtex: a TeX to MathML converter designed with MediaWiki in mind
blahtexml: an extension of blahtex with XML processing in mind
http://gva.noekeon.org/blahtexml

Copyright (c) 2006, David Harvey
Copyright (c) 2009, Gilles Van Assche
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the names of the authors nor the names of their affiliation may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include "MathmlNode.h"
#include "XmlEncode.h"

#ifdef BLAHTEXML_USING_XERCES
#include <xercesc/util/XMLString.hpp>
#include "../BlahtexXMLin/AttributesImpl.h"
#include "../BlahtexXMLin/XercesString.h"
#endif

using namespace std;

namespace blahtex
{

// Strings for each MathML "mathvariant" value.
wstring gMathmlFontStrings[] =
{
    L"normal",
    L"bold",
    L"italic",
    L"bold-italic",
    L"double-struck",
    L"bold-fraktur",
    L"script",
    L"bold-script",
    L"fraktur",
    L"sans-serif",
    L"bold-sans-serif",
    L"sans-serif-italic",
    L"sans-serif-bold-italic",
    L"monospace"
};


MathmlNode::~MathmlNode()
{
    for (list<MathmlNode*>::iterator
        p = mChildren.begin(); p != mChildren.end(); p++
    )
        delete *p;
}


void MathmlNode::AddFontAttributes(
    MathmlFont desiredFont,
    const MathmlOptions& options
)
{
    if (options.mUseVersion1FontAttributes)
    {
        // MathML version 1.x fonts requested.
        
        if (
            desiredFont == cMathmlFontDoubleStruck ||
            desiredFont == cMathmlFontBoldFraktur ||
            desiredFont == cMathmlFontScript ||
            desiredFont == cMathmlFontBoldScript ||
            desiredFont == cMathmlFontFraktur
        )
        {
            // The only way we can end up here is with a fraktur <mn>.
            // TeX has decent fraktur digits, but unicode doesn't seem to
            // list them. (FIX: this might be changing in an upcoming
            // revision of unicode. I thought I saw the stix fonts website
            // mention something.) Therefore we can't access them with
            // version 1 font attributes, so let's just map it to bold
            // instead.
            if (mType == cTypeMn &&
                (
                    desiredFont == cMathmlFontFraktur ||
                    desiredFont == cMathmlFontBoldFraktur
                )
            )
                mAttributes[cAttributeFontweight] = L"bold";
            else
                throw logic_error(
                    "Unexpected font/symbol combination "
                    "in MathmlNode::AddFontAttributes"
                );
        }
        else
        {
            
            bool defaultItalic = (mType == cTypeMi && mText.size() == 1);

            bool desiredItalic = (
                desiredFont == cMathmlFontItalic ||
                desiredFont == cMathmlFontBoldItalic ||
                desiredFont == cMathmlFontSansSerifItalic ||
                desiredFont == cMathmlFontSansSerifBoldItalic
            );
            
            if (defaultItalic != desiredItalic)
                mAttributes[cAttributeFontstyle] =
                    desiredItalic ? L"italic" : L"normal";
            
            if (
                desiredFont == cMathmlFontBold ||
                desiredFont == cMathmlFontBoldItalic ||
                desiredFont == cMathmlFontBoldSansSerif ||
                desiredFont == cMathmlFontSansSerifBoldItalic
            )
                mAttributes[cAttributeFontweight] = L"bold";

            if (
                desiredFont == cMathmlFontSansSerif ||
                desiredFont == cMathmlFontBoldSansSerif ||
                desiredFont == cMathmlFontSansSerifItalic ||
                desiredFont == cMathmlFontSansSerifBoldItalic
            )
                mAttributes[cAttributeFontfamily] = L"sans-serif";

            else if (desiredFont == cMathmlFontMonospace)
                mAttributes[cAttributeFontfamily] = L"monospace";
        }
    }
    else
    {
        // MathML version 2.0 fonts requested.
        
        MathmlFont defaultFont =
            (mType == cTypeMi && mText.size() == 1)
            ? cMathmlFontItalic : cMathmlFontNormal;
        
        if (desiredFont != defaultFont)
            mAttributes[cAttributeMathvariant] =
                gMathmlFontStrings[desiredFont];
    }
}


void WriteIndent(
    wostream& os,
    int depth
)
{
    for (int i = 0; i < depth; i++)
        os << L"  ";
}

    static wstring gTypeArray[] =
    {
        L"mi",
        L"mo",
        L"mn",
        L"mspace",
        L"mtext",
        L"mrow",
        L"mstyle",
        L"msub",
        L"msup",
        L"msubsup",
        L"munder",
        L"mover",
        L"munderover",
        L"mfrac",
        L"msqrt",
        L"mroot",
        L"mtable",
        L"mtr",
        L"mtd",
        L"mpadded"
    };
    
void MathmlNode::PrintType(wostream& os) const
{
    if (mType < 0 || mType >= sizeof(gTypeArray))
        throw logic_error("Illegal node type in MathmlNode::PrintType");
    
    os << gTypeArray[mType];
}

    static wstring gAttributeArray[] =
    {
        L"displaystyle",
        L"scriptlevel",
        L"mathvariant",
        L"mathcolor",
        L"lspace",
        L"rspace",
        L"width",
        L"stretchy",
        L"minsize",
        L"maxsize",
        L"accent",
        L"movablelimits",
        L"linethickness",
        L"columnalign",
        L"columnspacing",
        L"rowspacing",
        L"fontfamily",
        L"fontstyle",
        L"fontweight"
    };
    
void MathmlNode::PrintAttributes(wostream& os) const
{
    for (map<Attribute, wstring>::const_iterator
        attribute = mAttributes.begin();
        attribute != mAttributes.end();
        attribute++
    )
    {
        if (
            attribute->first < 0 ||
            attribute->first >= sizeof(gAttributeArray)
        )
            throw logic_error(
                "Illegal attribute in MathmlNode::PrintAttributes"
            );

        os << L" " << gAttributeArray[attribute->first] << L"=\""
            << attribute->second << L"\"";
    }
}

void MathmlNode::Print(
    wostream& os,
    const EncodingOptions& options,
    bool indent,
    int depth
) const
{
    if (indent)
        WriteIndent(os, depth);
    
    os << L"<";
    PrintType(os);
    PrintAttributes(os);
    if (mText.empty() && mChildren.empty())
        os << L"/>";
    else
    {
        if (!mText.empty())
        {
            // is a leaf node with text
            os << L">" << XmlEncode(mText, options);
        }
        else
        {
            // is a internal node with at least one child
            os << L">";
            if (indent)
                os << endl;
            
            for (list<MathmlNode*>::const_iterator
                child = mChildren.begin(); child != mChildren.end(); child++
            )
                (*child)->Print(os, options, indent, depth + 1);

            if (indent)
                WriteIndent(os, depth);
        }

        os << L"</";
        PrintType(os);
        os << L">";
    }
    
    if (indent)
        os << endl;
}

#ifdef BLAHTEXML_USING_XERCES
void MathmlNode::PrintAsSAX2(ContentHandler& sax, const wstring& prefix, bool ignoreFirstmrow) const
{
    if (mType < 0 || mType >= sizeof(gTypeArray))
        throw logic_error("Illegal node type in MathmlNode::PrintType");
    bool skipElement = ignoreFirstmrow && (mType == cTypeMrow) && (mAttributes.size() == 0) && (mText.empty());
    if (skipElement) {
        for (list<MathmlNode*>::const_iterator child = mChildren.begin(); child != mChildren.end(); ++child)
            (*child)->PrintAsSAX2(sax, prefix, false);
    }
    else {
        XercesString elementLocalName(gTypeArray[mType]);
        XercesString elementQName((prefix == L"") ? gTypeArray[mType] : (prefix + L":" + gTypeArray[mType]));
        AttributesImpl attributes;
        for (map<Attribute, wstring>::const_iterator attribute = mAttributes.begin();
                attribute != mAttributes.end(); ++attribute) {
            if (attribute->first < 0 || attribute->first >= sizeof(gAttributeArray))
                throw logic_error("Illegal attribute in MathmlNode::PrintAttributes");
            XercesString localPart(gAttributeArray[attribute->first]);
            XercesString qName = localPart;
            XercesString uri;
            XercesString value(attribute->second);
            XercesString type;
            attributes.addAttribute(qName, uri, localPart, value, type);
        }
        XercesString MathMLnamespace("http://www.w3.org/1998/Math/MathML");
        sax.startElement(MathMLnamespace.c_str(), elementLocalName.c_str(), elementQName.c_str(), attributes);
        if (!mText.empty()) {
            XercesString text(mText);
            sax.characters(text.data(), text.size());
        }
        for (list<MathmlNode*>::const_iterator child = mChildren.begin(); child != mChildren.end(); ++child)
            (*child)->PrintAsSAX2(sax, prefix, false);
        sax.endElement(MathMLnamespace.c_str(), elementLocalName.c_str(), elementQName.c_str());
    }
}
#endif

}

// end of file @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

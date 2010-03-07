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

#ifndef BLAHTEX_MATHMLNODE_H
#define BLAHTEX_MATHMLNODE_H

#include <iostream>
#include <map>
#include <list>
#include <string>
#include "Misc.h"

#ifdef BLAHTEXML_USING_XERCES
#include <xercesc/sax2/ContentHandler.hpp>
XERCES_CPP_NAMESPACE_USE
#endif

namespace blahtex
{

// MathmlFont lists all possible MathML "mathvariant" values. Blahtex
// uses these to record fonts in the layout tree; they get converted to
// MathML 1.x font attributes if needed.
enum MathmlFont
{
    cMathmlFontNormal,
    cMathmlFontBold,
    cMathmlFontItalic,
    cMathmlFontBoldItalic,
    cMathmlFontDoubleStruck,
    cMathmlFontBoldFraktur,
    cMathmlFontScript,
    cMathmlFontBoldScript,
    cMathmlFontFraktur,
    cMathmlFontSansSerif,
    cMathmlFontBoldSansSerif,
    cMathmlFontSansSerifItalic,
    cMathmlFontSansSerifBoldItalic,
    cMathmlFontMonospace
};

// String versions of the MathML mathvariant fonts.
// (See enum MathmlFont in LayoutTree.h.)
extern std::wstring gMathmlFontStrings[];


// Represents a node in an MathML tree.
struct MathmlNode
{
    enum Type
    {
        // Leaf nodes types ("token elements" in MathML documentation):
        cTypeMi,
        cTypeMo,
        cTypeMn,
        cTypeMspace,
        cTypeMtext,
        
        // Internal nodes types:
        cTypeMrow,
        cTypeMstyle,
        cTypeMsub,
        cTypeMsup,
        cTypeMsubsup,
        cTypeMunder,
        cTypeMover,
        cTypeMunderover,
        cTypeMfrac,
        cTypeMsqrt,
        cTypeMroot,
        cTypeMtable,
        cTypeMtr,
        cTypeMtd,
        cTypeMpadded
    }
    mType;

    enum Attribute
    {
        cAttributeDisplaystyle,
        cAttributeScriptlevel,
        cAttributeMathvariant,
        cAttributeMathcolor,
        cAttributeLspace,
        cAttributeRspace,
        cAttributeWidth,
        cAttributeStretchy,
        cAttributeMinsize,
        cAttributeMaxsize,
        cAttributeAccent,
        cAttributeMovablelimits,
        cAttributeLinethickness,
        cAttributeColumnalign,
        cAttributeColumnspacing,
        cAttributeRowspacing,
        cAttributeFontfamily,
        cAttributeFontstyle,
        cAttributeFontweight
    };

    std::map<Attribute, std::wstring> mAttributes;

    // mText is only used for leaf nodes: it holds the text that is
    // displayed between the opening and closing tags
    std::wstring mText;
    
    // mChildren is only used for internal nodes
    std::list<MathmlNode*> mChildren;
    
    MathmlNode(Type type, const std::wstring& text = L"") :
        mType(type),
        mText(text)
    { }
    
    ~MathmlNode();
    
    // This function adds mathvariant (for MathML 2.0) or fontstyle/
    // fontweight/fontfamily (for MathML 1.x) as appropriate to this node
    // to obtain the desired font. It knows about MathML defaults (like the
    // annoying automatic italic for single character <mi> nodes).
    void AddFontAttributes(
        MathmlFont desiredFont,
        const MathmlOptions& options
    );
    
    
    // Print() recursively prints the tree rooted at this node to the
    // given output stream.
    //
    // XML entities translated according to EncodingOptions.
    //
    // If "indent" is true, it will print each tag pair on a new line, and
    // add appropriate indenting.
    void Print(
        std::wostream& os,
        const EncodingOptions& options,
        bool indent,
        int depth = 0
    ) const;

    // Used internally by Print:
    void PrintType(std::wostream& os) const;
    void PrintAttributes(std::wostream& os) const;
#ifdef BLAHTEXML_USING_XERCES
    void PrintAsSAX2(ContentHandler& sax, const std::wstring& prefix, bool ignoreFirstmrow) const;
#endif
};

}

#endif

// end of file @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

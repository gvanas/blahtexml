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

#ifndef BLAHTEX_INTERFACE_H
#define BLAHTEX_INTERFACE_H

#include <string>
#include <memory>
#include "Misc.h"
#include "Manager.h"
#include "XmlEncode.h"

#ifdef BLAHTEXML_USING_XERCES
#include <xercesc/sax2/ContentHandler.hpp>
XERCES_CPP_NAMESPACE_USE
#endif

namespace blahtex
{

// If you want to use blahtex in your own code, using an Interface object
// is probably the easiest way to do it. It's essentially a wrapper for
// the Manager class, putting all the options and methods in one convenient
// place.
//
// To use it:
// (1) Declare an Interface object
// (2) Set the various public members to control options
//     (the data types are explained in Misc.h)
// (3) Call ProcessInput() on your input
// (4) Call GetMathml() to get the MathML output
// (5) Call GetPurifiedTex() to get a complete TeX file that could be sent
//     to latex to generate graphical output

class Interface
{
private:
    std::auto_ptr<Manager> mManager;

public:
    MathmlOptions mMathmlOptions;
    EncodingOptions mEncodingOptions;
    PurifiedTexOptions mPurifiedTexOptions;
    bool mTexvcCompatibility;
    bool mIndented;

    Interface() :
        mTexvcCompatibility(false),
        mIndented(false)
    {
    }

    const Manager* GetManager() const
    {
        return mManager.get();
    }

    void ProcessInput(const std::wstring& input, bool displayStyle = false);
    std::wstring GetMathml();
    std::wstring GetPurifiedTex();
    std::wstring GetPurifiedTexOnly();
#ifdef BLAHTEXML_USING_XERCES
    void PrintAsSAX2(ContentHandler& sax, const std::wstring& prefix, bool ignoreFirstmrow) const;
#endif
};

}

#endif

// end of file @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

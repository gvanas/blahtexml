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

#include <iostream>
#include "XercesString.h"

XercesString::XercesString()
    : basic_string<XMLCh>()
{
}

XercesString::XercesString(const XMLCh *s)
    : basic_string<XMLCh>(s)
{
}

XercesString::XercesString(const char *s)
    : basic_string<XMLCh>()
{
    XMLCh *Xs = XMLString::transcode(s);
    assign(Xs);
    XMLString::release(&Xs);
}

XercesString::XercesString(const wstring& in)
    : basic_string<XMLCh>()
{
    for(unsigned int i=0; i<in.length(); ++i) {
        wchar_t utf32 = in[i];
        if (utf32 >= 0x10000UL) {
            push_back(0xD800 - 0x40 + (utf32 >> 10));
            push_back(0xDC00 + (utf32 & 0x3FF));
        }
        else
            push_back(utf32);
    }
}

wstring XercesString::convertTowstring()
{
    wstring out;
    wchar_t ucs4;
    bool firstSurrogate = false;
    for(unsigned int i=0; i<length(); ++i) {
        XMLCh utf16 = at(i);
        if (firstSurrogate) {
            if ((utf16 & 0xFC00) == 0xDC00) {
                ucs4 += utf16-0xDC00UL+0x10000UL;
                out.push_back(ucs4);
            }
            /*else
                throw (...);*/
            firstSurrogate = false;
        }
        else if ((utf16 & 0xFC00) == 0xD800) {
            ucs4 = (utf16-0xD800UL) << 10;
            firstSurrogate = true;
        }
        else {
            out.push_back(utf16);
        }
    }
    return out;
}

using namespace std;

ostream& operator<<(ostream& out, const XercesString& s)
{
    char *temp = XMLString::transcode(s.c_str());
    out << temp;
    XMLString::release(&temp);
    return out;
}

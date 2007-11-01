// File "XercesString.cpp"
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

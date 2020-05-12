/*
blahtex: a TeX to MathML converter designed with MediaWiki in mind
blahtexml: an extension of blahtex with XML processing in mind
http://gva.noekeon.org/blahtexml

Copyright (c) 2006, David Harvey
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the names of the authors nor the names of their affiliation may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef BLAHTEX_UNICODE_CONVERTER_H
#define BLAHTEX_UNICODE_CONVERTER_H

#include <string>
#ifndef WIN32_CODECONV
#include <iconv.h>
#endif

// UnicodeConverter handles all UTF8 <=> wchar_t conversions. It's
// basically a wrapper for the iconv library in terms of
// - string (assumed to be in UTF-8) and
// - wstring (in internal wchar_t format, which may be big-endian or
// little-endian depending on platform).
class UnicodeConverter
{
    public:
        UnicodeConverter() :
            mIsOpen(false)
        { }

        ~UnicodeConverter();

        // Open() must be called before using this object.
        //
        // It will throw a std::runtime_error object if
        // (1) we are running on a platform with less than 4 bytes
        //     per wchar_t, or
        // (2) an appropriate iconv_t converter object can't be created
        void Open();

        std::wstring ConvertIn(const std::string& input);
        std::string ConvertOut(const std::wstring& input);

        // The above 'ConvertIn' and 'ConvertOut' functions will throw this
        // exception object if their input is invalid (e.g. invalid UTF-8).
        // More serious problems report a std::logic_error.
        class Exception
        {
        };

    private:
        bool mIsOpen;

#ifndef WIN32_CODECONV
        // mOutHandle is the iconv object handling wchar_t => UTF-8,
        // mInHandle does the other way.
        iconv_t mOutHandle;
        iconv_t mInHandle;
#endif
};

#endif

// end of file @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

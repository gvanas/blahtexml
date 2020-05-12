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

#include "UnicodeConverter.h"
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <cerrno>

using namespace std;

UnicodeConverter::~UnicodeConverter()
{
    if (mIsOpen)
    {
        iconv_close(mInHandle);
        iconv_close(mOutHandle);
    }
}

void UnicodeConverter::Open()
{
    if (mIsOpen)
        throw logic_error(
            "UnicodeConverter::Open called on already open object"
        );

#ifdef WCHAR_T_IS_16BIT
    if (sizeof(wchar_t) != 2)
        throw runtime_error(
            "The wchar_t data type on this system is not two bytes wide"
        );
#else
    if (sizeof(wchar_t) != 4)
        throw runtime_error(
            "The wchar_t data type on this system is not four bytes wide"
        );
#endif

    // Determine endian-ness of wchar_t.
    // (Really we should be able to just use "WCHAR_T". This unfortunately
    // doesn't seem to available on darwin.)
    wchar_t testChar = L'A';
    const char* UcsString =
        (*(reinterpret_cast<char*>(&testChar)) == 'A')
#ifdef WCHAR_T_IS_16BIT
            ? "UTF-16LE" : "UTF-16BE";
#else
            ? "UCS-4LE" : "UCS-4BE";
#endif

    mInHandle = iconv_open(UcsString, "UTF-8");
    if (mInHandle == (iconv_t)(-1))
    {
        switch (errno)
        {
            case EMFILE:
                throw runtime_error(
                    "iconv_open failed with errno == EMFILE"
                );
            case ENFILE:
                throw runtime_error(
                    "iconv_open failed with errno == ENFILE"
                );
            case ENOMEM:
                throw runtime_error(
                    "iconv_open failed with errno == ENOMEM"
                );
            case EINVAL:
                throw runtime_error(
                    "iconv_open failed with errno == EINVAL"
                );
            default:
                throw runtime_error(
                    "iconv_open failed with unknown error code"
                );
        }
    }

    mOutHandle = iconv_open("UTF-8", UcsString);
    if (mOutHandle == (iconv_t)(-1))
    {
        switch (errno)
        {
            case EMFILE:
                throw runtime_error(
                    "iconv_open failed with errno == EMFILE"
                );
            case ENFILE:
                throw runtime_error(
                    "iconv_open failed with errno == ENFILE"
                );
            case ENOMEM:
                throw runtime_error(
                    "iconv_open failed with errno == ENOMEM"
                );
            case EINVAL:
                throw runtime_error(
                    "iconv_open failed with errno == EINVAL"
                );
            default:
                throw runtime_error(
                    "iconv_open failed with unknown error code"
                );
        }
    }

    mIsOpen = true;
}

wstring UnicodeConverter::ConvertIn(const string& input)
{
    if (!mIsOpen)
        throw logic_error(
            "UnicodeConverter::ConvertIn called "
            "before UnicodeConverter::Open"
        );

    char* inputBuf  = new char[input.size()];
    memcpy(inputBuf, input.c_str(), input.size());

    char* outputBuf = new char[input.size() * sizeof(wchar_t)];

    // The following garbage is needed to handle the unfortunate
    // inconsistency between Linux and BSD definitions for the second
    // parameter of iconv. BSD (including Mac OS X) requires const char*,
    // whereas Linux requires char*, and neither option seems to produce
    // error-free, warning-free compilation on both systems simultaneously.
#ifdef BLAHTEX_ICONV_CONST
    const
#endif
    char* source = inputBuf;
    char* dest = outputBuf;

    size_t  inBytesLeft = input.size();
    size_t outBytesLeft = input.size() * sizeof(wchar_t);

    if (iconv(
        mInHandle,
        &source,
        &inBytesLeft,
        &dest,
        &outBytesLeft
    ) == -1)
    {
        delete[] inputBuf;
        delete[] outputBuf;
        switch (errno)
        {
            case EILSEQ:
            case EINVAL:    throw UnicodeConverter::Exception();
            default:
                throw logic_error(
                    "Conversion problem in UnicodeConverter::ConvertIn"
                );
        }
    }

    wstring output(
        reinterpret_cast<wchar_t*>(outputBuf),
        input.size() - outBytesLeft / sizeof(wchar_t)
    );
    delete[] inputBuf;
    delete[] outputBuf;
    return output;
}

string UnicodeConverter::ConvertOut(const wstring& input)
{
    if (!mIsOpen)
        throw logic_error(
            "UnicodeConverter::ConvertOut called "
            "before UnicodeConverter::Open"
        );

    wchar_t* inputBuf = new wchar_t[input.size()];
    wmemcpy(inputBuf, input.c_str(), input.size());

    char* outputBuf = new char[input.size() * sizeof(wchar_t)];

#ifdef BLAHTEX_ICONV_CONST
    const
#endif
    char* source = reinterpret_cast<char*>(inputBuf);
    char* dest = outputBuf;

    size_t  inBytesLeft = input.size() * sizeof(wchar_t);
    size_t outBytesLeft = input.size() * sizeof(wchar_t);

    if (iconv(
        mOutHandle,
        &source,
        &inBytesLeft,
        &dest,
        &outBytesLeft
    ) == -1)
    {
        delete[] inputBuf;
        delete[] outputBuf;
        switch (errno)
        {
            case EILSEQ:
            case EINVAL:    throw UnicodeConverter::Exception();
            default:
                throw logic_error(
                    "Conversion problem in UnicodeConverter::ConvertIn"
                );
        }
    }

    string output(outputBuf, input.size() * sizeof(wchar_t) - outBytesLeft);
    delete[] inputBuf;
    delete[] outputBuf;
    return output;
}

// end of file @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

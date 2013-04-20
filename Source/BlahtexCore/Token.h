/*
 blahtex: a TeX to MathML converter designed with MediaWiki in mind
 blahtexml: an extension of blahtex with XML processing in mind
 http://gva.noekeon.org/blahtexml
 
 Copyright (c) 2006, David Harvey
 Copyright (c) 2009, Gilles Van Assche
 Copyright (c) 2013, Troy Chmieleski, Matt Rajca
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * Neither the names of the authors nor the names of their affiliation may be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef BLAHTEX_TOKEN_H
#define BLAHTEX_TOKEN_H

#include <iostream>

namespace blahtex
{

class Token
{
private:
    std::wstring mValue;
    unsigned long long mStartPos, mLength;
    
public:
    Token(const std::wstring & value, const unsigned long long startPos, const unsigned long long length)
    {
        mValue = value;
        mStartPos = startPos;
        mLength = length;
    }
    
    Token(const Token & source) {
        mValue = source.mValue;
        mStartPos = source.mStartPos;
        mLength = source.mLength;
    }
    
    const std::wstring & getValue() const
    {
        return mValue;
    }
	
    const unsigned long long getStartPos() const
    {
        return mStartPos;
    }
	
    const unsigned long long getLength() const
    {
        return mLength;
    }
    
    void setValue(const std::wstring & token)
    {
        mValue = token;
    }
};

extern Token EmptyToken;

}

#endif

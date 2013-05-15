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

#ifndef BLAHTEX_MACROPROCESSOR_H
#define BLAHTEX_MACROPROCESSOR_H

#include <string>
#include <vector>
#include <map>
#include "Misc.h"

namespace blahtex
{

// The time spent by the parser should be O(cMaxParseCost).
// The aim is to prevent a nasty user inducing exponential time via
// tricky macro definitions.
const unsigned cMaxParseCost = 20000;


// MacroProcessor maintains a stack of tokens, can be queried for the next
// available token, and expands macros automatically. It is the layer
// between tokenising (handled by the Manager class) and parsing proper
// (handled by the Parser class).
//
// It does not process "\newcommand" commands automatically; instead it
// passes "\newcommand" straight back to the caller, and the caller is
// responsible for calling MacroProcessor::HandleNewcommand.
// (Rationale: this gives results much closer to real TeX parsing. For
// example, we wouldn't want "x^\newcommand{\stuff}{xyz}\stuff" to be
// construed as legal input.)
class MacroProcessor
{
public:
    // Input is a vector of strings, one for each input token.
    MacroProcessor(const std::vector<Token>& input);

    // Returns the next token on the stack (without removing it), after
    // expanding macros.
    // Returns empty string if there are no tokens left.
    Token & Peek();

    // Same as Peek(), but also removes the token.
    // Returns empty string if there are no tokens left.
    std::wstring Get();
	
	Token & GetToken();

    // Pops the current token.
    void Advance();

    // Pops consecutive whitespace tokens.
    void SkipWhitespace();

    // Assuming that "\newcommand" has just been seen and popped off the
    // stack, this function processes a subsequent macro definition.
    void HandleNewcommand();
	
	Token * FindLastInstanceOfToken(const std::wstring & tokenString);

private:

    // Records information about a single macro.
    struct Macro
    {
        // The number of parameters the macro accepts. (Blahtex doesn't
        // handle optional arguments.)
        int mParameterCount;

        // The sequence of tokens that get substituted when this macro is
        // expanded. Arguments are indicated as follows: first the string
        // "#", and then the string "n", where n is a number between 1 and
        // 9, indicating which argument to substitute.
        std::vector<std::wstring> mReplacement;

        Macro() :
            mParameterCount(0)
        { }
    };

    // List of all currently recognised macros.
    wishful_hash_map<std::wstring, Macro> mMacros;

    // The token stack; the top of the stack is mTokens.back().
    std::vector<Token> mTokens;
	long long mBackIndex;

    // This flag is set if we have already ascertained that the current
    // token doesn't need to undergo macro expansion.
    // (This is just an optimisation so that successive calls to Peek/Get
    // don't have to do extra work.)
    bool mIsTokenReady;

    // Reads a single macro argument; that is, either a single token, or if
    // that token is "{", reads all the way up to the matching "}". The
    // argument (not including delimiting braces) is appended to "output".
    //
    // Returns true on success, or false if the argument is missing.
    bool ReadArgument(std::vector<std::wstring>& output);

    // Skips whitespace without expanding macros.
    void SkipWhitespaceRaw();

    // Total approximate cost of parsing activity so far.
    // (See cMaxParseCost.)
    unsigned mCostIncurred;
};

}

#endif

// end of file @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

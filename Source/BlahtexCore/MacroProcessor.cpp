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

#include <iterator>
#include "MacroProcessor.h"

using namespace std;

namespace blahtex
{

// Implemented in Parser.cpp:
extern bool IsInTokenTables(const wstring& token);

// If the input string ends with "Reserved", this function strips it off.
// All other input is returned unharmed.
//
// The purpose is to convert internal commands like "\textReserved" into
// plain old "\text" for error reporting purposes.
wstring StripReservedSuffix(const wstring& input)
{
    if (input.size() >= 8 &&
        input.substr(input.size() - 8, 8) == L"Reserved"
    )
        return input.substr(0, input.size() - 8);
    else
        return input;
}

MacroProcessor::MacroProcessor(const vector<Token>& input)
{
	for (vector<Token>::const_reverse_iterator iter = input.rbegin(); iter != input.rend(); ++iter)
	{
		mTokens.push_back(*iter);
	}
	
	mBackIndex = mTokens.size() - 1;
	
    mCostIncurred = input.size();
    mIsTokenReady = false;
}

void MacroProcessor::Advance()
{
    if (mBackIndex >= 0)
    {
		mBackIndex--;
        mCostIncurred++;
        mIsTokenReady = false;
    }
}

void MacroProcessor::SkipWhitespace()
{
    while (Peek().getValue() == L" ")
        Advance();
}

void MacroProcessor::SkipWhitespaceRaw()
{
    while (mBackIndex >= 0 && mTokens[mBackIndex].getValue() == L" ")
        Advance();
}

bool MacroProcessor::ReadArgument(vector<wstring>& output)
{
    SkipWhitespaceRaw();
    if (mBackIndex == -1)
        // Missing argument
        return false;

    wstring token = mTokens[mBackIndex].getValue();
    mBackIndex--;
    mCostIncurred++;
    if (token == L"}")
        // Argument can't start with "}"
        return false;

    if (token == L"{")
    {
        // Keep track of brace nesting depth so we know which is the
        // matching closing brace
        int braceDepth = 1;
        while (mBackIndex >= 0)
        {
            mCostIncurred++;
            wstring token = mTokens[mBackIndex].getValue();
            mBackIndex--;
            if (token == L"{")
                braceDepth++;
            else if (token == L"}" && --braceDepth == 0)
                break;
            output.push_back(token);
        }
        if (braceDepth > 0)
            throw Exception(L"UnmatchedOpenBrace");
    }
    else
        output.push_back(token);

    mIsTokenReady = false;
    return true;
}

wstring MacroProcessor::Get()
{
    wstring token = Peek().getValue();
    Advance();
    return token;
}

Token & MacroProcessor::GetToken()
{
	Token &token = Peek();
    Advance();
    return token;
}

void MacroProcessor::HandleNewcommand()
{
    // pop the "\newcommand" command:
    mBackIndex--;
    mCostIncurred++;

    // gobble opening brace
    SkipWhitespaceRaw();
    if (mBackIndex == -1 || mTokens[mBackIndex].getValue() != L"{")
        throw Exception(L"MissingOpenBraceAfter", L"\\newcommand");
    mBackIndex--;

    // grab new command being defined
    SkipWhitespaceRaw();
    if (mBackIndex == -1 ||
        mTokens[mBackIndex].getValue().empty() ||
        mTokens[mBackIndex].getValue()[0] != L'\\'
    )
        throw Exception(L"MissingCommandAfterNewcommand");
    wstring newCommand = mTokens[mBackIndex].getValue();
    if (mMacros.count(newCommand) || IsInTokenTables(newCommand))
        throw Exception(
            L"IllegalRedefinition",
            StripReservedSuffix(newCommand)
        );
    mBackIndex--;

    // gobble close brace
    SkipWhitespaceRaw();
    if (mBackIndex == -1)
        throw Exception(L"UnmatchedOpenBrace");
    if (mTokens[mBackIndex].getValue() != L"}")
        throw Exception(L"MissingCommandAfterNewcommand");
    mBackIndex--;

    Macro& macro = mMacros[newCommand];

    SkipWhitespaceRaw();
    // Determine the number of arguments, if specified.
    if (mBackIndex >= 0 && mTokens[mBackIndex].getValue() == L"[")
    {
        mBackIndex--;

        SkipWhitespaceRaw();
        if (mBackIndex == -1 || mTokens[mBackIndex].getValue().size() != 1)
            throw Exception(L"MissingOrIllegalParameterCount", newCommand);
        macro.mParameterCount = static_cast<int>(mTokens[mBackIndex].getValue()[0] - L'0');
        if (macro.mParameterCount <= 0 || macro.mParameterCount > 9)
            throw Exception(L"MissingOrIllegalParameterCount", newCommand);
        mBackIndex--;

        SkipWhitespaceRaw();
        if (mBackIndex == -1 || mTokens[mBackIndex].getValue() != L"]")
            throw Exception(L"UnmatchedOpenBracket");
        mBackIndex--;
    }

    // Read and store the tokens which make up the macro replacement.
    if (!ReadArgument(macro.mReplacement))
        throw Exception(L"NotEnoughArguments", L"\\newcommand");
}

Token & MacroProcessor::Peek()
{
    while (mBackIndex >= 0)
    {
        // This is the only place that we check that the user hasn't
        // exceeded the token limit.
        if ((mBackIndex + 1) + (++mCostIncurred) >= cMaxParseCost)
            throw Exception(L"TooManyTokens");

        if (mIsTokenReady)
            return mTokens[mBackIndex];

        // "\sqrt" needs special handling due to its optional argument.
        // Something like "\sqrtReserved{x}" gets converted to "\sqrt{x}".
        // Something like "\sqrtReserved[y]{x}" gets converted to
        // "\rootReserved{y}{x}".
        //
        // (Blahtex doesn't handle grouping of [...] the same way as texvc;
        // it does it the TeX way. For example, "\sqrt[\sqrt[2]{3}]{4}"
        // generates an error, whereas it is valid in texvc.)
        //
        // We need to take into account grouping braces,
        // e.g. "\sqrt[{]}]{2}" should be valid.
        if (mTokens[mBackIndex].getValue() == L"\\sqrtReserved")
        {
            mBackIndex--;

            SkipWhitespaceRaw();
            if (mBackIndex >= 0 && mTokens[mBackIndex].getValue() == L"[")
            {
                mTokens[mBackIndex].setValue(L"{");

#ifndef _MSC_VER
#warning TODO: test this
#endif
				vector<Token>::iterator ptr = mTokens.begin() + mBackIndex + 1;

                int braceDepth = 0;
                while (ptr != mTokens.begin() &&
                    (braceDepth > 0 || (*ptr).getValue() != L"]")
                )
                {
                    mCostIncurred++;
                    if ((*ptr).getValue() == L"{")
                        braceDepth++;
                    else if ((*ptr).getValue() == L"}")
                    {
                        if (--braceDepth < 0)
                            throw Exception(L"UnmatchedCloseBrace");
                    }
                    ptr--;
                }
                if (ptr == mTokens.begin())
                    throw Exception(L"UnmatchedOpenBracket");
                if ((*ptr).getValue() != L"]")
                    throw Exception(L"NotEnoughArguments", L"\\sqrt");
                (*ptr).setValue(L"}");
				
#ifndef _MSC_VER
#warning TODO: actually store the token range and test this
#endif
				mTokens.insert(mTokens.begin() + (mBackIndex + 1), Token(L"\\rootReserved", 0, 0));
				mBackIndex++;
                mIsTokenReady = true;
				
				return mTokens[mBackIndex];
            }
            else
            {
#ifndef _MSC_VER
#warning TODO: actually store the token range and test this
#endif
				mTokens.insert(mTokens.begin() + (mBackIndex + 1), Token(L"\\sqrt", 0, 0));
				mBackIndex++;
                mIsTokenReady = true;
				
				return mTokens[mBackIndex];
            }
        }
        else
        {
            wstring token = mTokens[mBackIndex].getValue();
            wishful_hash_map<wstring, Macro>::const_iterator
                macroPtr = mMacros.find(token);
            if (macroPtr == mMacros.end())
            {
                // In this case it's not "\sqrt" and not a macro, so
                // we're finished here.
                mIsTokenReady = true;
                return mTokens[mBackIndex];
            }

            const Macro& macro = macroPtr->second;
            mBackIndex--;

            // It's a macro. Determines the arguments to substitute in....
            vector<vector<wstring> > arguments(macro.mParameterCount);
            for (int argumentIndex = 0;
                argumentIndex < macro.mParameterCount;
                argumentIndex++
            )
                if (!ReadArgument(arguments[argumentIndex]))
                    throw Exception(
                        L"NotEnoughArguments",
                        StripReservedSuffix(token)
                    );

            // ... and now write the replacement, substituting
            // arguments as we go.
            const vector<wstring>& replacement = macro.mReplacement;
            vector<wstring> output;
            for (vector<wstring>::const_iterator
                source = replacement.begin();
                source != replacement.end();
                source++
            )
            {
                mCostIncurred++;
                if (*source == L"#")
                {
                    if (++source == replacement.end() ||
                        source->size() != 1
                    )
                        throw Exception(
                            L"MissingOrIllegalParameterIndex",
                            token
                        );

                    int parameterIndex
                        = static_cast<int>((*source)[0] - '1');

                    // FIX: perhaps this next error should be flagged when
                    // reading the definition of the macro rather than
                    // during macro expansion
                    if (parameterIndex < 0 ||
                        parameterIndex >= macro.mParameterCount
                    )
                        throw Exception(
                            L"MissingOrIllegalParameterIndex",
                            token
                        );
                    copy(
                        arguments[parameterIndex].begin(),
                        arguments[parameterIndex].end(),
                        back_inserter(output)
                    );
                    mCostIncurred += arguments[parameterIndex].size();
                }
                else
                    output.push_back(*source);
            }
			
#ifndef _MSC_VER
#warning TODO: include token ranges
#endif
			for(vector<wstring>::reverse_iterator iter = output.rbegin(); iter != output.rend(); ++iter)
			{
				mTokens.insert(mTokens.begin() + mBackIndex + 1, Token(*iter, 0, 0));
				mBackIndex++;
			}
			
            mCostIncurred += output.size();
        }
    }
	
	return EmptyToken;
}

Token * MacroProcessor::FindLastInstanceOfToken(const std::wstring & tokenString)
{
	for (long n = mBackIndex+1;  n < mTokens.size(); n++)
	{
		if (mTokens[n].getValue() == tokenString)
			return &mTokens[n];
	}
	
	return NULL;
}

}

// end of file @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

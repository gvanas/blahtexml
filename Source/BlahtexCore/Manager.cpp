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

#include <sstream>
#include <stdexcept>
#include "Manager.h"
#include "Parser.h"

using namespace std;

namespace blahtex
{


// I don't entirely trust the wide versions of isalpha etc, so this
// function does the job instead.
bool IsAlphabetic(wchar_t c)
{
    return (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z');
}


// Tokenise() splits the given input into tokens, each represented by a
// string. The output is APPENDED to "output".
//
// There are several types of tokens:
// * single characters like "a", or "{", or single non-ASCII unicode
//   characters
// * alphabetic commands like "\frac"
// * commands like "\," which have a single nonalphabetic character
//   after the backslash
// * commands like "\   " which have their whitespace collapsed,
//   stored as "\ "
// * other consecutive whitespace characters which get collapsed to
//   just " "
// * the sequence "\begin   {  stuff  }" gets stored as the single token
//   "\begin{  stuff  }". Note that whitespace is preserved between the
//   braces but not between "\begin" and "{". Similarly for "\end".
void Tokenise(const wstring& input, vector<Token>& output)
{
    wstring::const_iterator ptr = input.begin();

    while (ptr != input.end())
    {
        // merge adjacent whitespace
        if (iswspace(*ptr))
        {
			unsigned long long index = std::distance(input.begin(), ptr);
			
            output.push_back(Token(L" ", index, 1));
            do
                ptr++;
            while (ptr != input.end() && iswspace(*ptr));
        }
        // boring single character tokens
        else if (*ptr != L'\\')
        {
            // Disallow non-printable, non-whitespace ASCII
            if (*ptr < L' ' || *ptr == 0x7F)
                throw Exception(L"IllegalCharacter");
			
			unsigned long long index = std::distance(input.begin(), ptr);
			
            output.push_back(Token(wstring(1, *ptr++), index, 1));
        }
        else
        {
            // tokens starting with backslash
            wstring token = L"\\";

            if (++ptr == input.end())
                throw Exception(L"IllegalFinalBackslash");
            if (IsAlphabetic(*ptr))
            {
                // plain alphabetic commands
				unsigned long long startPos = std::distance(input.begin(), ptr) - 1;
				
                do
                    token += *ptr++;
                while (ptr != input.end() && IsAlphabetic(*ptr));
				
				unsigned long long length = std::distance(input.begin(), ptr) - startPos;

                // Special treatment for "\begin" and "\end"; need to
                // collapse "\begin  {xyz}" to "\begin{xyz}", and store it
                // as a single token.
                if (token == L"\\begin" || token == L"\\end")
                {
                    while (ptr != input.end() && iswspace(*ptr))
                        ptr++;
                    if (ptr == input.end() || *ptr != L'{')
                        throw Exception(L"MissingOpenBraceAfter", token);
                    token += *ptr++;
                    while (ptr != input.end() && *ptr != L'}')
                        token += *ptr++;
                    if (ptr == input.end())
                        throw Exception(L"UnmatchedOpenBrace");
                    token += *ptr++;
					
					length = std::distance(input.begin(), ptr) - startPos;
                }
				
				output.push_back(Token(token, startPos, length));
            }
            else if (iswspace(*ptr))
            {
				unsigned long long startPos = std::distance(input.begin(), ptr) - 1;
				
                // commands like "\    "
                token += L" ";
                do
                    ptr++;
                while (ptr != input.end() && iswspace(*ptr));
				
				output.push_back(Token(token, startPos, 2));
            }
            // commands like "\," and "\;"
			
            else
			{
				unsigned long long startPos = std::distance(input.begin(), ptr) - 1;
				
                token += *ptr++;
				
				output.push_back(Token(token, startPos, 2));
			}
        }
    }
}


wstring Manager::gTexvcCompatibilityMacros =

    // First we have some macros which are not part of tex/latex/amslatex
    // but which texvc recognises, so for backward compatibility we define
    // them here too. Most of these are apparently intended to cater for
    // those more familiar with HTML entities.

    L"\\newcommand{\\R}{{\\mathbb R}}"
    L"\\newcommand{\\Reals}{\\R}"
    L"\\newcommand{\\reals}{\\R}"
    L"\\newcommand{\\Z}{{\\mathbb Z}}"
    L"\\newcommand{\\N}{{\\mathbb N}}"
    L"\\newcommand{\\natnums}{\\N}"
    L"\\newcommand{\\Complex}{{\\mathbb C}}"
    L"\\newcommand{\\cnums}{\\Complex}"
    L"\\newcommand{\\alefsym}{\\aleph}"
    L"\\newcommand{\\alef}{\\aleph}"
    L"\\newcommand{\\larr}{\\leftarrow}"
    L"\\newcommand{\\rarr}{\\rightarrow}"
    L"\\newcommand{\\Larr}{\\Leftarrow}"
    L"\\newcommand{\\lArr}{\\Leftarrow}"
    L"\\newcommand{\\Rarr}{\\Rightarrow}"
    L"\\newcommand{\\rArr}{\\Rightarrow}"
    L"\\newcommand{\\uarr}{\\uparrow}"
    L"\\newcommand{\\uArr}{\\Uparrow}"
    L"\\newcommand{\\Uarr}{\\Uparrow}"
    L"\\newcommand{\\darr}{\\downarrow}"
    L"\\newcommand{\\dArr}{\\Downarrow}"
    L"\\newcommand{\\Darr}{\\Downarrow}"
    L"\\newcommand{\\lrarr}{\\leftrightarrow}"
    L"\\newcommand{\\harr}{\\leftrightarrow}"
    L"\\newcommand{\\Lrarr}{\\Leftrightarrow}"
    L"\\newcommand{\\Harr}{\\Leftrightarrow}"
    L"\\newcommand{\\lrArr}{\\Leftrightarrow}"
    // The next one looks like a typo in the texvc source code:
    L"\\newcommand{\\hAar}{\\Leftrightarrow}"
    L"\\newcommand{\\sub}{\\subset}"
    L"\\newcommand{\\supe}{\\supseteq}"
    L"\\newcommand{\\sube}{\\subseteq}"
    L"\\newcommand{\\infin}{\\infty}"
    L"\\newcommand{\\lang}{\\langle}"
    L"\\newcommand{\\rang}{\\rangle}"
    L"\\newcommand{\\real}{\\Re}"
    L"\\newcommand{\\image}{\\Im}"
    L"\\newcommand{\\bull}{\\bullet}"
    L"\\newcommand{\\weierp}{\\wp}"
    L"\\newcommand{\\isin}{\\in}"
    L"\\newcommand{\\plusmn}{\\pm}"
    L"\\newcommand{\\Dagger}{\\ddagger}"
    L"\\newcommand{\\exist}{\\exists}"
    L"\\newcommand{\\sect}{\\S}"
    L"\\newcommand{\\clubs}{\\clubsuit}"
    L"\\newcommand{\\spades}{\\spadesuit}"
    L"\\newcommand{\\hearts}{\\heartsuit}"
    L"\\newcommand{\\diamonds}{\\diamondsuit}"
    L"\\newcommand{\\sdot}{\\cdot}"
    L"\\newcommand{\\ang}{\\angle}"
    L"\\newcommand{\\thetasym}{\\theta}"
    L"\\newcommand{\\Alpha}{A}"
    L"\\newcommand{\\Beta}{B}"
    L"\\newcommand{\\Epsilon}{E}"
    L"\\newcommand{\\Zeta}{Z}"
    L"\\newcommand{\\Eta}{H}"
    L"\\newcommand{\\Iota}{I}"
    L"\\newcommand{\\Kappa}{K}"
    L"\\newcommand{\\Mu}{M}"
    L"\\newcommand{\\Nu}{N}"
    L"\\newcommand{\\Rho}{P}"
    L"\\newcommand{\\Tau}{T}"
    L"\\newcommand{\\Chi}{X}"
    L"\\newcommand{\\arccot}{\\operatorname{arccot}}"
    L"\\newcommand{\\arcsec}{\\operatorname{arcsec}}"
    L"\\newcommand{\\arccsc}{\\operatorname{arccsc}}"
    L"\\newcommand{\\sgn}{\\operatorname{sgn}}"

    // The commands in this next group are defined in tex/latex/amslatex,
    // but they don't get mapped to what texvc thinks (e.g. "\part" is used
    // in typesetting books to mean a unit somewhat larger than a chapter,
    // like "Part IV").
    //
    // We'll stick to the way texvc does it, especially since wikipedia has
    // quite a number of equations using them.
    L"\\newcommand{\\empty}{\\emptyset}"
    L"\\newcommand{\\and}{\\wedge}"
    L"\\newcommand{\\or}{\\vee}"
    L"\\newcommand{\\part}{\\partial}"
;

wstring Manager::gStandardMacros =

    // The next group are standard TeX/LaTeX/AMS-LaTeX synonyms.
    L"\\newcommand{\\|}{\\Vert}"
    L"\\newcommand{\\implies}{\\;\\Longrightarrow\\;}"
    L"\\newcommand{\\neg}{\\lnot}"
    L"\\newcommand{\\ne}{\\neq}"
    L"\\newcommand{\\ge}{\\geq}"
    L"\\newcommand{\\le}{\\leq}"
    L"\\newcommand{\\land}{\\wedge}"
    L"\\newcommand{\\lor}{\\vee}"
    L"\\newcommand{\\gets}{\\leftarrow}"
    L"\\newcommand{\\to}{\\rightarrow}"
    L"\\newcommand{\\doublecap}{\\Cap}"
    L"\\newcommand{\\restriction}{\\upharpoonright}"
    L"\\newcommand{\\llless}{\\lll}"
    L"\\newcommand{\\gggtr}{\\ggg}"
    L"\\newcommand{\\Doteq}{\\doteqdot}"
    L"\\newcommand{\\doublecup}{\\Cup}"
    L"\\newcommand{\\dasharrow}{\\dashleftarrow}"
    L"\\newcommand{\\vartriangleleft}{\\lhd}"
    L"\\newcommand{\\vartriangleright}{\\rhd}"
    L"\\newcommand{\\trianglelefteq}{\\unlhd}"
    L"\\newcommand{\\trianglerighteq}{\\unrhd}"
    L"\\newcommand{\\Join}{\\bowtie}"
    L"\\newcommand{\\Diamond}{\\lozenge}"

    // The amsfonts package accepts the following two commands, but warns
    // that they are obsolete, so let's just quietly replace them.
    L"\\newcommand{\\Bbb}{\\mathbb}"
    L"\\newcommand{\\bold}{\\mathbf}"

    // Now we come to the xxxReserved commands. These are all implemented
    // as macros in TeX, so for maximum compatibility, we want to treat
    // their arguments the way a TeX macro does. The strategy is the
    // following. First, in Manager::ProcessInput, we convert e.g. "\mbox"
    // into "\mboxReserved". Then, the MacroProcessor object sees e.g.
    // "\mboxReserved A" and converts it to "\mbox{A}". This simplifies
    // things enormously for the parser, since now it can treat "\mbox"
    // and "\hbox" in the same way. ("\hbox" requires braces around its
    // argument, even if it's just a single character.) This strategy also
    // keeps TeX happy when we send off the purified TeX, since TeX doesn't
    // care about the extra braces.

    L"\\newcommand{\\mboxReserved}     [1]{\\mbox{#1}}"
    L"\\newcommand{\\substackReserved} [1]{\\substack{#1}}"
    L"\\newcommand{\\oversetReserved}  [2]{\\overset{#1}{#2}}"
    L"\\newcommand{\\undersetReserved} [2]{\\underset{#1}{#2}}"

    // The following are all similar, but they get extra "safety braces"
    // placed around them. For example, "x^\frac yz" is legal, because it
    // becomes "x^{y \over z}".

    L"\\newcommand{\\textReserved}     [1]{{\\text{#1}}}"
    L"\\newcommand{\\textitReserved}   [1]{{\\textit{#1}}}"
    L"\\newcommand{\\textrmReserved}   [1]{{\\textrm{#1}}}"
    L"\\newcommand{\\textbfReserved}   [1]{{\\textbf{#1}}}"
    L"\\newcommand{\\textsfReserved}   [1]{{\\textsf{#1}}}"
    L"\\newcommand{\\textttReserved}   [1]{{\\texttt{#1}}}"
    L"\\newcommand{\\emphReserved}     [1]{{\\emph{#1}}}"
    L"\\newcommand{\\fracReserved}     [2]{{\\frac{#1}{#2}}}"
    L"\\newcommand{\\mathrmReserved}   [1]{{\\mathrm{#1}}}"
    L"\\newcommand{\\mathbfReserved}   [1]{{\\mathbf{#1}}}"
    L"\\newcommand{\\mathbbReserved}   [1]{{\\mathbb{#1}}}"
    L"\\newcommand{\\mathitReserved}   [1]{{\\mathit{#1}}}"
    L"\\newcommand{\\mathcalReserved}  [1]{{\\mathcal{#1}}}"
    L"\\newcommand{\\mathfrakReserved} [1]{{\\mathfrak{#1}}}"
    L"\\newcommand{\\mathttReserved}   [1]{{\\mathtt{#1}}}"
    L"\\newcommand{\\mathsfReserved}   [1]{{\\mathsf{#1}}}"
    L"\\newcommand{\\bigReserved}      [1]{{\\big#1}}"
    L"\\newcommand{\\biggReserved}     [1]{{\\bigg#1}}"
    L"\\newcommand{\\BigReserved}      [1]{{\\Big#1}}"
    L"\\newcommand{\\BiggReserved}     [1]{{\\Bigg#1}}"

    L"\\newcommand{\\japReserved}     [1]{{\\jap{#1}}}"
    L"\\newcommand{\\cyrReserved}     [1]{{\\cyr{#1}}}"
;

vector<Token> Manager::gStandardMacrosTokenised;
vector<Token> Manager::gTexvcCompatibilityMacrosTokenised;

Manager::Manager()
{
    if (sizeof(RGBColour) != 4)
        throw runtime_error("The \"unsigned\" type is not 4 bytes wide!");

    // Tokenise the standard macros if it hasn't been done already.

#warning TODO: this throws off token ranges
//    if (gTexvcCompatibilityMacrosTokenised.empty())
//        Tokenise(
//            gTexvcCompatibilityMacros,
//            gTexvcCompatibilityMacrosTokenised
//        );
//
//    if (gStandardMacrosTokenised.empty())
//        Tokenise(gStandardMacros, gStandardMacrosTokenised);

    mStrictSpacingRequested = false;
}

void Manager::ProcessInput(const wstring& input, bool texvcCompatibility, bool displayStyle)
{
    // Here are all the commands which get "Reserved" tacked on the end
    // before the MacroProcessor sees them:

    static wstring reservedCommandArray[] =
    {
        L"\\sqrt",
        L"\\mbox",
        L"\\text",
        L"\\textit",
        L"\\textrm",
        L"\\textbf",
        L"\\textsf",
        L"\\texttt",
        L"\\jap",
        L"\\cyr",
        L"\\emph",
        L"\\frac",
        L"\\mathrm",
        L"\\mathbf",
        L"\\mathbb",
        L"\\mathit",
        L"\\mathcal",
        L"\\mathfrak",
        L"\\mathtt",
        L"\\mathsf",
        L"\\big",
        L"\\bigg",
        L"\\Big",
        L"\\Bigg",
        L"\\overset",
        L"\\underset",
        L"\\substack"
    };
    static wishful_hash_set<wstring> reservedCommandTable(
        reservedCommandArray,
        END_ARRAY(reservedCommandArray)
    );

    vector<Token> inputTokens;
    Tokenise(input, inputTokens);

    mStrictSpacingRequested = false;

    // Check that the user hasn't supplied any input directly containing the
    // "Reserved" suffix, and add Reserved suffixes appropriately.
    //
    // Also search for magic commands (currently the only magic command is
    // "\strictspacing")
    for (vector<Token>::iterator
        ptr = inputTokens.begin();
        ptr != inputTokens.end();
        ptr++
    )
    {
		wstring value = (*ptr).getValue();

        if (reservedCommandTable.count(value))
			(*ptr).setValue(value + L"Reserved");

        else if (value.size() >= 8 && value.substr(value.size() - 8, 8) == L"Reserved")
            throw Exception(L"ReservedCommand", value);

        else if (value == L"\\strictspacing")
        {
            mStrictSpacingRequested = true;
            (*ptr).setValue(L" ");
        }
    }

    vector<Token> tokens;

    // Append the texvc-compatibility and standard macros where appropriate.

    if (texvcCompatibility)
        tokens = gTexvcCompatibilityMacrosTokenised;

    copy(
        gStandardMacrosTokenised.begin(),
        gStandardMacrosTokenised.end(),
        back_inserter(tokens)
    );
    copy(inputTokens.begin(), inputTokens.end(), back_inserter(tokens));

    // Generate the parse tree and the layout tree.
    Parser P;
    mParseTree = P.DoParse(tokens);
    mHasDelayedMathmlError = false;
    
    try
    {
        TexProcessingState topState;
        topState.mStyle = displayStyle ? LayoutTree::Node::cStyleDisplay : LayoutTree::Node::cStyleText;
        topState.mColour = 0;
        mLayoutTree = mParseTree->BuildLayoutTree(topState);
        mLayoutTree->Optimise();
    }
    catch (Exception& e)
    {
        // Some types of error need to returned as MathML errors, not
        // parsing errors.
        if (e.GetCode() == L"UnavailableSymbolFontCombination")
        {
            mHasDelayedMathmlError = true;
            mDelayedMathmlError = e;
            mLayoutTree.reset(NULL);
        }
        else
            throw e;
    }
}


auto_ptr<MathmlNode> Manager::GenerateMathml(
    const MathmlOptions& options
) const
{
    if (mHasDelayedMathmlError)
        throw mDelayedMathmlError;
    
    if (!mLayoutTree.get())
        throw logic_error(
            "Layout tree not yet built in Manager::GenerateMathml"
        );

    MathmlOptions optionsCopy = options;
    if (mStrictSpacingRequested)
        // Override the spacing control setting if the "\strictspacing"
        // command appeared somewhere in the input.
        optionsCopy.mSpacingControl = MathmlOptions::cSpacingControlStrict;

    // Build the MathML tree. The nodeCount variables counts the number
    // of nodes being generated; if too many appear, an exception is thrown.
    unsigned nodeCount = 0;
    auto_ptr<MathmlNode> root = mLayoutTree->BuildMathmlTree(
        optionsCopy,
        MathmlEnvironment(LayoutTree::Node::cStyleText, RGBColour(0)),
        nodeCount
    );

    return root;
}


wstring Manager::GeneratePurifiedTex(
    const PurifiedTexOptions& options
) const
{
    if (!mParseTree.get())
        throw logic_error(
            "Parse tree not yet built in Manager::GeneratePurifiedTex"
        );

    wostringstream os;
    LatexFeatures features;
    mParseTree->GetPurifiedTex(os, features, cFontEncodingDefault);
    wstring latex = os.str();
    
    if (features.mNeedsX2 || features.mNeedsCJK)
    {
        features.mNeedsUcs = true;
        features.mNeedsAmsmath = true;      // for the "\text" command
    }
    
    // Generate purified tex output

    wostringstream output;
    
    output << 
        L"\\nonstopmode\n"
        L"\\documentclass[12pt]{article}\n";
    
    if (features.mNeedsAmsmath)
        output << L"\\usepackage{amsmath}\n";
    if (features.mNeedsAmsfonts)
        output << L"\\usepackage{amsfonts}\n";
    if (features.mNeedsAmssymb)
        output << L"\\usepackage{amssymb}\n";
    if (features.mNeedsColor)
        output << L"\\usepackage[dvips,usenames]{color}\n";

    if (features.mNeedsUcs)
    {
        if (!options.mAllowUcs)
            throw Exception(L"LatexPackageUnavailable", L"ucs");
            
        output << L"\\usepackage[utf8x]{inputenc}\n";
    }

    if (features.mNeedsX2)
        output <<
            L"\\usepackage[X2,T1]{fontenc}\n"
            L"\\newcommand{\\cyr}[1]{\\text{"
            L"\\bgroup\\fontencoding{X2}\\selectfont #1\\egroup}}\n";

    if (features.mNeedsCJK)
    {
        if (!options.mAllowCJK)
            throw Exception(L"LatexPackageUnavailable", L"CJK");
            
        output << L"\\usepackage{CJK}\n";

        if (features.mNeedsJapaneseFont)
        {
            if (options.mJapaneseFont.empty())
                throw Exception(L"LatexFontNotSpecified", L"japanese");
            
            output
                << L"\\newcommand{\\jap}[1]{\\text{\\begin{CJK}{UTF8}{"
                << options.mJapaneseFont
                << L"}#1\\end{CJK}}}\n";
        }
    }

    if (options.mAllowPreview)
        output << L"\\usepackage[active]{preview}\n";
    else
        output << L"\\pagestyle{empty}\n";

    if (options.mLaTeXPreamble.length() > 0)
        output << options.mLaTeXPreamble << "\n";

    output << L"\\begin{document}\n";

    if (options.mLaTeXBeforeMath.length() > 0)
        output << options.mLaTeXBeforeMath << "\n";

    if (options.mAllowPreview)
        output << L"\\begin{preview}\n";

    if (options.mDisplayMath)
        output << L"\\[\n" << latex << L"\n\\]\n";
    else
        output << L"$\n" << latex << L"\n$\n";

    if (options.mAllowPreview)
        output << L"\\end{preview}\n";

    output << L"\\end{document}\n";

    return output.str();
}

wstring Manager::GeneratePurifiedTexOnly() const
{
    if (!mParseTree.get())
        throw logic_error(
            "Parse tree not yet built in Manager::GeneratePurifiedTex"
        );

    wostringstream os;
    LatexFeatures features;
    mParseTree->GetPurifiedTex(os, features, cFontEncodingDefault);
    wstring temp = os.str();
    wstring result;
    bool opening = true;
    for(unsigned int i=0; i<temp.size(); i++) {
        if ((!opening) || (temp[i] != ' '))
            result += temp[i];
        opening = (temp[i] == '{');
    }
    return result;
}

}

// end of file @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

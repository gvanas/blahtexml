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

#include <iomanip>
#include <sstream>
#include <map>
#include <stdint.h>
#include "XmlEncode.h"

using namespace std;

namespace blahtex
{

struct UnicodeNameInfo
{
    wstring mShortName;
    wstring mLongName;

    UnicodeNameInfo()
    { }

    UnicodeNameInfo(
        const wstring& shortName
    ) :
        mShortName(shortName)
    { }

    UnicodeNameInfo(
        const wstring& shortName,
        const wstring& longName
    ) :
        mShortName(shortName),
        mLongName(longName)
    { }
};

// This table lists all the non-ASCII characters that blahtex can give
// names to. For each one it possibly lists a short and long MathML name.
pair<uint32_t, UnicodeNameInfo> gUnicodeNameArray[] =
{
    make_pair(0x00000060, UnicodeNameInfo(L"grave", L"DiacriticalGrave")),
    make_pair(0x000000A0, UnicodeNameInfo(L"nbsp", L"NonBreakingSpace")),
    make_pair(0x000000A5, UnicodeNameInfo(L"yen")),
    make_pair(0x000000A7, UnicodeNameInfo(L"sect")),
    make_pair(0x000000AC, UnicodeNameInfo(L"not")),
    make_pair(0x000000AE, UnicodeNameInfo(L"reg", L"circledR")),
    make_pair(0x000000AF, UnicodeNameInfo(L"macr", L"OverBar")),
    make_pair(0x000000B1, UnicodeNameInfo(L"pm", L"PlusMinus")),
    make_pair(0x000000B4, UnicodeNameInfo(L"acute", L"DiacriticalAcute")),
    make_pair(0x000000B6, UnicodeNameInfo(L"para")),
    make_pair(0x000000B7, UnicodeNameInfo(L"middot", L"CenterDot")),
    make_pair(0x000000C5, UnicodeNameInfo(L"Aring")),
    make_pair(0x000000D7, UnicodeNameInfo(L"times")),
    make_pair(0x000000D8, UnicodeNameInfo(L"Oslash")),
    make_pair(0x000000E5, UnicodeNameInfo(L"aring")),
    make_pair(0x000000F0, UnicodeNameInfo(L"eth")),
    make_pair(0x000000F7, UnicodeNameInfo(L"div", L"divide")),
    make_pair(0x00000127, UnicodeNameInfo(L"hstrok")),
    make_pair(0x00000131, UnicodeNameInfo(L"imath")),
    make_pair(0x000002C7, UnicodeNameInfo(L"caron", L"Hacek")),
    make_pair(0x000002D8, UnicodeNameInfo(L"breve", L"Breve")),
    make_pair(0x000002DC, UnicodeNameInfo(L"tilde", L"DiacriticalTilde")),
    make_pair(0x00000338, UnicodeNameInfo()),     // FIX: combining character that needs some thought
    make_pair(0x00000393, UnicodeNameInfo(L"Gamma")),
    make_pair(0x00000394, UnicodeNameInfo(L"Delta")),
    make_pair(0x00000398, UnicodeNameInfo(L"Theta")),
    make_pair(0x0000039B, UnicodeNameInfo(L"Lambda")),
    make_pair(0x0000039E, UnicodeNameInfo(L"Xi")),
    make_pair(0x000003A0, UnicodeNameInfo(L"Pi")),
    make_pair(0x000003A3, UnicodeNameInfo(L"Sigma")),
    make_pair(0x000003A5, UnicodeNameInfo(L"Upsilon")),
    make_pair(0x000003A6, UnicodeNameInfo(L"Phi")),
    make_pair(0x000003A8, UnicodeNameInfo(L"Psi")),
    make_pair(0x000003A9, UnicodeNameInfo(L"Omega")),
    make_pair(0x000003B1, UnicodeNameInfo(L"alpha")),
    make_pair(0x000003B2, UnicodeNameInfo(L"beta")),
    make_pair(0x000003B3, UnicodeNameInfo(L"gamma")),
    make_pair(0x000003B4, UnicodeNameInfo(L"delta")),
    make_pair(0x000003B5, UnicodeNameInfo(L"epsiv", L"varepsilon")),
    make_pair(0x000003B6, UnicodeNameInfo(L"zeta")),
    make_pair(0x000003B7, UnicodeNameInfo(L"eta")),
    make_pair(0x000003B8, UnicodeNameInfo(L"theta")),
    make_pair(0x000003B9, UnicodeNameInfo(L"iota")),
    make_pair(0x000003BA, UnicodeNameInfo(L"kappa")),
    make_pair(0x000003BB, UnicodeNameInfo(L"lambda")),
    make_pair(0x000003BC, UnicodeNameInfo(L"mu")),
    make_pair(0x000003BD, UnicodeNameInfo(L"nu")),
    make_pair(0x000003BE, UnicodeNameInfo(L"xi")),
    make_pair(0x000003C0, UnicodeNameInfo(L"pi")),
    make_pair(0x000003C1, UnicodeNameInfo(L"rho")),
    make_pair(0x000003C2, UnicodeNameInfo(L"sigmav", L"varsigma")),
    make_pair(0x000003C3, UnicodeNameInfo(L"sigma")),
    make_pair(0x000003C4, UnicodeNameInfo(L"tau")),
    make_pair(0x000003C5, UnicodeNameInfo(L"upsi", L"upsilon")),
#if 0
    // FIX: note Firefox 1.5 has &phi; and &varphi; around the wrong
    // way, so better to stick with numeric codes for 0x3C6 and 0x3D5.
    // See mozilla bug 321438.
    make_pair(0x000003C6, UnicodeNameInfo(L"phiv", L"varphi")),
    make_pair(0x000003D5, UnicodeNameInfo(L"phi", L"straightphi")),
#endif
    make_pair(0x000003C7, UnicodeNameInfo(L"chi")),
    make_pair(0x000003C8, UnicodeNameInfo(L"psi")),
    make_pair(0x000003C9, UnicodeNameInfo(L"omega")),
    make_pair(0x000003D1, UnicodeNameInfo(L"thetav", L"vartheta")),
    make_pair(0x000003D6, UnicodeNameInfo(L"piv", L"varpi")),
    make_pair(0x000003DD, UnicodeNameInfo(L"gammad", L"digamma")),
    make_pair(0x000003F0, UnicodeNameInfo(L"kappav", L"varkappa")),
    make_pair(0x000003F1, UnicodeNameInfo(L"rhov", L"varrho")),
    make_pair(0x000003F5, UnicodeNameInfo(L"epsi", L"straightepsilon")),
    make_pair(0x000003F6, UnicodeNameInfo(L"bepsi", L"backepsilon")),
    make_pair(0x00002020, UnicodeNameInfo(L"dagger")),
    make_pair(0x00002021, UnicodeNameInfo(L"Dagger", L"ddagger")),
    make_pair(0x00002022, UnicodeNameInfo(L"bull", L"bullet")),
    make_pair(0x00002026, UnicodeNameInfo(L"hellip")),
    make_pair(0x00002032, UnicodeNameInfo(L"prime")),
    make_pair(0x00002035, UnicodeNameInfo(L"bprime", L"backprime")),
    make_pair(0x00002102, UnicodeNameInfo(L"Copf", L"complexes")),
    make_pair(0x0000210B, UnicodeNameInfo(L"Hscr", L"HilbertSpace")),
    make_pair(0x0000210C, UnicodeNameInfo(L"Hfr", L"Poincareplane")),
    make_pair(0x0000210D, UnicodeNameInfo(L"Hopf", L"quaternions")),
    make_pair(0x0000210F, UnicodeNameInfo(L"hbar", L"planck")),
    make_pair(0x00002110, UnicodeNameInfo(L"Iscr", L"imagline")),
    make_pair(0x00002111, UnicodeNameInfo(L"Im", L"imagpart")),
    make_pair(0x00002112, UnicodeNameInfo(L"Lscr", L"Laplacetrf")),
    make_pair(0x00002113, UnicodeNameInfo(L"ell")),
    make_pair(0x00002118, UnicodeNameInfo(L"wp", L"weierp")),
    make_pair(0x00002119, UnicodeNameInfo(L"Popf", L"primes")),
    make_pair(0x0000211A, UnicodeNameInfo(L"Qopf", L"rationals")),
    make_pair(0x0000211B, UnicodeNameInfo(L"Rscr", L"realine")),
    make_pair(0x0000211C, UnicodeNameInfo(L"Re", L"realpart")),
    make_pair(0x0000211D, UnicodeNameInfo(L"Ropf", L"reals")),
    make_pair(0x00002124, UnicodeNameInfo(L"Zopf", L"integers")),
    make_pair(0x00002127, UnicodeNameInfo(L"mho")),
    make_pair(0x00002128, UnicodeNameInfo(L"Zfr", L"zeetrf")),
    make_pair(0x0000212C, UnicodeNameInfo(L"Bscr", L"Bernoullis")),
    make_pair(0x0000212D, UnicodeNameInfo(L"Cfr", L"Cayleys")),
    make_pair(0x00002130, UnicodeNameInfo(L"Escr", L"expectation")),
    make_pair(0x00002131, UnicodeNameInfo(L"Fscr", L"Fouriertrf")),
    make_pair(0x00002133, UnicodeNameInfo(L"Mscr", L"Mellintrf")),
    make_pair(0x00002135, UnicodeNameInfo(L"aleph")),
    make_pair(0x00002136, UnicodeNameInfo(L"beth")),
    make_pair(0x00002137, UnicodeNameInfo(L"gimel")),
    make_pair(0x00002138, UnicodeNameInfo(L"daleth")),
    make_pair(0x00002190, UnicodeNameInfo(L"larr", L"LeftArrow")),
    make_pair(0x00002191, UnicodeNameInfo(L"uarr", L"UpArrow")),
    make_pair(0x00002192, UnicodeNameInfo(L"rarr", L"RightArrow")),
    make_pair(0x00002193, UnicodeNameInfo(L"darr", L"DownArrow")),
    make_pair(0x00002194, UnicodeNameInfo(L"harr", L"LeftRightArrow")),
    make_pair(0x00002195, UnicodeNameInfo(L"varr", L"UpDownArrow")),
    make_pair(0x00002196, UnicodeNameInfo(L"nwarr", L"UpperLeftArrow")),
    make_pair(0x00002197, UnicodeNameInfo(L"nearr", L"UpperRightArrow")),
    make_pair(0x00002198, UnicodeNameInfo(L"searr", L"LowerRightArrow")),
    make_pair(0x00002199, UnicodeNameInfo(L"swarr", L"LowerLeftArrow")),
    make_pair(0x0000219A, UnicodeNameInfo(L"nlarr", L"nleftarrow")),
    make_pair(0x0000219B, UnicodeNameInfo(L"nrarr", L"nrightarrow")),
    make_pair(0x0000219D, UnicodeNameInfo(L"rarrw", L"rightsquigarrow")),
    make_pair(0x0000219E, UnicodeNameInfo(L"Larr", L"twoheadleftarrow")),
    make_pair(0x000021A0, UnicodeNameInfo(L"Rarr", L"twoheadrightarrow")),
    make_pair(0x000021A2, UnicodeNameInfo(L"larrtl", L"leftarrowtail")),
    make_pair(0x000021A3, UnicodeNameInfo(L"rarrtl", L"rightarrowtail")),
    make_pair(0x000021A6, UnicodeNameInfo(L"map", L"RightTeeArrow")),
    make_pair(0x000021A9, UnicodeNameInfo(L"larrhk", L"hookleftarrow")),
    make_pair(0x000021AA, UnicodeNameInfo(L"rarrhk", L"hookrightarrow")),
    make_pair(0x000021AB, UnicodeNameInfo(L"larrlp", L"looparrowleft")),
    make_pair(0x000021AC, UnicodeNameInfo(L"rarrlp", L"looparrowright")),
    make_pair(0x000021AD, UnicodeNameInfo(L"harrw", L"leftrightsquigarrow")),
    make_pair(0x000021AE, UnicodeNameInfo(L"nharr", L"nleftrightarrow")),
    make_pair(0x000021B0, UnicodeNameInfo(L"lsh", L"Lsh")),
    make_pair(0x000021B1, UnicodeNameInfo(L"rsh", L"Rsh")),
    make_pair(0x000021B6, UnicodeNameInfo(L"cularr", L"curvearrowleft")),
    make_pair(0x000021B7, UnicodeNameInfo(L"curarr", L"curvearrowright")),
    make_pair(0x000021BA, UnicodeNameInfo(L"olarr", L"circlearrowleft")),
    make_pair(0x000021BB, UnicodeNameInfo(L"orarr", L"circlearrowright")),
    make_pair(0x000021BC, UnicodeNameInfo(L"lharu", L"leftharpoonup")),
    make_pair(0x000021BD, UnicodeNameInfo(L"lhard", L"leftharpoondown")),
    make_pair(0x000021BE, UnicodeNameInfo(L"uharr", L"upharpoonright")),
    make_pair(0x000021BF, UnicodeNameInfo(L"uharl", L"upharpoonleft")),
    make_pair(0x000021C0, UnicodeNameInfo(L"rharu", L"rightharpoonup")),
    make_pair(0x000021C1, UnicodeNameInfo(L"rhard", L"rightharpoondown")),
    make_pair(0x000021C2, UnicodeNameInfo(L"dharr", L"downharpoonright")),
    make_pair(0x000021C3, UnicodeNameInfo(L"dharl", L"downharpoonleft")),
    make_pair(0x000021C4, UnicodeNameInfo(L"rlarr", L"RightArrowLeftArrow")),
    make_pair(0x000021C6, UnicodeNameInfo(L"lrarr", L"LeftArrowRightArrow")),
    make_pair(0x000021C7, UnicodeNameInfo(L"llarr", L"leftleftarrows")),
    make_pair(0x000021C8, UnicodeNameInfo(L"uuarr", L"upuparrows")),
    make_pair(0x000021C9, UnicodeNameInfo(L"rrarr", L"rightrightarrows")),
    make_pair(0x000021CA, UnicodeNameInfo(L"ddarr", L"downdownarrows")),
    make_pair(0x000021CB, UnicodeNameInfo(L"lrhar", L"ReverseEquilibrium")),
    make_pair(0x000021CC, UnicodeNameInfo(L"rlhar", L"Equilibrium")),
    make_pair(0x000021CD, UnicodeNameInfo(L"nlArr", L"nLeftarrow")),
    make_pair(0x000021CE, UnicodeNameInfo(L"nhArr", L"nLeftrightarrow")),
    make_pair(0x000021CF, UnicodeNameInfo(L"nrArr", L"nRightarrow")),
    make_pair(0x000021D0, UnicodeNameInfo(L"lArr", L"DoubleLeftArrow")),
    make_pair(0x000021D1, UnicodeNameInfo(L"uArr", L"DoubleUpArrow")),
    make_pair(0x000021D2, UnicodeNameInfo(L"rArr", L"DoubleRightArrow")),
    make_pair(0x000021D3, UnicodeNameInfo(L"dArr", L"DoubleDownArrow")),
    make_pair(0x000021D4, UnicodeNameInfo(L"hArr", L"DoubleLeftRightArrow")),
    make_pair(0x000021D5, UnicodeNameInfo(L"vArr", L"DoubleUpDownArrow")),
    make_pair(0x000021DA, UnicodeNameInfo(L"lAarr", L"Lleftarrow")),
    make_pair(0x000021DB, UnicodeNameInfo(L"rAarr", L"Rrightarrow")),
    make_pair(0x000021DD, UnicodeNameInfo(L"zigrarr")),
    make_pair(0x00002200, UnicodeNameInfo(L"forall", L"ForAll")),
    make_pair(0x00002201, UnicodeNameInfo(L"comp", L"complement")),
    make_pair(0x00002202, UnicodeNameInfo(L"part", L"PartialD")),
    make_pair(0x00002203, UnicodeNameInfo(L"exist", L"Exists")),
    make_pair(0x00002204, UnicodeNameInfo(L"nexist", L"NotExists")),
    make_pair(0x00002205, UnicodeNameInfo(L"empty", L"emptyset")),
    make_pair(0x00002207, UnicodeNameInfo(L"nabla", L"Del")),
    make_pair(0x00002208, UnicodeNameInfo(L"in", L"Element")),
    make_pair(0x00002209, UnicodeNameInfo(L"notin", L"NotElement")),
    make_pair(0x0000220B, UnicodeNameInfo(L"ni", L"ReverseElement")),
    make_pair(0x0000220C, UnicodeNameInfo(L"notni", L"NotReverseElement")),
    make_pair(0x0000220F, UnicodeNameInfo(L"prod", L"Product")),
    make_pair(0x00002210, UnicodeNameInfo(L"coprod", L"Coproduct")),
    make_pair(0x00002211, UnicodeNameInfo(L"sum", L"Sum")),
    make_pair(0x00002213, UnicodeNameInfo(L"mp", L"MinusPlus")),
    make_pair(0x00002214, UnicodeNameInfo(L"dotplus")),
    make_pair(0x00002216, UnicodeNameInfo(L"setmn", L"Backslash")),
    make_pair(0x00002218, UnicodeNameInfo(L"compfn", L"SmallCircle")),
    make_pair(0x0000221A, UnicodeNameInfo(L"radic", L"Sqrt")),
    make_pair(0x0000221D, UnicodeNameInfo(L"prop", L"Proportional")),
    make_pair(0x0000221E, UnicodeNameInfo(L"infin")),
    make_pair(0x00002220, UnicodeNameInfo(L"ang", L"angle")),
    make_pair(0x00002221, UnicodeNameInfo(L"angmsd", L"measuredangle")),
    make_pair(0x00002222, UnicodeNameInfo(L"angsph")),
    make_pair(0x00002223, UnicodeNameInfo(L"mid", L"VerticalBar")),
    make_pair(0x00002224, UnicodeNameInfo(L"nmid", L"NotVerticalBar")),
    make_pair(0x00002225, UnicodeNameInfo(L"par", L"DoubleVerticalBar")),
    make_pair(0x00002226, UnicodeNameInfo(L"npar", L"NotDoubleVerticalBar")),
    make_pair(0x00002227, UnicodeNameInfo(L"and", L"wedge")),
    make_pair(0x00002228, UnicodeNameInfo(L"or", L"vee")),
    make_pair(0x00002229, UnicodeNameInfo(L"cap")),
    make_pair(0x0000222A, UnicodeNameInfo(L"cup")),
    make_pair(0x0000222B, UnicodeNameInfo(L"int", L"Integral")),
    make_pair(0x0000222C, UnicodeNameInfo(L"Int")),
    make_pair(0x0000222D, UnicodeNameInfo(L"tint", L"iiint")),
    make_pair(0x0000222E, UnicodeNameInfo(L"conint", L"ContourIntegral")),
    make_pair(0x00002234, UnicodeNameInfo(L"there4", L"Therefore")),
    make_pair(0x00002235, UnicodeNameInfo(L"becaus", L"Because")),
    make_pair(0x0000223C, UnicodeNameInfo(L"sim", L"Tilde")),
    make_pair(0x0000223D, UnicodeNameInfo(L"bsim", L"backsim")),
    make_pair(0x00002240, UnicodeNameInfo(L"wr", L"VerticalTilde")),
    make_pair(0x00002241, UnicodeNameInfo(L"nsim", L"NotTilde")),
    make_pair(0x00002242, UnicodeNameInfo(L"esim", L"EqualTilde")),
    make_pair(0x00002243, UnicodeNameInfo(L"sime", L"TildeEqual")),
    make_pair(0x00002244, UnicodeNameInfo(L"nsime", L"NotTildeEqual")),
    make_pair(0x00002245, UnicodeNameInfo(L"cong", L"TildeFullEqual")),
    make_pair(0x00002247, UnicodeNameInfo(L"ncong", L"NotTildeFullEqual")),
    make_pair(0x00002248, UnicodeNameInfo(L"ap", L"TildeTilde")),
    make_pair(0x00002249, UnicodeNameInfo(L"nap", L"NotTildeTilde")),
    make_pair(0x0000224A, UnicodeNameInfo(L"ape", L"approxeq")),
    make_pair(0x0000224E, UnicodeNameInfo(L"bump", L"HumpDownHump")),
    make_pair(0x0000224F, UnicodeNameInfo(L"nbump", L"NotHumpDownHump")),
    make_pair(0x00002250, UnicodeNameInfo(L"esdot", L"DotEqual")),
    make_pair(0x00002251, UnicodeNameInfo(L"eDot", L"doteqdot")),
    make_pair(0x00002252, UnicodeNameInfo(L"efDot", L"fallingdotseq")),
    make_pair(0x00002253, UnicodeNameInfo(L"erDot", L"risingdotseq")),
    make_pair(0x00002256, UnicodeNameInfo(L"ecir", L"eqcirc")),
    make_pair(0x00002257, UnicodeNameInfo(L"cire", L"circeq")),
    make_pair(0x0000225C, UnicodeNameInfo(L"trie", L"triangleq")),
    make_pair(0x00002260, UnicodeNameInfo(L"ne", L"NotEqual")),
    make_pair(0x00002261, UnicodeNameInfo(L"equiv", L"Congruent")),
    make_pair(0x00002262, UnicodeNameInfo(L"nequiv", L"NotCongruent")),
    make_pair(0x00002264, UnicodeNameInfo(L"le", L"leq")),
    make_pair(0x00002265, UnicodeNameInfo(L"ge", L"GreaterEqual")),
    make_pair(0x00002266, UnicodeNameInfo(L"lE", L"LessFullEqual")),
    make_pair(0x00002267, UnicodeNameInfo(L"gE", L"GreaterFullEqual")),
    make_pair(0x00002268, UnicodeNameInfo(L"lnE", L"lneqq")),
    make_pair(0x00002269, UnicodeNameInfo(L"gnE", L"gneqq")),
    make_pair(0x0000226A, UnicodeNameInfo(L"Lt", L"NestedLessLess")),
    make_pair(0x0000226B, UnicodeNameInfo(L"Gt", L"NestedGreaterGreater")),
    make_pair(0x0000226C, UnicodeNameInfo(L"twixt", L"between")),
    make_pair(0x0000226E, UnicodeNameInfo(L"nlt", L"NotLess")),
    make_pair(0x0000226F, UnicodeNameInfo(L"ngt", L"NotGreater")),
    make_pair(0x00002270, UnicodeNameInfo(L"nle", L"NotLessEqual")),
    make_pair(0x00002271, UnicodeNameInfo(L"nge", L"NotGreaterEqual")),
    make_pair(0x00002272, UnicodeNameInfo(L"lsim", L"LessTilde")),
    make_pair(0x00002273, UnicodeNameInfo(L"gsim", L"GreaterTilde")),
    make_pair(0x00002276, UnicodeNameInfo(L"lg", L"LessGreater")),
    make_pair(0x00002277, UnicodeNameInfo(L"gl", L"GreaterLess")),
    make_pair(0x0000227A, UnicodeNameInfo(L"pr", L"Precedes")),
    make_pair(0x0000227B, UnicodeNameInfo(L"sc", L"Succeeds")),
    make_pair(0x0000227C, UnicodeNameInfo(L"prcue", L"PrecedesSlantEqual")),
    make_pair(0x0000227D, UnicodeNameInfo(L"sccue", L"SucceedsSlantEqual")),
    make_pair(0x0000227E, UnicodeNameInfo(L"prsim", L"PrecedesTilde")),
    make_pair(0x0000227F, UnicodeNameInfo(L"scsim", L"SucceedsTilde")),
    make_pair(0x00002280, UnicodeNameInfo(L"npr", L"NotPrecedes")),
    make_pair(0x00002281, UnicodeNameInfo(L"nsc", L"NotSucceeds")),
    make_pair(0x00002282, UnicodeNameInfo(L"sub", L"subset")),
    make_pair(0x00002283, UnicodeNameInfo(L"sup", L"supset")),
    make_pair(0x00002284, UnicodeNameInfo(L"nsub")),
    make_pair(0x00002285, UnicodeNameInfo(L"nsup")),
    make_pair(0x00002286, UnicodeNameInfo(L"sube", L"SubsetEqual")),
    make_pair(0x00002287, UnicodeNameInfo(L"supe", L"SupersetEqual")),
    make_pair(0x00002288, UnicodeNameInfo(L"nsube", L"NotSubsetEqual")),
    make_pair(0x00002289, UnicodeNameInfo(L"nsupe", L"NotSupersetEqual")),
    make_pair(0x0000228A, UnicodeNameInfo(L"subne", L"subsetneq")),
    make_pair(0x0000228B, UnicodeNameInfo(L"supne", L"supsetneq")),
    make_pair(0x0000228E, UnicodeNameInfo(L"uplus", L"UnionPlus")),
    make_pair(0x0000228F, UnicodeNameInfo(L"sqsub", L"SquareSubset")),
    make_pair(0x00002290, UnicodeNameInfo(L"sqsup", L"SquareSuperset")),
    make_pair(0x00002291, UnicodeNameInfo(L"sqsube", L"SquareSubsetEqual")),
    make_pair(0x00002292, UnicodeNameInfo(L"sqsupe", L"SquareSupersetEqual")),
    make_pair(0x00002293, UnicodeNameInfo(L"sqcap", L"SquareIntersection")),
    make_pair(0x00002294, UnicodeNameInfo(L"sqcup", L"SquareUnion")),
    make_pair(0x00002295, UnicodeNameInfo(L"oplus", L"CirclePlus")),
    make_pair(0x00002296, UnicodeNameInfo(L"ominus", L"CircleMinus")),
    make_pair(0x00002297, UnicodeNameInfo(L"otimes", L"CircleTimes")),
    make_pair(0x00002298, UnicodeNameInfo(L"osol")),
    make_pair(0x00002299, UnicodeNameInfo(L"odot", L"CircleDot")),
    make_pair(0x0000229A, UnicodeNameInfo(L"ocir", L"circledcirc")),
    make_pair(0x0000229B, UnicodeNameInfo(L"oast", L"circledast")),
    make_pair(0x0000229D, UnicodeNameInfo(L"odash", L"circleddash")),
    make_pair(0x0000229E, UnicodeNameInfo(L"plusb", L"boxplus")),
    make_pair(0x0000229F, UnicodeNameInfo(L"minusb", L"boxminus")),
    make_pair(0x000022A0, UnicodeNameInfo(L"timesb", L"boxtimes")),
    make_pair(0x000022A1, UnicodeNameInfo(L"sdotb", L"dotsquare")),
    make_pair(0x000022A2, UnicodeNameInfo(L"vdash", L"RightTee")),
    make_pair(0x000022A3, UnicodeNameInfo(L"dashv", L"LeftTee")),
    make_pair(0x000022A4, UnicodeNameInfo(L"top", L"DownTee")),
    make_pair(0x000022A5, UnicodeNameInfo(L"bot", L"UpTee")),
    make_pair(0x000022A7, UnicodeNameInfo(L"models")),
    make_pair(0x000022A8, UnicodeNameInfo(L"vDash", L"DoubleRightTee")),
    make_pair(0x000022A9, UnicodeNameInfo(L"Vdash")),
    make_pair(0x000022AA, UnicodeNameInfo(L"Vvdash")),
    make_pair(0x000022AC, UnicodeNameInfo(L"nvdash")),
    make_pair(0x000022AD, UnicodeNameInfo(L"nvDash")),
    make_pair(0x000022AE, UnicodeNameInfo(L"nVdash")),
    make_pair(0x000022AF, UnicodeNameInfo(L"nVDash")),
    make_pair(0x000022B2, UnicodeNameInfo(L"vltri", L"LeftTriangle")),
    make_pair(0x000022B3, UnicodeNameInfo(L"vrtri", L"RightTriangle")),
    make_pair(0x000022B4, UnicodeNameInfo(L"ltrie", L"LeftTriangleEqual")),
    make_pair(0x000022B5, UnicodeNameInfo(L"rtrie", L"RightTriangleEqual")),
    make_pair(0x000022B8, UnicodeNameInfo(L"mumap", L"multimap")),
    make_pair(0x000022BA, UnicodeNameInfo(L"intcal", L"intercal")),
    make_pair(0x000022BB, UnicodeNameInfo(L"veebar")),
    make_pair(0x000022C0, UnicodeNameInfo(L"xwedge", L"Wedge")),
    make_pair(0x000022C1, UnicodeNameInfo(L"xvee", L"Vee")),
    make_pair(0x000022C2, UnicodeNameInfo(L"xcap", L"Intersection")),
    make_pair(0x000022C3, UnicodeNameInfo(L"xcup", L"Union")),
    make_pair(0x000022C4, UnicodeNameInfo(L"diam", L"Diamond")),
    make_pair(0x000022C5, UnicodeNameInfo(L"sdot")),
    make_pair(0x000022C6, UnicodeNameInfo(L"Star")),
    make_pair(0x000022C7, UnicodeNameInfo(L"divonx", L"divideontimes")),
    make_pair(0x000022C8, UnicodeNameInfo(L"bowtie")),
    make_pair(0x000022C9, UnicodeNameInfo(L"ltimes")),
    make_pair(0x000022CA, UnicodeNameInfo(L"rtimes")),
    make_pair(0x000022CB, UnicodeNameInfo(L"lthree", L"leftthreetimes")),
    make_pair(0x000022CC, UnicodeNameInfo(L"rthree", L"rightthreetimes")),
    make_pair(0x000022CD, UnicodeNameInfo(L"bsime", L"backsimeq")),
    make_pair(0x000022CE, UnicodeNameInfo(L"cuvee", L"curlyvee")),
    make_pair(0x000022CF, UnicodeNameInfo(L"cuwed", L"curlywedge")),
    make_pair(0x000022D0, UnicodeNameInfo(L"Sub", L"Subset")),
    make_pair(0x000022D1, UnicodeNameInfo(L"Sup", L"Supset")),
    make_pair(0x000022D2, UnicodeNameInfo(L"Cap")),
    make_pair(0x000022D3, UnicodeNameInfo(L"Cup")),
    make_pair(0x000022D4, UnicodeNameInfo(L"fork", L"pitchfork")),
    make_pair(0x000022D6, UnicodeNameInfo(L"ltdot", L"lessdot")),
    make_pair(0x000022D7, UnicodeNameInfo(L"gtdot", L"gtrdot")),
    make_pair(0x000022D8, UnicodeNameInfo(L"Ll")),
    make_pair(0x000022D9, UnicodeNameInfo(L"Gg")),
    make_pair(0x000022DA, UnicodeNameInfo(L"leg", L"LessEqualGreater")),
    make_pair(0x000022DB, UnicodeNameInfo(L"gel", L"GreaterEqualLess")),
    make_pair(0x000022DE, UnicodeNameInfo(L"cuepr", L"curlyeqprec")),
    make_pair(0x000022DF, UnicodeNameInfo(L"cuesc", L"curlyeqsucc")),
    make_pair(0x000022E2, UnicodeNameInfo(L"nsqsube", L"NotSquareSubsetEqual")),
    make_pair(0x000022E3, UnicodeNameInfo(L"nsqsupe", L"NotSquareSupersetEqual")),
    make_pair(0x000022E6, UnicodeNameInfo(L"lnsim")),
    make_pair(0x000022E7, UnicodeNameInfo(L"gnsim")),
    make_pair(0x000022E8, UnicodeNameInfo(L"prnsim", L"precnsim")),
    make_pair(0x000022E9, UnicodeNameInfo(L"scnsim", L"succnsim")),
    make_pair(0x000022EA, UnicodeNameInfo(L"nltri", L"NotLeftTriangle")),
    make_pair(0x000022EB, UnicodeNameInfo(L"nrtri", L"NotRightTriangle")),
    make_pair(0x000022EC, UnicodeNameInfo(L"nltrie", L"NotLeftTriangleEqual")),
    make_pair(0x000022ED, UnicodeNameInfo(L"nrtrie", L"NotRightTriangleEqual")),
    make_pair(0x000022EE, UnicodeNameInfo(L"vellip")),
    make_pair(0x000022EF, UnicodeNameInfo(L"ctdot")),
    make_pair(0x000022F1, UnicodeNameInfo(L"dtdot")),
    make_pair(0x00002305, UnicodeNameInfo(L"barwed", L"barwedge")),
    make_pair(0x00002306, UnicodeNameInfo(L"Barwed", L"doublebarwedge")),
    make_pair(0x00002308, UnicodeNameInfo(L"lceil", L"LeftCeiling")),
    make_pair(0x00002309, UnicodeNameInfo(L"rceil", L"RightCeiling")),
    make_pair(0x0000230A, UnicodeNameInfo(L"lfloor", L"LeftFloor")),
    make_pair(0x0000230B, UnicodeNameInfo(L"rfloor", L"RightFloor")),
    make_pair(0x0000231C, UnicodeNameInfo(L"ulcorn", L"ulcorner")),
    make_pair(0x0000231D, UnicodeNameInfo(L"urcorn", L"urcorner")),
    make_pair(0x0000231E, UnicodeNameInfo(L"dlcorn", L"llcorner")),
    make_pair(0x0000231F, UnicodeNameInfo(L"drcorn", L"lrcorner")),
    make_pair(0x00002322, UnicodeNameInfo(L"frown", L"sfrown")),
    make_pair(0x00002323, UnicodeNameInfo(L"smile", L"ssmile")),
    make_pair(0x00002329, UnicodeNameInfo(L"lang", L"LeftAngleBracket")),
    make_pair(0x0000232A, UnicodeNameInfo(L"rang", L"RightAngleBracket")),
    make_pair(0x000023B5, UnicodeNameInfo(L"bbrk", L"UnderBracket")),
    make_pair(0x000024C8, UnicodeNameInfo(L"oS", L"circledS")),
    make_pair(0x000025A1, UnicodeNameInfo(L"squ", L"Square")),
    make_pair(0x000025B3, UnicodeNameInfo(L"xutri", L"bigtriangleup")),
    make_pair(0x000025B4, UnicodeNameInfo(L"utrif", L"blacktriangle")),
    make_pair(0x000025B5, UnicodeNameInfo(L"utri", L"triangle")),
    make_pair(0x000025B6, UnicodeNameInfo()),
    make_pair(0x000025B9, UnicodeNameInfo(L"rtri", L"triangleright")),
    make_pair(0x000025BD, UnicodeNameInfo(L"xdtri", L"bigtriangledown")),
    make_pair(0x000025BE, UnicodeNameInfo(L"dtrif", L"blacktriangledown")),
    make_pair(0x000025BF, UnicodeNameInfo(L"dtri", L"triangledown")),
    make_pair(0x000025C0, UnicodeNameInfo()),
    make_pair(0x000025C3, UnicodeNameInfo(L"ltri", L"triangleleft")),
    make_pair(0x000025CA, UnicodeNameInfo(L"loz", L"lozenge")),
    make_pair(0x000025EF, UnicodeNameInfo(L"xcirc", L"bigcirc")),
    make_pair(0x000025FC, UnicodeNameInfo(L"FilledSmallSquare")),
    make_pair(0x00002605, UnicodeNameInfo(L"starf", L"bigstar")),
    make_pair(0x00002660, UnicodeNameInfo(L"spades", L"spadesuit")),
    make_pair(0x00002663, UnicodeNameInfo(L"clubs", L"clubsuit")),
    make_pair(0x00002665, UnicodeNameInfo(L"hearts", L"heartsuit")),
    make_pair(0x00002666, UnicodeNameInfo(L"diams", L"diamondsuit")),
    make_pair(0x0000266D, UnicodeNameInfo(L"flat")),
    make_pair(0x0000266E, UnicodeNameInfo(L"natur", L"natural")),
    make_pair(0x0000266F, UnicodeNameInfo(L"sharp")),
    make_pair(0x00002713, UnicodeNameInfo(L"check", L"checkmark")),
    make_pair(0x00002720, UnicodeNameInfo(L"malt", L"maltese")),
    make_pair(0x000027F5, UnicodeNameInfo(L"xlarr", L"LongLeftArrow")),
    make_pair(0x000027F6, UnicodeNameInfo(L"xrarr", L"LongRightArrow")),
    make_pair(0x000027F7, UnicodeNameInfo(L"xharr", L"LongLeftRightArrow")),
    make_pair(0x000027F8, UnicodeNameInfo(L"xlArr", L"DoubleLongLeftArrow")),
    make_pair(0x000027F9, UnicodeNameInfo(L"xrArr", L"DoubleLongRightArrow")),
    make_pair(0x000027FA, UnicodeNameInfo(L"xhArr", L"DoubleLongLeftRightArrow")),
    make_pair(0x000027FC, UnicodeNameInfo(L"xmap", L"longMapsto")),
    make_pair(0x0000290E, UnicodeNameInfo(L"lBarr")),
    make_pair(0x0000290F, UnicodeNameInfo(L"rBarr", L"dbkarow")),
    make_pair(0x000029EB, UnicodeNameInfo(L"lozf", L"blacklozenge")),
    make_pair(0x00002A00, UnicodeNameInfo(L"xodot", L"bigodot")),
    make_pair(0x00002A01, UnicodeNameInfo(L"xoplus", L"bigoplus")),
    make_pair(0x00002A02, UnicodeNameInfo(L"xotime", L"bigotimes")),
    make_pair(0x00002A04, UnicodeNameInfo(L"xuplus", L"biguplus")),
    make_pair(0x00002A06, UnicodeNameInfo(L"xsqcup", L"bigsqcup")),
    make_pair(0x00002A0C, UnicodeNameInfo(L"qint", L"iiiint")),
    make_pair(0x00002A2F, UnicodeNameInfo(L"Cross")),
    make_pair(0x00002A3F, UnicodeNameInfo(L"amalg")),
    make_pair(0x00002A7D, UnicodeNameInfo(L"les", L"LessSlantEqual")),
    make_pair(0x00002A7E, UnicodeNameInfo(L"ges", L"GreaterSlantEqual")),
    make_pair(0x00002A85, UnicodeNameInfo(L"lap", L"lessapprox")),
    make_pair(0x00002A86, UnicodeNameInfo(L"gap", L"gtrapprox")),
    make_pair(0x00002A89, UnicodeNameInfo(L"lnap", L"lnapprox")),
    make_pair(0x00002A8A, UnicodeNameInfo(L"gnap", L"gnapprox")),
    make_pair(0x00002A8B, UnicodeNameInfo(L"lEg", L"lesseqqgtr")),
    make_pair(0x00002A8C, UnicodeNameInfo(L"gEl", L"gtreqqless")),
    make_pair(0x00002A95, UnicodeNameInfo(L"els", L"eqslantless")),
    make_pair(0x00002A96, UnicodeNameInfo(L"egs", L"eqslantgtr")),
    make_pair(0x00002AAF, UnicodeNameInfo(L"pre", L"PrecedesEqual")),
    make_pair(0x00002AB0, UnicodeNameInfo(L"sce", L"SucceedsEqual")),
    make_pair(0x00002AB5, UnicodeNameInfo(L"prnE", L"precneqq")),
    make_pair(0x00002AB6, UnicodeNameInfo(L"scnE", L"succneqq")),
    make_pair(0x00002AB7, UnicodeNameInfo(L"prap", L"precapprox")),
    make_pair(0x00002AB8, UnicodeNameInfo(L"scap", L"succapprox")),
    make_pair(0x00002AB9, UnicodeNameInfo(L"prnap", L"precnapprox")),
    make_pair(0x00002ABA, UnicodeNameInfo(L"scnap", L"succnapprox")),
    make_pair(0x00002AC5, UnicodeNameInfo(L"subE", L"subseteqq")),
    make_pair(0x00002AC6, UnicodeNameInfo(L"supE", L"supseteqq")),
    make_pair(0x00002ACB, UnicodeNameInfo(L"subnE", L"subsetneqq")),
    make_pair(0x00002ACC, UnicodeNameInfo(L"supnE", L"supsetneqq")),
    make_pair(0x0000FE00, UnicodeNameInfo()),        // FIX: think about this combining character...
    make_pair(0x0000FE37, UnicodeNameInfo(L"OverBrace")),
    make_pair(0x0000FE38, UnicodeNameInfo(L"UnderBrace")),
    make_pair(0x0001D49C, UnicodeNameInfo(L"Ascr")),
    make_pair(0x0001D49E, UnicodeNameInfo(L"Cscr")),
    make_pair(0x0001D49F, UnicodeNameInfo(L"Dscr")),
    make_pair(0x0001D4A2, UnicodeNameInfo(L"Gscr")),
    make_pair(0x0001D4A5, UnicodeNameInfo(L"Jscr")),
    make_pair(0x0001D4A6, UnicodeNameInfo(L"Kscr")),
    make_pair(0x0001D4A9, UnicodeNameInfo(L"Nscr")),
    make_pair(0x0001D4AA, UnicodeNameInfo(L"Oscr")),
    make_pair(0x0001D4AB, UnicodeNameInfo(L"Pscr")),
    make_pair(0x0001D4AC, UnicodeNameInfo(L"Qscr")),
    make_pair(0x0001D4AE, UnicodeNameInfo(L"Sscr")),
    make_pair(0x0001D4AF, UnicodeNameInfo(L"Tscr")),
    make_pair(0x0001D4B0, UnicodeNameInfo(L"Uscr")),
    make_pair(0x0001D4B1, UnicodeNameInfo(L"Vscr")),
    make_pair(0x0001D4B2, UnicodeNameInfo(L"Wscr")),
    make_pair(0x0001D4B3, UnicodeNameInfo(L"Xscr")),
    make_pair(0x0001D4B4, UnicodeNameInfo(L"Yscr")),
    make_pair(0x0001D4B5, UnicodeNameInfo(L"Zscr")),
    make_pair(0x0001D4D0, UnicodeNameInfo()),    // mathematical bold script capitals
    make_pair(0x0001D4D1, UnicodeNameInfo()),
    make_pair(0x0001D4D2, UnicodeNameInfo()),
    make_pair(0x0001D4D3, UnicodeNameInfo()),
    make_pair(0x0001D4D4, UnicodeNameInfo()),
    make_pair(0x0001D4D5, UnicodeNameInfo()),
    make_pair(0x0001D4D6, UnicodeNameInfo()),
    make_pair(0x0001D4D7, UnicodeNameInfo()),
    make_pair(0x0001D4D8, UnicodeNameInfo()),
    make_pair(0x0001D4D9, UnicodeNameInfo()),
    make_pair(0x0001D4DA, UnicodeNameInfo()),
    make_pair(0x0001D4DB, UnicodeNameInfo()),
    make_pair(0x0001D4DC, UnicodeNameInfo()),
    make_pair(0x0001D4DD, UnicodeNameInfo()),
    make_pair(0x0001D4DE, UnicodeNameInfo()),
    make_pair(0x0001D4DF, UnicodeNameInfo()),
    make_pair(0x0001D4E0, UnicodeNameInfo()),
    make_pair(0x0001D4E1, UnicodeNameInfo()),
    make_pair(0x0001D4E2, UnicodeNameInfo()),
    make_pair(0x0001D4E3, UnicodeNameInfo()),
    make_pair(0x0001D4E4, UnicodeNameInfo()),
    make_pair(0x0001D4E5, UnicodeNameInfo()),
    make_pair(0x0001D4E6, UnicodeNameInfo()),
    make_pair(0x0001D4E7, UnicodeNameInfo()),
    make_pair(0x0001D4E8, UnicodeNameInfo()),
    make_pair(0x0001D4E9, UnicodeNameInfo()),
    make_pair(0x0001D504, UnicodeNameInfo(L"Afr")),
    make_pair(0x0001D505, UnicodeNameInfo(L"Bfr")),
    make_pair(0x0001D507, UnicodeNameInfo(L"Dfr")),
    make_pair(0x0001D508, UnicodeNameInfo(L"Efr")),
    make_pair(0x0001D509, UnicodeNameInfo(L"Ffr")),
    make_pair(0x0001D50A, UnicodeNameInfo(L"Gfr")),
    make_pair(0x0001D50D, UnicodeNameInfo(L"Jfr")),
    make_pair(0x0001D50E, UnicodeNameInfo(L"Kfr")),
    make_pair(0x0001D50F, UnicodeNameInfo(L"Lfr")),
    make_pair(0x0001D510, UnicodeNameInfo(L"Mfr")),
    make_pair(0x0001D511, UnicodeNameInfo(L"Nfr")),
    make_pair(0x0001D512, UnicodeNameInfo(L"Ofr")),
    make_pair(0x0001D513, UnicodeNameInfo(L"Pfr")),
    make_pair(0x0001D514, UnicodeNameInfo(L"Qfr")),
    make_pair(0x0001D516, UnicodeNameInfo(L"Sfr")),
    make_pair(0x0001D517, UnicodeNameInfo(L"Tfr")),
    make_pair(0x0001D518, UnicodeNameInfo(L"Ufr")),
    make_pair(0x0001D519, UnicodeNameInfo(L"Vfr")),
    make_pair(0x0001D51A, UnicodeNameInfo(L"Wfr")),
    make_pair(0x0001D51B, UnicodeNameInfo(L"Xfr")),
    make_pair(0x0001D51C, UnicodeNameInfo(L"Yfr")),
    make_pair(0x0001D51E, UnicodeNameInfo(L"afr")),
    make_pair(0x0001D51F, UnicodeNameInfo(L"bfr")),
    make_pair(0x0001D520, UnicodeNameInfo(L"cfr")),
    make_pair(0x0001D521, UnicodeNameInfo(L"dfr")),
    make_pair(0x0001D522, UnicodeNameInfo(L"efr")),
    make_pair(0x0001D523, UnicodeNameInfo(L"ffr")),
    make_pair(0x0001D524, UnicodeNameInfo(L"gfr")),
    make_pair(0x0001D525, UnicodeNameInfo(L"hfr")),
    make_pair(0x0001D526, UnicodeNameInfo(L"ifr")),
    make_pair(0x0001D527, UnicodeNameInfo(L"jfr")),
    make_pair(0x0001D528, UnicodeNameInfo(L"kfr")),
    make_pair(0x0001D529, UnicodeNameInfo(L"lfr")),
    make_pair(0x0001D52A, UnicodeNameInfo(L"mfr")),
    make_pair(0x0001D52B, UnicodeNameInfo(L"nfr")),
    make_pair(0x0001D52C, UnicodeNameInfo(L"ofr")),
    make_pair(0x0001D52D, UnicodeNameInfo(L"pfr")),
    make_pair(0x0001D52E, UnicodeNameInfo(L"qfr")),
    make_pair(0x0001D52F, UnicodeNameInfo(L"rfr")),
    make_pair(0x0001D530, UnicodeNameInfo(L"sfr")),
    make_pair(0x0001D531, UnicodeNameInfo(L"tfr")),
    make_pair(0x0001D532, UnicodeNameInfo(L"ufr")),
    make_pair(0x0001D533, UnicodeNameInfo(L"vfr")),
    make_pair(0x0001D534, UnicodeNameInfo(L"wfr")),
    make_pair(0x0001D535, UnicodeNameInfo(L"xfr")),
    make_pair(0x0001D536, UnicodeNameInfo(L"yfr")),
    make_pair(0x0001D537, UnicodeNameInfo(L"zfr")),
    make_pair(0x0001D538, UnicodeNameInfo(L"Aopf")),
    make_pair(0x0001D539, UnicodeNameInfo(L"Bopf")),
    make_pair(0x0001D53B, UnicodeNameInfo(L"Dopf")),
    make_pair(0x0001D53C, UnicodeNameInfo(L"Eopf")),
    make_pair(0x0001D53D, UnicodeNameInfo(L"Fopf")),
    make_pair(0x0001D53E, UnicodeNameInfo(L"Gopf")),
    make_pair(0x0001D540, UnicodeNameInfo(L"Iopf")),
    make_pair(0x0001D541, UnicodeNameInfo(L"Jopf")),
    make_pair(0x0001D542, UnicodeNameInfo(L"Kopf")),
    make_pair(0x0001D543, UnicodeNameInfo(L"Lopf")),
    make_pair(0x0001D544, UnicodeNameInfo(L"Mopf")),
    make_pair(0x0001D546, UnicodeNameInfo(L"Oopf")),
    make_pair(0x0001D54A, UnicodeNameInfo(L"Sopf")),
    make_pair(0x0001D54B, UnicodeNameInfo(L"Topf")),
    make_pair(0x0001D54C, UnicodeNameInfo(L"Uopf")),
    make_pair(0x0001D54D, UnicodeNameInfo(L"Vopf")),
    make_pair(0x0001D54E, UnicodeNameInfo(L"Wopf")),
    make_pair(0x0001D54F, UnicodeNameInfo(L"Xopf")),
    make_pair(0x0001D550, UnicodeNameInfo(L"Yopf")),
    make_pair(0x0001D55C, UnicodeNameInfo(L"kopf")),
    make_pair(0x0001D6A5, UnicodeNameInfo())
};

wishful_hash_map<uint32_t, UnicodeNameInfo> gUnicodeNameTable(
    gUnicodeNameArray,
    END_ARRAY(gUnicodeNameArray)
);


// FIX:
// Need to read about and think about combining characters.
// In particular, does the current strategy work for *named* entities
// and combining characters? I'm not sure.


// XmlEncode() handles conversion of non-ASCII characters to entities.
// It uses the "options" parameter and gUnicodeNameTable to decide how to
// translate each character.
wstring XmlEncode(
    const wstring& input,
    const EncodingOptions& options
)
{
    wostringstream os;
#ifdef WCHAR_T_IS_16BIT
    wchar_t surrogate_upper = 0;
#endif

    for (wstring::const_iterator
        ptr = input.begin(); ptr != input.end(); ptr++
    )
    {
        if (*ptr == L'&')
            os << L"&amp;";
        else if (*ptr == L'<')
            os << L"&lt;";
        else if (*ptr == L'>')
            os << L"&gt;";
        else if (*ptr <= 0x7F)
            os << *ptr;
#ifdef WCHAR_T_IS_16BIT
        else if (static_cast<wchar_t>(0xD800) <= *ptr &&
                 *ptr < static_cast<wchar_t>(0xDC00)) {
            surrogate_upper = *ptr;
            continue;
        }
#endif
        else
        {
            uint32_t chara = (uint32_t)*ptr;
#ifdef WCHAR_T_IS_16BIT
            if (0xDC00 <= chara && chara < 0xDF00) {
                if (surrogate_upper == 0) {
                    continue;
                }
                chara &= 0x3FF;
                chara |= ((uint32_t)surrogate_upper & 0x000003FF) << 10;
                chara += 0x00010000;
            }
#endif
            wishful_hash_map<uint32_t, UnicodeNameInfo>::const_iterator
                search = gUnicodeNameTable.find(chara);

            if (search == gUnicodeNameTable.end())
            {
                if (options.mOtherEncodingRaw) {
#ifdef WCHAR_T_IS_16BIT
                    if (surrogate_upper)
                        os << surrogate_upper;
#endif
                    os << *ptr;
                }
                else
                    os << L"&#x" << hex << chara << L";";
            }
            else
            {
                EncodingOptions::MathmlEncoding encoding
                    = options.mMathmlEncoding;

                // Deal with plane-1 characters.
                if (!options.mAllowPlane1 && chara >= 0x10000 &&
                    (
                        encoding == EncodingOptions::cMathmlEncodingNumeric
                        ||
                        encoding == EncodingOptions::cMathmlEncodingRaw
                    )
                )
                {
                    encoding = EncodingOptions::cMathmlEncodingShort;
                }

                // Notice the missing "break"s in this switch statement.
                // We are falling back on other encoding methods if certain
                // ones aren't available.
                switch (encoding)
                {
                    case EncodingOptions::cMathmlEncodingLong:
                        if (!search->second.mLongName.empty())
                        {
                            os << L"&" << search->second.mLongName << L";";
                            break;
                        }

                    case EncodingOptions::cMathmlEncodingShort:
                        if (!search->second.mShortName.empty())
                        {
                            os << L"&" << search->second.mShortName << L";";
                            break;
                        }

                    case EncodingOptions::cMathmlEncodingNumeric:
                        os << L"&#x" << hex << chara << L";";
                        break;

                    case EncodingOptions::cMathmlEncodingRaw:
#ifdef WCHAR_T_IS_16BIT
                        if (surrogate_upper)
                            os << surrogate_upper;
#endif
                        os << *ptr;
                        break;
                }

            }
        }
#ifdef WCHAR_T_IS_16BIT
        surrogate_upper = 0;
#endif
    }

    return os.str();
}

}

// end of file @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

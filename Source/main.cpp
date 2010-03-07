/*
blahtex: a TeX to MathML converter designed with MediaWiki in mind
blahtexml: an extension of blahtex with XML processing in mind
http://gva.noekeon.org/blahtexml

Copyright (c) 2006, David Harvey
Copyright (c) 2010, Gilles Van Assche
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the names of the authors nor the names of their affiliation may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "BlahtexCore/Interface.h"
#include "UnicodeConverter.h"
#include "mainPng.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace blahtex;

#ifdef BLAHTEXML_USING_XERCES
#include <iostream>
#include <string.h>
#include <xercesc/framework/StdInInputSource.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/parsers/SAX2XMLFilterImpl.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include "BlahtexXMLin/BlahtexFilter.h"
#include "BlahtexXMLin/SAX2Output.h"
#endif

string gBlahtexVersion = "0.8";

// A single global instance of UnicodeConverter.
UnicodeConverter gUnicodeConverter;

// Imported from Messages.cpp:
extern wstring GetErrorMessage(const blahtex::Exception& e);
extern wstring GetErrorMessages();

// FormatError() converts a blahtex Exception object into a string like
// "<error><id>...</id><arg>...</arg><arg>...</arg> ...
// <message>...</message></error".
wstring FormatError(
    const blahtex::Exception& e,
    const EncodingOptions& options
)
{
    wstring output = L"<error><id>" + e.GetCode() + L"</id>";
    for (vector<wstring>::const_iterator
        arg = e.GetArgs().begin(); arg != e.GetArgs().end(); arg++
    )
        output += L"<arg>" + XmlEncode(*arg, options) + L"</arg>";

    output += L"<message>";
    output += XmlEncode(GetErrorMessage(e), options);
    output += L"</message>";

    output += L"</error>";
    return output;
}

// ShowUsage() prints a help screen.
void ShowUsage()
{
    cout << "\n"
#ifdef BLAHTEXML_USING_XERCES
"Blahtexml version " << gBlahtexVersion << "\n"
#else
"Blahtex version " << gBlahtexVersion << "\n"
#endif
"Copyright (C) 2006, David Harvey\n"
"Copyright (C) 2007-2010, Gilles Van Assche\n"
"\n"
"This is free software; see the source "
"for copying conditions. There is NO\n"
"warranty; not even for MERCHANTABILITY "
"or FITNESS FOR A PARTICULAR PURPOSE.\n"
"\n"
#ifdef BLAHTEXML_USING_XERCES
"Normal mode:    blahtexml [ options ] < inputfile > outputfile\n"
"XML input mode: blahtexml --xmlin [ options ] < inputfile > outputfile\n"
#else
"Usage: blahtex [ options ] < inputfile > outputfile\n"
#endif
"\n"
"SUMMARY OF OPTIONS (see manual for details)\n"
"\n"
" --texvc-compatible-commands\n"
"\n"
" --mathml\n"
" --indented\n"
" --spacing { strict | moderate | relaxed }\n"
" --mathml-version-1-fonts\n"
" --disallow-plane-1\n"
" --mathml-encoding { raw | numeric | short | long }\n"
" --other-encoding { raw | numeric }\n"
"\n"
" --png\n"
" --displaymath\n"
" --use-ucs-package\n"
" --use-cjk-package\n"
" --use-preview-package\n"
" --japanese-font  fontname\n"
" --shell-latex  command\n"
" --shell-dvipng  command\n"
" --temp-directory  directory\n"
" --png-directory  directory\n"
" --png-latex-preamble content\n"
" --png-latex-before-math content\n"
"\n"
" --debug { parse | layout | purified }\n"
" --keep-temp-files\n"
" --throw-logic-error\n"
" --print-error-messages\n"
"\n"
#ifdef BLAHTEXML_USING_XERCES
" --doctype-system DTD\n"
" --doctype-public PublicID DTD\n"
" --doctype-xhtml+mathml\n"
" --mathml-nsprefix-auto\n"
" --mathml-nsprefix-none\n"
" --mathml-nsprefix prefix\n"
" --annotate-PNG\n"
" --annotate-TeX\n"
"\n"
"\n"
#endif
"More information available at http://gva.noekeon.org/blahtexml/\n"
"\n";

    // FIX: need command line option to select output DPI

    exit(0);
}

// CommandLineException is used for reporting incorrect command line
// syntax.
struct CommandLineException
{
    string mMessage;

    CommandLineException(
        const string& message
    ) :
        mMessage(message)
    { }
};

// Adds a trailing slash to the string, if it's not already there.
void AddTrailingSlash(string& s)
{
    if (!s.empty() && s[s.size() - 1] != '/')
        s += '/';
}

PngParams pngParams;
#ifdef BLAHTEXML_USING_XERCES
SAX2Output::Doctype outputDoctype = SAX2Output::DoctypeNone;
string outputPublicID;
string outputDTD;
BlahtexFilter::PrefixType MathMLPrefixType = BlahtexFilter::PrefixAuto;
string MathMLPrefix;
bool annotatePNG, annotateTeX;
int batchXMLConversion(blahtex::Interface& interface)
{
    cerr << "\n"
        "Blahtexml version " << gBlahtexVersion << "\n"
        "Copyright (C) 2006, David Harvey\n"
        "Copyright (C) 2007-2010, Gilles Van Assche\n"
        "\n"
        "This is free software; see the source "
        "for copying conditions. There is NO\n"
        "warranty; not even for MERCHANTABILITY "
        "or FITNESS FOR A PARTICULAR PURPOSE.\n";
    cerr << endl;
    try {
         XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch) {
         XERCES_STD_QUALIFIER cerr << "Error during initialization! :\n" << XMLString::transcode(toCatch.getMessage()) << endl;
         return 1;
    }
    SAX2XMLReader* reader = XMLReaderFactory::createXMLReader();
    BlahtexFilter* parser = new BlahtexFilter(reader, interface);

    parser->setFeature(XMLUni::fgSAX2CoreValidation, false);
    parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
    parser->setFeature(XMLUni::fgXercesSchema, false);
    parser->setFeature(XMLUni::fgXercesSchemaFullChecking, false);
    parser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, false);

    XercesString _MathMLPrefix(MathMLPrefix.c_str());
    wstring __MathMLPrefix = _MathMLPrefix.convertTowstring();
    parser->setDesiredMathMLPrefixType(MathMLPrefixType, __MathMLPrefix);
    parser->setAnnotatePNG(annotatePNG);
    parser->setAnnotateTeX(annotateTeX);
    parser->setPngParams(pngParams);

    int parserErrors = 0;
    int result = 0;
    {
        StdInInputSource stdIn;
        try {
            XercesString publicID(outputPublicID.c_str());
            XercesString DTD(outputDTD.c_str());
            SAX2Output output(cout, "UTF-8", XMLFormatter::UnRep_CharRef, outputDoctype, publicID, DTD);
            parser->setContentHandler(&output);
            parser->setErrorHandler(&output);
            parser->parse(stdIn);
            parserErrors = parser->getErrorCount();
        }
        catch (const OutOfMemoryException&) {
            cerr << endl;
            cerr << "Out of memory exception" << endl;
            result = 1;
        }
        catch (const XMLException& e) {
            char *errorString = XMLString::transcode(e.getMessage());
            cerr << endl;
            cerr << "Error: " << errorString << endl;
            XMLString::release(&errorString);
            result = 1;
        }
    }
    if (parserErrors > 0) {
        cerr << "During the XML input parsing, ";
        if (parserErrors == 1) cerr << "an error";
        else cerr << parserErrors << " errors";
        cerr << " occurred." << endl;
    }
    int blahtexErrors = parser->getNumberOfErrors();
    if (blahtexErrors > 0) {
        cerr << "During the blahtex conversion, ";
        if (blahtexErrors == 1) cerr << "an error";
        else cerr << blahtexErrors << " errors";
        cerr << " occurred.\nSee the output file for more information." << endl;
        result = 1;
    }
    delete parser;
    delete reader;
    XMLPlatformUtils::Terminate();
    return result;
}
#endif

int main (int argc, char* const argv[]) {
    // This outermost try block catches std::runtime_error
    // and CommandLineException.
    try
    {
        gUnicodeConverter.Open();

        blahtex::Interface interface;

        bool doPng    = false;
        bool doMathml = false;
#ifdef BLAHTEXML_USING_XERCES
        bool doXMLinput = false;
        annotatePNG = false;
        annotateTeX = false;
#endif

        bool debugLayoutTree  = false;
        bool debugParseTree   = false;
        bool debugPurifiedTex = false;

        pngParams.deleteTempFiles  = true;
        pngParams.shellLatex    = "latex";
        pngParams.shellDvipng   = "dvipng";
        pngParams.tempDirectory = "./";
        pngParams.pngDirectory  = "./";

        // Process command line arguments
        for (int i = 1; i < argc; i++)
        {
            string arg(argv[i]);

            if (arg == "--help")
                ShowUsage();

            else if (arg == "--print-error-messages")
            {
                cout << gUnicodeConverter.ConvertOut(GetErrorMessages())
                    << endl;
                return 0;
            }

            else if (arg == "--throw-logic-error")
                throw logic_error("Aaarrrgggghhhh!");

            else if (arg == "--shell-latex")
            {
                if (++i == argc)
                    throw CommandLineException(
                        "Missing string after \"--shell-latex\""
                    );
                pngParams.shellLatex = string(argv[i]);
            }

            else if (arg == "--shell-dvipng")
            {
                if (++i == argc)
                    throw CommandLineException(
                        "Missing string after \"--shell-dvipng\""
                    );
                pngParams.shellDvipng = string(argv[i]);
            }

            else if (arg == "--temp-directory")
            {
                if (++i == argc)
                    throw CommandLineException(
                        "Missing string after \"--temp-directory\""
                    );
                pngParams.tempDirectory = string(argv[i]);
                AddTrailingSlash(pngParams.tempDirectory);
            }

            else if (arg == "--png-directory")
            {
                if (++i == argc)
                    throw CommandLineException(
                        "Missing string after \"--png-directory\""
                    );
                pngParams.pngDirectory = string(argv[i]);
                AddTrailingSlash(pngParams.pngDirectory);
            }

            else if (arg == "--displaymath")
                interface.mPurifiedTexOptions.mDisplayMath = true;

            else if (arg == "--use-ucs-package")
                interface.mPurifiedTexOptions.mAllowUcs = true;

            else if (arg == "--use-cjk-package")
                interface.mPurifiedTexOptions.mAllowCJK = true;

            else if (arg == "--use-preview-package")
                interface.mPurifiedTexOptions.mAllowPreview = true;

            else if (arg == "--japanese-font")
            {
                if (++i == argc)
                    throw CommandLineException(
                        "Missing string after \"--japanese-font\""
                    );
                interface.mPurifiedTexOptions.mJapaneseFont =
                    gUnicodeConverter.ConvertIn(string(argv[i]));
            }

            else if (arg == "--indented")
                interface.mIndented = true;

            else if (arg == "--spacing")
            {
                if (++i == argc)
                    throw CommandLineException(
                        "Missing string after \"--spacing\""
                    );
                arg = string(argv[i]);

                if (arg == "strict")
                    interface.mMathmlOptions.mSpacingControl
                        = MathmlOptions::cSpacingControlStrict;

                else if (arg == "moderate")
                    interface.mMathmlOptions.mSpacingControl
                        = MathmlOptions::cSpacingControlModerate;

                else if (arg == "relaxed")
                    interface.mMathmlOptions.mSpacingControl
                        = MathmlOptions::cSpacingControlRelaxed;

                else
                    throw CommandLineException(
                        "Illegal string after \"--spacing\""
                    );
            }

            else if (arg == "--mathml-version-1-fonts")
                interface.mMathmlOptions.mUseVersion1FontAttributes = true;

            else if (arg == "--texvc-compatible-commands")
                interface.mTexvcCompatibility = true;

            else if (arg == "--png")
                doPng = true;

            else if (arg == "--mathml")
                doMathml = true;

            else if (arg == "--mathml-encoding")
            {
                if (++i == argc)
                    throw CommandLineException(
                        "Missing string after \"--mathml-encoding\""
                    );
                arg = string(argv[i]);

                if (arg == "raw")
                    interface.mEncodingOptions.mMathmlEncoding
                        = EncodingOptions::cMathmlEncodingRaw;

                else if (arg == "numeric")
                    interface.mEncodingOptions.mMathmlEncoding
                        = EncodingOptions::cMathmlEncodingNumeric;

                else if (arg == "short")
                    interface.mEncodingOptions.mMathmlEncoding
                        = EncodingOptions::cMathmlEncodingShort;

                else if (arg == "long")
                    interface.mEncodingOptions.mMathmlEncoding
                        = EncodingOptions::cMathmlEncodingLong;

                else
                    throw CommandLineException(
                        "Illegal string after \"--mathml-encoding\""
                    );
            }

            else if (arg == "--disallow-plane-1")
            {
                interface.mMathmlOptions  .mAllowPlane1 = false;
                interface.mEncodingOptions.mAllowPlane1 = false;
            }

            else if (arg == "--other-encoding")
            {
                if (++i == argc)
                    throw CommandLineException(
                        "Missing string after \"--other-encoding\""
                    );
                arg = string(argv[i]);
                if (arg == "raw")
                    interface.mEncodingOptions.mOtherEncodingRaw = true;
                else if (arg == "numeric")
                    interface.mEncodingOptions.mOtherEncodingRaw = false;
                else
                    throw CommandLineException(
                        "Illegal string after \"--other-encoding\""
                    );
            }

            else if (arg == "--debug")
            {
                if (++i == argc)
                    throw CommandLineException(
                        "Missing string after \"--debug\""
                    );
                arg = string(argv[i]);
                if (arg == "layout")
                    debugLayoutTree = true;
                else if (arg == "parse")
                    debugParseTree = true;
                else if (arg == "purified")
                    debugPurifiedTex = true;
                else
                    throw CommandLineException(
                        "Illegal string after \"--debug\""
                    );
            }

            else if (arg == "--keep-temp-files")
                pngParams.deleteTempFiles = false;
#ifdef BLAHTEXML_USING_XERCES
            else if (arg == "--xmlin")
                doXMLinput = true;
            else if (arg == "--doctype-system") {
                outputDoctype = SAX2Output::DoctypeSystem;
                if (++i == argc) throw CommandLineException("Missing string after \"--doctype-system\"");
                outputDTD = argv[i];
            }
            else if (arg == "--doctype-public") {
                outputDoctype = SAX2Output::DoctypePublic;
                if (++i == argc) throw CommandLineException("Missing two strings after \"--doctype-public\"");
                outputPublicID = argv[i];
                if (++i == argc) throw CommandLineException("Missing one string after \"--doctype-public\"");
                outputDTD = argv[i];
            }
            else if (arg == "--doctype-xhtml+mathml") {
                outputDoctype = SAX2Output::DoctypePublic;
                outputPublicID = "-//W3C//DTD XHTML 1.1 plus MathML 2.0//EN";
                outputDTD = "http://www.w3.org/TR/MathML2/dtd/xhtml-math11-f.dtd";
                MathMLPrefixType = BlahtexFilter::PrefixNone;
            }
            else if (arg == "--mathml-nsprefix-auto")
                MathMLPrefixType = BlahtexFilter::PrefixAuto;
            else if (arg == "--mathml-nsprefix-none")
                MathMLPrefixType = BlahtexFilter::PrefixNone;
            else if (arg == "--mathml-nsprefix") {
                MathMLPrefixType = BlahtexFilter::PrefixAdd;
                if (++i == argc) throw CommandLineException("Missing string after \"--mathml-nsprefix\"");
                MathMLPrefix = argv[i];
            }
            else if (arg == "--annotate-PNG")
                annotatePNG = true;
            else if (arg == "--annotate-TeX")
                annotateTeX = true;
#endif
            else if (arg == "--png-latex-preamble") {
                if (++i == argc) {
                    throw CommandLineException(
                        "Missing string after \"--png-latex-preamble\""
                    );
                }
                interface.mPurifiedTexOptions.mLaTeXPreamble = gUnicodeConverter.ConvertIn(string(argv[i]));
            }
            else if (arg == "--png-latex-before-math") {
                if (++i == argc) {
                    throw CommandLineException(
                        "Missing string after \"--png-latex-before-math\""
                    );
                }
                interface.mPurifiedTexOptions.mLaTeXBeforeMath = gUnicodeConverter.ConvertIn(string(argv[i]));
            }
            else
                throw CommandLineException(
                    "Unrecognised command line option \"" + arg + "\""
                );
        }

        // Finished processing command line, now process the input

#ifdef BLAHTEXML_USING_XERCES
        if (doXMLinput)
            return batchXMLConversion(interface);
#endif
        if (isatty(0))
            ShowUsage();

        wostringstream mainOutput;

        try
        {
            wstring input;

            // Read input file
            string inputUtf8;
            {
                char c;
                while (cin.get(c))
                    inputUtf8 += c;
            }

            // This try block converts UnicodeConverter::Exception into an
            // input syntax error, i.e. if the user supplies invalid UTF-8.
            // (Later we treat such exceptions as debug assertions.)
            try
            {
                input = gUnicodeConverter.ConvertIn(inputUtf8);
            }
            catch (UnicodeConverter::Exception& e)
            {
                throw blahtex::Exception(L"InvalidUtf8Input");
            }

            // Build the parse and layout trees.
            interface.ProcessInput(input);

            if (debugParseTree)
            {
                mainOutput << L"\n=== BEGIN PARSE TREE ===\n\n";
                interface.GetManager()->GetParseTree()->Print(mainOutput);
                mainOutput << L"\n=== END PARSE TREE ===\n\n";
            }

            if (debugLayoutTree)
            {
                mainOutput << L"\n=== BEGIN LAYOUT TREE ===\n\n";
                wostringstream temp;
                interface.GetManager()->GetLayoutTree()->Print(temp);
                mainOutput << XmlEncode(temp.str(), EncodingOptions());
                mainOutput << L"\n=== END LAYOUT TREE ===\n\n";
            }

            // Generate purified TeX if required.
            if (doPng || debugPurifiedTex)
            {
                // This stream is where we build the PNG output block:
                wostringstream pngOutput;

                try
                {
                    wstring purifiedTex = interface.GetPurifiedTex();

                    if (debugPurifiedTex)
                    {
                        pngOutput << L"\n=== BEGIN PURIFIED TEX ===\n\n";
                        pngOutput << purifiedTex;
                        pngOutput << L"\n=== END PURIFIED TEX ===\n\n";
                    }

                    // Make the system calls to generate the PNG image
                    // if requested.
                    if (doPng)
                    {
                        PngInfo info = MakePngFile(purifiedTex, "", pngParams);

                        // The height and depth measurements are only
                        // valid if the "preview" package is used:
                        if (interface.mPurifiedTexOptions.mAllowPreview
                            && info.mDimensionsValid
                        )
                        {
                            pngOutput << L"<height>"
                                << info.mHeight << L"</height>\n";
                            pngOutput << L"<depth>"
                                << info.mDepth << L"</depth>\n";
                        }

                        pngOutput << L"<md5>"
                            << gUnicodeConverter.ConvertIn(info.mMd5)
                            << L"</md5>\n";
                    }
                }

                // Catching errors that occurred during PNG generation:
                catch (blahtex::Exception& e)
                {
                    pngOutput.str(L"");
                    pngOutput << FormatError(e, interface.mEncodingOptions)
                        << endl;
                }

                mainOutput << L"<png>\n" << pngOutput.str() << L"</png>\n";
            }

            // This block generates MathML output if requested.
            if (doMathml)
            {
                // This stream is where we build the MathML output block:
                wostringstream mathmlOutput;

                try
                {
                    mathmlOutput << L"<markup>\n";
                    mathmlOutput << interface.GetMathml();
                    if (!interface.mIndented)
                        mathmlOutput << L"\n";
                    mathmlOutput << L"</markup>\n";
                }

                // Catch errors in generating the MathML:
                catch (blahtex::Exception& e)
                {
                    mathmlOutput.str(L"");
                    mathmlOutput
                        << FormatError(e, interface.mEncodingOptions)
                        << endl;
                }

                mainOutput << L"<mathml>\n" << mathmlOutput.str()
                    << L"</mathml>\n";
            }
        }

        // This catches input syntax errors.
        catch (blahtex::Exception& e)
        {
            mainOutput.str(L"");
            mainOutput << FormatError(e, interface.mEncodingOptions)
                << endl;
        }

        cout << "<blahtex>\n"
            << gUnicodeConverter.ConvertOut(mainOutput.str())
            << "</blahtex>\n";
    }

    // The following errors might occur if there's a bug in blahtex that
    // some assertion condition picked up. We still want to report these
    // nicely to the user so that they can notify the developers.
    catch (std::logic_error& e)
    {
        // WARNING: this doesn't XML-encode the message
        // (We don't expect to the message to contain the characters &<>)
        cout << "<blahtex>\n<logicError>" << e.what()
            << "</logicError>\n</blahtex>\n";
    }

    // These indicate incorrect command line syntax:
    catch (CommandLineException& e)
    {
        cerr << "blahtex: " << e.mMessage
#ifdef BLAHTEXML_USING_XERCES
            << " (try \"blahtexml --help\")\n";
#else
            << " (try \"blahtex --help\")\n";
#endif
    }

    // These kind of errors should only occur if the program has been
    // installed incorrectly.
    catch (std::runtime_error& e)
    {
        cerr << "blahtex runtime error: " << e.what() << endl;
    }

    return 0;
}

// end of file @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// File "BlahtexFilter.h"
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

#include <list>
#include <string>
#include <utility>
#include <xercesc/parsers/SAX2XMLFilterImpl.hpp>
#include "../BlahtexCore/Interface.h"

XERCES_CPP_NAMESPACE_USE

typedef list<pair<wstring, wstring> > list_wstring2;

class BlahtexFilter : public SAX2XMLFilterImpl
{
public:
    typedef enum { PrefixAuto, PrefixNone, PrefixAdd } PrefixType;
protected:
    blahtex::Interface& interface;
    list_wstring2 namespaceContext;
    int numberOfErrors;
    PrefixType desiredMathMLPrefixType;
    wstring desiredMathMLPrefix;
public:
    BlahtexFilter(SAX2XMLReader* parent, blahtex::Interface& anInterface);
    ~BlahtexFilter();
    virtual void startElement(const XMLCh* const uri, const XMLCh* const localname,
        const XMLCh* const qname, const Attributes& attributes);
    virtual void startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri);
    virtual void endPrefixMapping(const XMLCh* const prefix);
    int getNumberOfErrors();
    void setDesiredMathMLPrefixType(PrefixType aPrefixType, const wstring& aPrefix);
protected:
    bool getMathMLprefix(wstring& prefix);
};

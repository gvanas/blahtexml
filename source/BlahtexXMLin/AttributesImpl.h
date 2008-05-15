// File "AttributesImpl.h"
//
// blahtexml (version 0.5)
// Copyright (C) 2007-2008, Gilles Van Assche
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

#ifndef ATTRIBUTESIMPL_H
#define ATTRIBUTESIMPL_H

#include <string>
#include <vector>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/XMLString.hpp>
#include "XercesString.h"

XERCES_CPP_NAMESPACE_USE
using namespace std;

struct Attribute {
    XercesString qName;
    XercesString uri;
    XercesString localPart;
    XercesString value;
    XercesString type;
};

class AttributesImpl : public Attributes
{
private:
    vector<Attribute> theAttributes;
public:
    AttributesImpl();
    AttributesImpl(const Attributes& attributes);
    virtual unsigned int getLength() const;
    virtual const XMLCh* getURI(const unsigned int index) const;
    virtual const XMLCh* getLocalName(const unsigned int index) const;
    virtual const XMLCh* getQName(const unsigned int index) const;
    virtual const XMLCh* getType(const unsigned int index) const;
    virtual const XMLCh* getValue(const unsigned int index) const;
    virtual int getIndex(const XMLCh* const uri, const XMLCh* const localPart ) const;
    virtual int getIndex(const XMLCh* const qName ) const;
    virtual const XMLCh* getType(const XMLCh* const uri, const XMLCh* const localPart ) const;
    virtual const XMLCh* getType(const XMLCh* const qName) const;
    virtual const XMLCh* getValue(const XMLCh* const uri, const XMLCh* const localPart ) const;
    virtual const XMLCh* getValue(const XMLCh* const qName) const;
    void removeAttribute(const unsigned int index);
    void addAttribute(const XercesString& qName, const XercesString& uri, const XercesString& localPart, const XercesString& value, const XercesString& type);
};

#endif

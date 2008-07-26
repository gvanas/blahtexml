// File "AttributesImpl.cpp"
//
// blahtexml (version 0.6)
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

#include "AttributesImpl.h"

AttributesImpl::AttributesImpl()
{
}

AttributesImpl::AttributesImpl(const Attributes& attributes)
{
    for(unsigned int i=0; i<attributes.getLength(); ++i) {
        Attribute attribute;
        attribute.qName = XercesString(attributes.getQName(i));
        attribute.uri = XercesString(attributes.getURI(i));
        attribute.localPart = XercesString(attributes.getLocalName(i));
        attribute.value = XercesString(attributes.getValue(i));
        attribute.type = XercesString(attributes.getType(i));
        theAttributes.push_back(attribute);
    }
}

unsigned int AttributesImpl::getLength() const
{
    return theAttributes.size();
}

const XMLCh* AttributesImpl::getURI(const unsigned int index) const
{
    return theAttributes[index].uri.c_str();
}

const XMLCh* AttributesImpl::getLocalName(const unsigned int index) const
{
    return theAttributes[index].localPart.c_str();
}

const XMLCh* AttributesImpl::getQName(const unsigned int index) const
{
    return theAttributes[index].qName.c_str();
}

const XMLCh* AttributesImpl::getType(const unsigned int index) const
{
    return theAttributes[index].type.c_str();
}

const XMLCh* AttributesImpl::getValue(const unsigned int index) const
{
    return theAttributes[index].value.c_str();
}

int AttributesImpl::getIndex(const XMLCh* const uri, const XMLCh* const localPart) const
{
    for(unsigned int i=0;i<theAttributes.size();i++)
        if(XMLString::equals(getURI(i), uri) && XMLString::equals(getLocalName(i), localPart))
            return i;
    return -1;
}

int AttributesImpl::getIndex(const XMLCh* const qName ) const
{
    for(unsigned int i=0;i<theAttributes.size();i++)
        if(XMLString::equals(getQName(i), qName))
            return i;
    return -1;
}

const XMLCh* AttributesImpl::getType(const XMLCh* const uri, const XMLCh* const localPart ) const
{
    for(unsigned int i=0;i<theAttributes.size();i++)
        if(XMLString::equals(getURI(i), uri) && XMLString::equals(getLocalName(i), localPart))
            return getType(i);
    return NULL;
}

const XMLCh* AttributesImpl::getType(const XMLCh* const qName) const
{
    for(unsigned int i=0;i<theAttributes.size();i++)
        if(XMLString::equals(getQName(i), qName))
            return getType(i);
    return NULL;
}

const XMLCh* AttributesImpl::getValue(const XMLCh* const uri, const XMLCh* const localPart ) const
{
    for(unsigned int i=0;i<theAttributes.size();i++)
        if(XMLString::equals(getURI(i), uri) && XMLString::equals(getLocalName(i), localPart))
            return getValue(i);
    return NULL;
}

const XMLCh* AttributesImpl::getValue(const XMLCh* const qName) const
{
    for(unsigned int i=0;i<theAttributes.size();i++)
        if(XMLString::equals(getQName(i), qName))
            return getValue(i);
    return NULL;
}

void AttributesImpl::removeAttribute(const unsigned int index)
{
    theAttributes.erase(theAttributes.begin()+index);
}

void AttributesImpl::addAttribute(const XercesString& qName, const XercesString& uri, const XercesString& localPart, const XercesString& value, const XercesString& type)
{
    Attribute attribute;
    attribute.qName = qName;
    attribute.uri = uri;
    attribute.localPart = localPart;
    attribute.value = value;
    attribute.type = type;
    theAttributes.push_back(attribute);
}

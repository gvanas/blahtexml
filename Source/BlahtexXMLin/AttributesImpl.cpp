/*
blahtex: a TeX to MathML converter designed with MediaWiki in mind
blahtexml: an extension of blahtex with XML processing in mind
http://gva.noekeon.org/blahtexml

Copyright (c) 2010, Gilles Van Assche
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the names of the authors nor the names of their affiliation may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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

#if _XERCES_VERSION >= 30000
XMLSize_t AttributesImpl::getLength() const
#else
unsigned int AttributesImpl::getLength() const
#endif
{
    return theAttributes.size();
}

#if _XERCES_VERSION >= 30000
const XMLCh* AttributesImpl::getURI(const XMLSize_t index) const
#else
const XMLCh* AttributesImpl::getURI(const unsigned int index) const
#endif
{
    return theAttributes[index].uri.c_str();
}

#if _XERCES_VERSION >= 30000
const XMLCh* AttributesImpl::getLocalName(const XMLSize_t index) const
#else
const XMLCh* AttributesImpl::getLocalName(const unsigned int index) const
#endif
{
    return theAttributes[index].localPart.c_str();
}

#if _XERCES_VERSION >= 30000
const XMLCh* AttributesImpl::getQName(const XMLSize_t index) const
#else
const XMLCh* AttributesImpl::getQName(const unsigned int index) const
#endif
{
    return theAttributes[index].qName.c_str();
}

#if _XERCES_VERSION >= 30000
const XMLCh* AttributesImpl::getType(const XMLSize_t index) const
#else
const XMLCh* AttributesImpl::getType(const unsigned int index) const
#endif
{
    return theAttributes[index].type.c_str();
}

#if _XERCES_VERSION >= 30000
const XMLCh* AttributesImpl::getValue(const XMLSize_t index) const
#else
const XMLCh* AttributesImpl::getValue(const unsigned int index) const
#endif
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

#if _XERCES_VERSION >= 30000
bool AttributesImpl::getIndex(const XMLCh* const uri, const XMLCh* const localPart, XMLSize_t& index) const
{
    int temp = getIndex(uri, localPart);
    if (temp >= 0) {
        index = temp;
        return true;
    }
    else
        return false;
}

bool AttributesImpl::getIndex(const XMLCh* const qName, XMLSize_t& index) const
{
    int temp = getIndex(qName);
    if (temp >= 0) {
        index = temp;
        return true;
    }
    else
        return false;
}
#endif

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

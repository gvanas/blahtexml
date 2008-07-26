// File "XercesString.h"
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

#ifndef XERCESSTRING_H
#define XERCESSTRING_H

#include <string>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE
using namespace std;

class XercesString : public basic_string<XMLCh>
{
public:
    XercesString();
    XercesString(const char *s);
    XercesString(const XMLCh *s);
    XercesString(const wstring& in);
    wstring convertTowstring();
    friend ostream& operator<<(ostream& out, const XercesString& s);
};

#endif

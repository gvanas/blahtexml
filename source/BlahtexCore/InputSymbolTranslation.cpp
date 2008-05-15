// File "InputSymbolTranslation.cpp"
//
// blahtexml (version 0.5)
// Copyright (C) 2008, Gilles Van Assche
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

#include <map>
#include <memory>
#include <stdexcept>
#include "Misc.h"
#include "InputSymbolTranslation.h"

using namespace std;

namespace blahtex {

pair<wchar_t, wstring> equivalentInputSymbolsArray[] =
{
#include "InputSymbolTranslation.inc"
};
wishful_hash_map<wchar_t, wstring> equivalentInputSymbolsTable(
    equivalentInputSymbolsArray,
    END_ARRAY(equivalentInputSymbolsArray)
);

wstring translateToken(const wstring& token)
{
    if (token.length() == 1) {
        wishful_hash_map<wchar_t, wstring>::const_iterator
            equivalentInputSymbolsLookup = equivalentInputSymbolsTable.find(token[0]);
        if (equivalentInputSymbolsLookup != equivalentInputSymbolsTable.end())
            return equivalentInputSymbolsLookup->second;
        else
            return token;
    }
    else
        return token;
}

}

// end of file @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

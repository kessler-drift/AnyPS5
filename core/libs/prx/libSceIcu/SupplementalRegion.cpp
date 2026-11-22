// Derived from icu4c/source/common/loclikely.cpp@84e1f26ea77152936e70d53178a816dbfbf69989 (ICU 68.2):
// https://github.com/unicode-org/icu/blob/84e1f26ea77152936e70d53178a816dbfbf69989/icu4c/source/common/loclikely.cpp
//
// The upstream file's notice, verbatim:
//   © 2016 and later: Unicode, Inc. and others.
//   License & terms of use: http://www.unicode.org/copyright.html
//   Copyright (C) 1997-2016, International Business Machines
//   Corporation and others.  All Rights Reserved.

#include "prx/libSceIcu/libSceIcu.hpp"
#include "prx/libc/include/General.hpp"
#include <cstring>
#include <stdexcept>

namespace {

constexpr std::int32_t RegionBufferLength = 8;

}

extern "C" {

std::int32_t APS5_VABI ulocimp_getRegionForSupplementalData_nid_postfix(const char* localeID, std::int8_t inferRegion,
                                                                       char* region, std::int32_t regionCapacity,
                                                                       std::int32_t* status) {
    if (!status) throw std::invalid_argument("ulocimp_getRegionForSupplementalData: null status");
    if (*status > 0) return 0;
    if (regionCapacity < 0 || (!region && regionCapacity > 0))
        throw std::invalid_argument("ulocimp_getRegionForSupplementalData: invalid region buffer");
    char buffer[RegionBufferLength];
    std::int32_t keywordStatus = 0;
    std::int32_t length = uloc_getKeywordValue(localeID, "rg", buffer, RegionBufferLength, &keywordStatus);
    if (keywordStatus > 0 || length != 6) {
        length = 0;
    } else {
        for (char* unit = buffer; *unit != 0; ++unit) {
            if (*unit >= 'a' && *unit <= 'z') *unit = static_cast<char>(*unit - 'a' + 'A');
        }
        length = std::strcmp(buffer + 2, "ZZZZ") == 0 ? 2 : 0;
    }
    if (length == 0) {
        length = uloc_getCountry(localeID, buffer, RegionBufferLength, status);
        if (*status > 0) {
            length = 0;
        } else if (length >= RegionBufferLength) {
            NotImplemented_nid_no_patch(__func__);
        } else if (length == 0 && inferRegion) {
            NotImplemented_nid_no_patch(__func__);
        }
    }
    buffer[length] = 0;
    if (regionCapacity > 0) std::strncpy(region, buffer, static_cast<std::size_t>(regionCapacity));
    return u_terminateChars_nid_postfix(region, regionCapacity, length, status);
}

}

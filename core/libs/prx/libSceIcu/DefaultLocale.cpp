// Derived from icu4c/source/common/putil.cpp, locid.cpp and uloc.cpp@84e1f26ea77152936e70d53178a816dbfbf69989 (ICU 68.2):
// https://github.com/unicode-org/icu/blob/84e1f26ea77152936e70d53178a816dbfbf69989/icu4c/source/common/putil.cpp
// https://github.com/unicode-org/icu/blob/84e1f26ea77152936e70d53178a816dbfbf69989/icu4c/source/common/locid.cpp
// https://github.com/unicode-org/icu/blob/84e1f26ea77152936e70d53178a816dbfbf69989/icu4c/source/common/uloc.cpp
//
// The upstream files' notice, verbatim:
//   © 2016 and later: Unicode, Inc. and others.
//   License & terms of use: http://www.unicode.org/copyright.html
//   Copyright (C) 1997-2016, International Business Machines
//   Corporation and others.  All Rights Reserved.

#include "prx/libSceIcu/include/InventoryExports.hpp"
#include "prx/libSceIcu/Status.hpp"
#include "prx/libc/include/Api.hpp"
#include "prx/libc/include/General.hpp"
#include "prx/libSceIcu/include/LegacyExports.hpp"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <utility>

namespace {

constexpr int MessagesCategory = 6;
constexpr std::size_t LanguageCapacity = 12;

bool IsPosixDefault(const char* id) {
    return !id || std::strcmp(id, "C") == 0 || std::strcmp(id, "POSIX") == 0;
}

const char* PosixId() {
    const char* id = setlocale_nid_postfix(MessagesCategory, nullptr);
    if (IsPosixDefault(id)) {
        id = getenv_nid_postfix("LC_ALL");
        if (!id) {
            id = getenv_nid_postfix("LC_MESSAGES");
            if (!id) id = getenv_nid_postfix("LANG");
        }
    }
    return IsPosixDefault(id) ? "en_US_POSIX" : id;
}

std::string CorrectedPosixId(const char* posixId) {
    std::string corrected(posixId);
    corrected.resize(std::min(corrected.find('.'), corrected.size()));
    corrected.resize(std::min(corrected.find('@'), corrected.size()));
    if (corrected == "C" || corrected == "POSIX") corrected = "en_US_POSIX";
    if (const char* at = std::strrchr(posixId, '@')) {
        std::string_view variant(at + 1);
        if (variant == "nynorsk") variant = "NY";
        corrected += corrected.find('_') == std::string::npos ? "__" : "_";
        corrected += variant.substr(0, variant.find('.'));
    }
    return corrected;
}

const std::string& SystemLocaleId() {
    static const std::string id = CorrectedPosixId(PosixId());
    return id;
}

std::string Canonicalize(const char* id) {
    std::int32_t status = 0;
    const std::int32_t length = uloc_canonicalize(id, nullptr, 0, &status);
    if (status > 0 && status != Icu::BufferOverflow) NotImplemented_nid_no_patch("uloc_getDefault: the system locale ID does not canonicalize");
    std::string name(static_cast<std::size_t>(length) + 1, '\0');
    status = 0;
    uloc_canonicalize(id, name.data(), length + 1, &status);
    if (status > 0) NotImplemented_nid_no_patch("uloc_getDefault: the system locale ID does not canonicalize");
    name.resize(static_cast<std::size_t>(length));
    return name;
}

bool ReachesIDefault(std::string_view id) {
    return id.starts_with("i-default") && id.size() > 9 && std::string_view("._-@").find(id[9]) != std::string_view::npos &&
        id.find('@') != std::string_view::npos;
}

bool Bcp47Form(std::string_view id) {
    if (id.find('@') != std::string_view::npos) return false;
    if (id.size() == 1) return true;
    std::size_t length = 0;
    for (const char c : id) {
        if (c != '_' && c != '-') {
            ++length;
            continue;
        }
        if (length == 1) return true;
        length = 0;
    }
    return false;
}

bool TrailingSeparator(std::string_view id) {
    const auto base = id.substr(0, id.find_first_of(".@"));
    return !base.empty() && (base.back() == '_' || base.back() == '-');
}

std::optional<std::string> Name(const char* id) {
    if (ReachesIDefault(id)) NotImplemented_nid_no_patch("uloc_setDefault: ICU 68.2 deadlocks resolving i-default under its default locale lock");
    if (Bcp47Form(id) || TrailingSeparator(id)) NotImplemented_nid_no_patch("uloc_setDefault: ICU 78.1 uloc_getName differs from ICU 68.2 for this locale ID form");
    std::int32_t status = 0;
    const std::int32_t length = uloc_getName_nid_postfix(id, nullptr, 0, &status);
    if (status == Icu::IllegalArgument) NotImplemented_nid_no_patch("uloc_setDefault: ICU 78.1 limits subtag lengths that ICU 68.2 accepts");
    if (status > 0 && status != Icu::BufferOverflow) return std::nullopt;
    std::string name(static_cast<std::size_t>(length) + 1, '\0');
    status = 0;
    uloc_getName_nid_postfix(id, name.data(), length + 1, &status);
    if (status > 0) return std::nullopt;
    name.resize(static_cast<std::size_t>(length));
    return name;
}

std::size_t LanguageLength(const std::string& name) {
    const auto at = name.find('@');
    const auto separator = name.find('_');
    if (separator != std::string::npos && (at == std::string::npos || separator < at)) return separator;
    return std::min(name.find_first_of("@."), name.size());
}

std::mutex defaultMutex;
const char* defaultName = nullptr;

std::set<std::string>& DefaultNames() {
    static std::set<std::string> names;
    return names;
}

const char* SetSystemDefault() {
    std::string name = Canonicalize(SystemLocaleId().c_str());
    if (LanguageLength(name) >= LanguageCapacity) name.clear();
    std::lock_guard lock(defaultMutex);
    defaultName = DefaultNames().insert(std::move(name)).first->c_str();
    return defaultName;
}

void SetDefault(const char* id) {
    const auto resolved = Name(id);
    if (!resolved) return;
    std::string name = Name(resolved->c_str()).value_or(std::string());
    std::lock_guard lock(defaultMutex);
    defaultName = DefaultNames().insert(std::move(name)).first->c_str();
}

}

extern "C" {

const char* APS5_VABI uprv_getDefaultLocaleID_nid_postfix() {
    return SystemLocaleId().c_str();
}

const char* APS5_VABI uloc_getDefault() {
    {
        std::lock_guard lock(defaultMutex);
        if (defaultName) return defaultName;
    }
    return SetSystemDefault();
}

void APS5_VABI uloc_setDefault_nid_postfix(const char* newDefaultLocale, std::int32_t* status) {
    if (Icu::Failure(status)) return;
    if (!newDefaultLocale) {
        SetSystemDefault();
        return;
    }
    SetDefault(newDefaultLocale);
}

}

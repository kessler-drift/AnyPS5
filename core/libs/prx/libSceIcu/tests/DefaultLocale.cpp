#include "prx/libSceIcu/libSceIcu.hpp"
#include "prx/libSceIcu/tests/RecoveredSignatureChecks.hpp"
#include "prx/libc/include/Api.hpp"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>

namespace {

using IcuRecoveredTest::Require;
using IcuRecoveredTest::RequireThrows;

static_assert(std::is_same_v<decltype(&uloc_getDefault), const char* (APS5_VABI*)()>);
static_assert(std::is_same_v<decltype(&uloc_setDefault_nid_postfix), void (APS5_VABI*)(const char*, std::int32_t*)>);
static_assert(std::is_same_v<decltype(&uprv_getDefaultLocaleID_nid_postfix), const char* (APS5_VABI*)()>);

void SetEnvironment(const char* name, const char* value) {
    Require(setenv_nid_postfix(name, value, 1) == 0);
}

void Set(const char* id, std::source_location location = std::source_location::current()) {
    std::int32_t status = 0;
    uloc_setDefault_nid_postfix(id, &status);
    Require(status == 0, location);
}

void RequireDefault(std::string_view expected, std::source_location location = std::source_location::current()) {
    if (uloc_getDefault() != expected) std::fprintf(stderr, "default \"%s\"\n", uloc_getDefault());
    Require(uloc_getDefault() == expected, location);
}

void CheckExplicit(const char* systemName) {
    Set("fr_FR");
    const char* french = uloc_getDefault();
    RequireDefault("fr_FR");
    Set("EN-us");
    const char* english = uloc_getDefault();
    RequireDefault("en_US");
    Set("en_US");
    Require(uloc_getDefault() == english);
    Set("fr_FR");
    Require(uloc_getDefault() == french);

    std::int32_t status = 1;
    uloc_setDefault_nid_postfix("de_DE", &status);
    Require(status == 1);
    Require(uloc_getDefault() == french);

    status = -124;
    uloc_setDefault_nid_postfix("de_DE", &status);
    Require(status == -124);
    RequireDefault("de_DE");

    for (const char* rejected : {"en@=x", "en@a=", "en@abcdefghijklmnopqrstuvwxyzabcdefgh=v"}) {
        Set(rejected);
        RequireDefault("de_DE");
    }

    Set("sr_RS.UTF-8@latin");
    RequireDefault("sr_RS.UTF-8@latin");
    Set("de@collation=phonebook;currency=EUR");
    RequireDefault("de@collation=phonebook;currency=EUR");
    Set("zh-hant-tw");
    RequireDefault("zh_Hant_TW");
    Set("en__POSIX");
    RequireDefault("en__POSIX");
    const std::string longVariant = "en_US_" + std::string(179, 'X');
    Set(longVariant.c_str());
    RequireDefault(longVariant);
    Set("abcdefghijk_US");
    RequireDefault("abcdefghijk_US");
    Set("");
    RequireDefault("");

    Set("de_DE");
    const std::string tooLongVariant = "en_US_" + std::string(180, 'X');
    for (const char* unsettled : {"i-default@a=b", "I-default@a=b", "i_default", "x-foo", "en-US-u-ca-japanese", "en_u_ca_gregory", "a", "_",
             "en_", "en_US-", "abcdefghijkl_US", tooLongVariant.c_str()}) {
        status = 0;
        RequireThrows([&] { uloc_setDefault_nid_postfix(unsettled, &status); });
        RequireDefault("de_DE");
    }

    Set(nullptr);
    Require(uloc_getDefault() == systemName);
}

void Check(std::string_view systemId, std::string_view defaultName) {
    const char* id = uprv_getDefaultLocaleID_nid_postfix();
    Require(id == systemId);
    Require(uprv_getDefaultLocaleID_nid_postfix() == id);

    const char* name = uloc_getDefault();
    Require(name == defaultName);
    Require(uloc_getDefault() == name);

    SetEnvironment("LC_ALL", "ja_JP");
    Require(uprv_getDefaultLocaleID_nid_postfix() == id);

    std::int32_t status = 1;
    uloc_setDefault_nid_postfix(nullptr, &status);
    Require(status == 1);
    Require(uloc_getDefault() == name);

    status = 0;
    uloc_setDefault_nid_postfix(nullptr, &status);
    Require(status == 0);
    Require(uloc_getDefault() == name);

    CheckExplicit(name);
    RequireThrows([] { uloc_setDefault_nid_postfix(nullptr, nullptr); });
    Require(uloc_getDefault() == name);
}

}

int main(int argc, char** argv) {
    const std::string_view scenario = argc > 1 ? argv[1] : "";
    for (const char* name : {"LC_ALL", "LC_MESSAGES", "LANG"}) {
        Require(unsetenv_nid_postfix(name) == 0);
    }
    if (scenario == "posix") {
        Check("en_US_POSIX", "en_US_POSIX");
    } else if (scenario == "messages") {
        SetEnvironment("LANG", "fr_FR");
        SetEnvironment("LC_MESSAGES", "sr_RS.UTF-8@latin");
        Check("sr_RS_latin", "sr_RS_LATIN");
    } else if (scenario == "codeset") {
        SetEnvironment("LC_MESSAGES", "de_DE");
        SetEnvironment("LC_ALL", "C.UTF-8");
        Check("en_US_POSIX", "en_US_POSIX");
    } else if (scenario == "nynorsk") {
        SetEnvironment("LANG", "no@nynorsk");
        Check("no__NY", "no__NY");
    } else if (scenario == "long") {
        SetEnvironment("LANG", "abcdefghijkl_US");
        Check("abcdefghijkl_US", "");
    } else {
        std::fprintf(stderr, "unknown default locale scenario\n");
        return 1;
    }
    return 0;
}

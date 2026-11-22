#include "prx/libSceIcu/libSceIcu.hpp"
#include "prx/libc/include/General.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>

namespace {

struct Case {
    const char* locale;
    std::int8_t inferRegion;
    std::int32_t capacity;
    std::int32_t statusIn;
    std::int32_t result;
    std::int32_t statusOut;
    const char* bytes;
};

struct Inferred {
    const char* locale;
    std::int8_t inferRegion;
};

const Case Cases[] = {
    {"en", 0, 0, 0, 0, -124, "55555555"},
    {"en", 0, 1, 0, 0, 0, "0055555555"},
    {"en", 0, 2, 0, 0, 0, "000055555555"},
    {"en", 0, 3, 0, 0, 0, "00000055555555"},
    {"en", 0, 4, 0, 0, 0, "0000000055555555"},
    {"en", 0, 8, 0, 0, 0, "000000000000000055555555"},
    {"en", 0, 8, -124, 0, 0, "000000000000000055555555"},
    {"en", 0, 8, -128, 0, -128, "000000000000000055555555"},
    {"en", 0, 8, 1, 0, 1, "555555555555555555555555"},
    {"en", 1, 8, 1, 0, 1, "555555555555555555555555"},
    {"en_US", 0, 0, 0, 2, 15, "55555555"},
    {"en_US", 0, 1, 0, 2, 15, "5555555555"},
    {"en_US", 0, 2, 0, 2, -124, "555355555555"},
    {"en_US", 0, 3, 0, 2, 0, "55530055555555"},
    {"en_US", 0, 4, 0, 2, 0, "5553000055555555"},
    {"en_US", 0, 8, 0, 2, 0, "555300000000000055555555"},
    {"en_US", 0, 8, -124, 2, 0, "555300000000000055555555"},
    {"en_US", 0, 8, -128, 2, -128, "555300000000000055555555"},
    {"en_US", 0, 8, 1, 0, 1, "555555555555555555555555"},
    {"en_US", 1, 0, 0, 2, 15, "55555555"},
    {"en_US", 1, 1, 0, 2, 15, "5555555555"},
    {"en_US", 1, 2, 0, 2, -124, "555355555555"},
    {"en_US", 1, 3, 0, 2, 0, "55530055555555"},
    {"en_US", 1, 4, 0, 2, 0, "5553000055555555"},
    {"en_US", 1, 8, 0, 2, 0, "555300000000000055555555"},
    {"en_US", 1, 8, -124, 2, 0, "555300000000000055555555"},
    {"en_US", 1, 8, -128, 2, -128, "555300000000000055555555"},
    {"en_US", 1, 8, 1, 0, 1, "555555555555555555555555"},
    {"en-US", 0, 8, 0, 2, 0, "555300000000000055555555"},
    {"en-US", 1, 8, 0, 2, 0, "555300000000000055555555"},
    {"und", 0, 8, 0, 0, 0, "000000000000000055555555"},
    {"", 0, 8, 0, 0, 0, "000000000000000055555555"},
    {nullptr, 0, 8, 0, 2, 0, "434100000000000055555555"},
    {nullptr, 1, 8, 0, 2, 0, "434100000000000055555555"},
    {"zh_Hant_TW", 0, 8, 0, 2, 0, "545700000000000055555555"},
    {"zh_Hant_TW", 1, 8, 0, 2, 0, "545700000000000055555555"},
    {"es_419@rg=mxzzzz", 0, 8, 0, 2, 0, "4d5800000000000055555555"},
    {"es_419@rg=mxzzzz", 1, 8, 0, 2, 0, "4d5800000000000055555555"},
    {"en@rg=uszzzz", 0, 0, 0, 2, 15, "55555555"},
    {"en@rg=uszzzz", 0, 1, 0, 2, 15, "5555555555"},
    {"en@rg=uszzzz", 0, 2, 0, 2, -124, "555355555555"},
    {"en@rg=uszzzz", 0, 3, 0, 2, 0, "55530055555555"},
    {"en@rg=uszzzz", 0, 4, 0, 2, 0, "5553000055555555"},
    {"en@rg=uszzzz", 0, 8, 0, 2, 0, "555300000000000055555555"},
    {"en@rg=uszzzz", 0, 8, -124, 2, 0, "555300000000000055555555"},
    {"en@rg=uszzzz", 0, 8, -128, 2, -128, "555300000000000055555555"},
    {"en@rg=uszzzz", 0, 8, 1, 0, 1, "555555555555555555555555"},
    {"en@rg=uszzzz", 1, 0, 0, 2, 15, "55555555"},
    {"en@rg=uszzzz", 1, 1, 0, 2, 15, "5555555555"},
    {"en@rg=uszzzz", 1, 2, 0, 2, -124, "555355555555"},
    {"en@rg=uszzzz", 1, 3, 0, 2, 0, "55530055555555"},
    {"en@rg=uszzzz", 1, 4, 0, 2, 0, "5553000055555555"},
    {"en@rg=uszzzz", 1, 8, 0, 2, 0, "555300000000000055555555"},
    {"en@rg=uszzzz", 1, 8, -124, 2, 0, "555300000000000055555555"},
    {"en@rg=uszzzz", 1, 8, -128, 2, -128, "555300000000000055555555"},
    {"en@rg=uszzzz", 1, 8, 1, 0, 1, "555555555555555555555555"},
    {"en_GB@rg=uszzzz", 0, 8, 0, 2, 0, "555300000000000055555555"},
    {"en_GB@rg=uszzzz", 1, 8, 0, 2, 0, "555300000000000055555555"},
    {"en@RG=uszzzz", 0, 8, 0, 2, 0, "555300000000000055555555"},
    {"en@RG=uszzzz", 1, 8, 0, 2, 0, "555300000000000055555555"},
    {"en@rg=us", 0, 8, 0, 0, 0, "000000000000000055555555"},
    {"en@rg=", 0, 8, 0, 0, 0, "000000000000000055555555"},
    {"en@rg=419zzz", 0, 8, 0, 0, 0, "000000000000000055555555"},
    {"en_US@rg=zzzzzz", 0, 8, 0, 2, 0, "5a5a00000000000055555555"},
    {"en_US@rg=zzzzzz", 1, 8, 0, 2, 0, "5a5a00000000000055555555"},
    {"en@sd=usca;rg=gbzzzz", 0, 8, 0, 2, 0, "474200000000000055555555"},
    {"en@sd=usca;rg=gbzzzz", 1, 8, 0, 2, 0, "474200000000000055555555"},
    {"en@rg=uszzzz;rg=gbzzzz", 0, 8, 0, 2, 0, "555300000000000055555555"},
    {"en@rg=uszzzz;rg=gbzzzz", 1, 8, 0, 2, 0, "555300000000000055555555"},
    {"en-US-u-rg-gbzzzz", 0, 8, 0, 2, 0, "474200000000000055555555"},
    {"en-US-u-rg-gbzzzz", 1, 8, 0, 2, 0, "474200000000000055555555"},
    {"en-u-rg-uszzzz", 0, 8, 0, 2, 0, "555300000000000055555555"},
    {"en-u-rg-uszzzz", 1, 8, 0, 2, 0, "555300000000000055555555"},
    {"de_DE.UTF-8@euro", 0, 8, 0, 2, 0, "444500000000000055555555"},
    {"de_DE.UTF-8@euro", 1, 8, 0, 2, 0, "444500000000000055555555"},
    {"en__POSIX@rg=dezzzz", 0, 8, 0, 2, 0, "444500000000000055555555"},
    {"en__POSIX@rg=dezzzz", 1, 8, 0, 2, 0, "444500000000000055555555"},
    {"abcdefghijk_US", 0, 8, 0, 2, 0, "555300000000000055555555"},
    {"abcdefghijk_US", 1, 8, 0, 2, 0, "555300000000000055555555"},
    {"en@ rg = uszzzz ", 0, 8, 0, 2, 0, "555300000000000055555555"},
    {"en@ rg = uszzzz ", 1, 8, 0, 2, 0, "555300000000000055555555"},
    {"@rg=uszzzz", 0, 8, 0, 2, 0, "555300000000000055555555"},
    {"@rg=uszzzz", 1, 8, 0, 2, 0, "555300000000000055555555"},
    {"en@rg=uszzzz@", 0, 8, 0, 0, 0, "000000000000000055555555"},
    {"en_@rg=gbzzzz", 0, 8, 0, 2, 0, "474200000000000055555555"},
    {"en_@rg=gbzzzz", 1, 8, 0, 2, 0, "474200000000000055555555"},
    {"en_GB@rg=uszzzy", 0, 8, 0, 2, 0, "474200000000000055555555"},
    {"en@rg=cazzzz", 0, 8, 0, 2, 0, "434100000000000055555555"},
    {"en@rg=12zzzz", 0, 8, 0, 2, 0, "313200000000000055555555"},
};

const Inferred InferredCases[] = {
    {"en", 1},
    {"und", 1},
    {"", 1},
    {"en@rg=us", 1},
    {"en@rg=", 1},
    {"en@rg=419zzz", 1},
    {"en@rg=uszzzz@", 1},
};

int failures = 0;

void Fail(const char* locale, const char* what) {
    std::fprintf(stderr, "%s: %s\n", locale ? locale : "(null)", what);
    ++failures;
}

std::string Hex(const char* bytes, std::int32_t count) {
    std::string text;
    char unit[3];
    for (std::int32_t i = 0; i < count; ++i) {
        std::snprintf(unit, sizeof unit, "%02x", static_cast<unsigned char>(bytes[i]));
        text += unit;
    }
    return text;
}

void TestCases() {
    for (const auto& item : Cases) {
        char region[16];
        std::memset(region, 0x55, sizeof region);
        std::int32_t status = item.statusIn;
        const auto result = ulocimp_getRegionForSupplementalData_nid_postfix(item.locale, item.inferRegion, region, item.capacity, &status);
        if (result != item.result) Fail(item.locale, "result");
        if (status != item.statusOut) Fail(item.locale, "status");
        if (Hex(region, item.capacity + 4) != item.bytes) Fail(item.locale, "region bytes");
    }
}

void TestInferredRegions() {
    for (const auto& item : InferredCases) {
        char region[16];
        std::memset(region, 0x55, sizeof region);
        std::int32_t status = 0;
        const auto before = NotImplementedCalls_nid_no_patch();
        try {
            ulocimp_getRegionForSupplementalData_nid_postfix(item.locale, item.inferRegion, region, 8, &status);
            Fail(item.locale, "no throw where likely subtags are needed");
        } catch (const std::runtime_error&) {
        }
        if (NotImplementedCalls_nid_no_patch() - before != 1) Fail(item.locale, "NotImplemented calls");
        if (Hex(region, 12) != "555555555555555555555555") Fail(item.locale, "region written before the throw");
    }
}

void TestNullRegion() {
    const struct {
        const char* locale;
        std::int32_t result;
        std::int32_t status;
    } cases[] = {{"en", 0, -124}, {"en_US", 2, 15}, {"en@rg=gbzzzz", 2, 15}};
    for (const auto& item : cases) {
        std::int32_t status = 0;
        if (ulocimp_getRegionForSupplementalData_nid_postfix(item.locale, 0, nullptr, 0, &status) != item.result) Fail(item.locale, "null region result");
        if (status != item.status) Fail(item.locale, "null region status");
    }
}

template <typename Function>
void ExpectInvalid(Function function, const char* what) {
    try {
        function();
    } catch (const std::invalid_argument&) {
        return;
    }
    Fail(what, "no std::invalid_argument");
}

void TestInvalidArguments() {
    char region[8];
    std::int32_t status = 0;
    ExpectInvalid([&] { ulocimp_getRegionForSupplementalData_nid_postfix("en_US", 0, region, 8, nullptr); }, "null status");
    ExpectInvalid([&] { ulocimp_getRegionForSupplementalData_nid_postfix("en_US", 0, region, -1, &status); }, "negative capacity");
    ExpectInvalid([&] { ulocimp_getRegionForSupplementalData_nid_postfix("en_US", 0, nullptr, 4, &status); }, "null region");
    status = 1;
    if (ulocimp_getRegionForSupplementalData_nid_postfix("en_US", 0, nullptr, -1, &status) != 0 || status != 1)
        Fail("en_US", "failing status checked first");
}

} // namespace

int main() {
    std::int32_t status = 0;
    uloc_setDefault_nid_postfix("fr_CA", &status);
    if (status > 0) return EXIT_FAILURE;
    TestCases();
    TestInferredRegions();
    TestNullRegion();
    TestInvalidArguments();
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

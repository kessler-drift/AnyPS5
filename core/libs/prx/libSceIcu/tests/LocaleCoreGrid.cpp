#include "prx/libSceIcu/include/InventoryExports.hpp"

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>

namespace {

using ProjectGetter = std::int32_t (APS5_VABI*)(const char*, char*, std::int32_t, std::int32_t*);
using ProjectCode = const char* (APS5_VABI*)(const char*);

constexpr std::array<ProjectGetter, 5> ProjectGetters{
    uloc_getLanguage, uloc_getCountry, uloc_getScript, uloc_getBaseName, uloc_canonicalize};
constexpr std::array<ProjectCode, 2> ProjectCodes{uloc_getISO3Language, uloc_getISO3Country};

}

#include "unicode/uloc.h"

namespace {

using IcuGetter = std::int32_t (*)(const char*, char*, std::int32_t, UErrorCode*);
using IcuCode = const char* (*)(const char*);

constexpr std::array<IcuGetter, 5> IcuGetters{
    uloc_getLanguage, uloc_getCountry, uloc_getScript, uloc_getBaseName, uloc_canonicalize};
constexpr std::array<IcuCode, 2> IcuCodes{uloc_getISO3Language, uloc_getISO3Country};
constexpr std::array<const char*, 5> GetterNames{
    "uloc_getLanguage", "uloc_getCountry", "uloc_getScript", "uloc_getBaseName", "uloc_canonicalize"};
constexpr std::array<const char*, 2> CodeNames{"uloc_getISO3Language", "uloc_getISO3Country"};

using Family = std::initializer_list<std::initializer_list<std::string_view>>;

int failures = 0;

void Fail(const char* function, const std::string& id, std::int32_t capacity) {
    std::fprintf(stderr, "%s(\"%s\", capacity %d) differs from ICU\n", function, id.c_str(), capacity);
    ++failures;
}

void Compare(const std::string& id) {
    for (std::size_t which = 0; which < ProjectGetters.size(); ++which) {
        UErrorCode probe = U_ZERO_ERROR;
        const std::int32_t full = IcuGetters[which](id.c_str(), nullptr, 0, &probe);
        for (const std::int32_t capacity : {full - 1, full, full + 1, 100}) {
            if (capacity < 0) continue;
            std::array<char, 104> expected{};
            std::array<char, 104> actual{};
            expected.fill('Q');
            actual.fill('Q');
            UErrorCode icuStatus = U_ZERO_ERROR;
            std::int32_t projectStatus = 0;
            const std::int32_t icuLength = IcuGetters[which](id.c_str(), expected.data(), capacity, &icuStatus);
            const std::int32_t projectLength = ProjectGetters[which](id.c_str(), actual.data(), capacity, &projectStatus);
            if (projectStatus != icuStatus ||
                (icuStatus != U_BUFFER_OVERFLOW_ERROR && expected != actual) ||
                ((icuStatus <= U_ZERO_ERROR || icuStatus == U_BUFFER_OVERFLOW_ERROR) &&
                 projectLength != icuLength)) {
                Fail(GetterNames[which], id, capacity);
            }
        }
    }
    for (std::size_t which = 0; which < ProjectCodes.size(); ++which) {
        if (std::string_view(ProjectCodes[which](id.c_str())) != IcuCodes[which](id.c_str()))
            Fail(CodeNames[which], id, 0);
    }
}

void Expand(const Family& family) {
    std::vector<std::string> ids{std::string()};
    for (const auto& choices : family) {
        std::vector<std::string> next;
        for (const auto& prefix : ids) {
            for (const auto choice : choices) next.push_back(prefix + std::string(choice));
        }
        ids.swap(next);
    }
    for (const auto& id : ids) Compare(id);
}

}

int main() {
    Expand({
        {"root", "ROOT", "Root", "und", "UND", "uNd", "undx", "rooty", "roo"},
        {"", "_GB", "-gb", "_Latn", "-latn-us", "_US_POSIX", ".utf8", "_GB.utf8", "@currency=EUR",
         "_GB@currency=EUR"},
    });
    Expand({
        {"en", "de_DE", "_US", "", "sr_Latn_RS", "fr.utf8"},
        {"@Collation=Phonebook", "@B=1;a=2", "@calendar=buddhist;calendar=islamic", "@ calendar = buddhist ",
         "@calendar=", "@calendar=x;;currency=y", "@k= ;a=b", "@a=b; ;c=d", "@a=b;c", "@;a=b", "@a=b;",
         "@A=B;C=D", "@x=a b c", "@a=b=c", "@a.b=c", "@ =b", "@abcdefghijklmnopqrstuvwxy=1"},
    });
    Expand({
        {"en", "de_DE", "_US", "", "en_Latn", "en_Latn_US", "en_US_POSIX", "en__POSIX", "en_Latn__POSIX",
         "fr_FR.utf8", "en-US"},
        {"@euro", "@EURO", "@foo-bar", "@foo,bar", "@foo.bar", "@a;b", "@euro@x"},
    });
    Expand({
        {"en_US_POSIX", "de_DE_PREEURO", "sr_Latn_RS_REVISED", "en__POSIX", "en_Latn__POSIX"},
        {"_", "__", "_.utf8", "_@euro", "-@euro", "__@euro", "_.utf8@euro", "_@calendar=x"},
    });
    if (failures != 0) {
        std::fprintf(stderr, "%d differences\n", failures);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

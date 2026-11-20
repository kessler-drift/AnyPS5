# libSceIcu

## add Functional locale parsing and metadata

- [Keyword lists in uloc_canonicalize](../../../core/libs/prx/libSceIcu/LocaleCore.cpp) - a keyword list that fails with `U_INVALID_FORMAT_ERROR` or `U_INTERNAL_PROGRAM_ERROR` returns the length of the ID written before it, as ICU 68.2 does ([uloc.cpp](https://github.com/unicode-org/icu/blob/84e1f26ea77152936e70d53178a816dbfbf69989/icu4c/source/common/uloc.cpp#L1918-L1925)); ICU 78.1 returns 0. Which one a console returns is unmeasured. A keyword list right after a separator, as in `en_US_@calendar=buddhist`, is canonicalized as in ICU 78.1 (`en_US@calendar=buddhist`); ICU 68.2 reads it as the variant (`en_US_CALENDAR=BUDDHIST`).

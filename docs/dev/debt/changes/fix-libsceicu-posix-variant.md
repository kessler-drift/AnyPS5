# libSceIcu

## add Functional locale parsing and metadata

- [POSIX variants in uloc_canonicalize](../../../core/libs/prx/libSceIcu/LocaleCore.cpp) - an `@` modifier follows the `_` separators counted as in [ICU 68.2](https://github.com/unicode-org/icu/blob/84e1f26ea77152936e70d53178a816dbfbf69989/icu4c/source/common/uloc.cpp#L1505-L1611), so `en_POSIX@euro` gives `en__POSIX_EURO` (78.1 `en__POSIX__EURO`; a console is unmeasured). With no variant, a single separator followed by a non-empty modifier is not handled as in 68.2, which copies the modifier as the variant and resumes from a wrong position (`uloc_canonicalize("en_US_.utf8@euro")` gives `en_US_EURO_EURO` there): `uloc_getBaseName("en_US_.utf8@euro")` gives `en_US.utf8@euro` (68.2 `en_US_EURO@euro`, 78.1 `en_US@euro`) and `uloc_getBaseName("en_US_@euro")` `en_US@euro` (68.2 `en_US_EURO`), while `uloc_canonicalize("en_Latn_@euro")` gives `en_Latn__EURO` as 68.2 does (78.1 `en_Latn_EURO`).

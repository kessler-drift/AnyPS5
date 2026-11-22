# libSceIcu

## add Functional

- [ulocimp_getRegionForSupplementalData](../../../core/libs/prx/libSceIcu/SupplementalRegion.cpp) (libSceIcu) - throws `NotImplemented_nid_no_patch` when `inferRegion` is set and the locale has neither a valid `rg` keyword nor a region: ICU 68.2 then takes the region from its likely-subtags data, which the project does not have. A null status, a negative capacity, or a null region with a positive capacity throw `std::invalid_argument`, where ICU 68.2 dereferences them. Otherwise it follows ICU 68.2's [loclikely.cpp](https://github.com/unicode-org/icu/blob/84e1f26ea77152936e70d53178a816dbfbf69989/icu4c/source/common/loclikely.cpp#L1362-L1409) on the project's `uloc_getKeywordValue` and `uloc_getCountry`, and inherits their gaps. Not measured on a console.

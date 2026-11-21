# libSceLibreSsl3

## add Functional

- [d2i_PKCS7 long-form lengths](../../../core/libs/prx/libSceLibreSsl3/src/Pkcs7.cpp) (libSceLibreSsl3) - not checked on the console. The content info, signed data, signer info and attribute headers accept long-form lengths below 128 and leading zero octets, as LibreSSL 3.4.1 [`asn1_get_length`](https://github.com/libressl/openbsd/blob/078b3d1df8ca2011c1605cd40418c58fa0f5b143/src/lib/libcrypto/asn1/asn1_lib.c#L156-L194) does. Algorithm identifiers, issuer names and certificates are read by the `X509_STORE_load_locations` decoder, which still refuses such lengths with reason 102, so a PKCS7 that uses them there fails to decode. INTEGER and ENUMERATED fields are read again by `d2i_ASN1_INTEGER`, which refuses a first length octet 0xff (127 length octets) as LibreSSL 4.0.0 does, where 3.4.1 accepts it. Closes when: that decoder accepts these lengths, the 0xff rule is settled, and both are compared with the console.

// The Garden Spine programme CA.
//
// This is a public trust anchor, not a secret. It is baked in so that no student has to paste a
// certificate by hand. TLS verification still happens in full — GardenSpine.h never disables it.
//
//   subject/issuer  CN=Garden Spine Programme CA (self-signed root)
//   valid           2026-07-30 to 2029-07-29
//   sha256          15:6C:68:77:A4:70:E3:88:59:1C:5A:E3:0B:06:CD:E7:B6:20:B2:99:
//                   14:51:D2:DB:CF:8C:7B:68:8A:A4:4C:AE
//   source          https://gardenspine.ikapo.fi/downloads/garden-spine-ca.crt
//
// Verify this copy against the live one at any time:
//
//   curl -s https://gardenspine.ikapo.fi/downloads/garden-spine-ca.crt | openssl x509 -noout -fingerprint -sha256
//
// If the broker's CA is ever replaced, update this file, bump the version in library.properties,
// and tell every team to reinstall the library. Devices cannot connect until they do.
//
// To point a device at a different broker, define both of these in config.h instead:
//
//   #define SPINE_CUSTOM_CA
//   const char* SPINE_CA = R"EOF(
//   -----BEGIN CERTIFICATE-----
//   )EOF";

#pragma once

const char* SPINE_PROGRAMME_CA = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFKTCCAxGgAwIBAgIUWrgFnhWIp4nVsu598sljt3q8BHYwDQYJKoZIhvcNAQEL
BQAwJDEiMCAGA1UEAwwZR2FyZGVuIFNwaW5lIFByb2dyYW1tZSBDQTAeFw0yNjA3
MzAxNTIyNTFaFw0yOTA3MjkxNTIyNTFaMCQxIjAgBgNVBAMMGUdhcmRlbiBTcGlu
ZSBQcm9ncmFtbWUgQ0EwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQDC
8u6w6OT9dPp5M6WiXFGxhsCjZB5gvNpeRcqlEJ4XYysf+6BcByhudhYSwdjF3ueW
Q/NXN4vn60p8m2WhhOoJ41uTyS4Q9k0CXm0dliqJZ4dUDTqLkt5g1pNerEb1qk9x
3QkpkdPekBxlhLhqivqaoPJO+hH2gEBNWZ+vEPApr4bEPwK0/EZJyPv0L8I8zIN+
hub1n1aYKUybQ2VYpxdXfCqpPdasuBNmXBnV+xPMm+m0+f3+8DTuY6rGc7o6AZ81
BV9tWzGWudEYAip5LB5TMyerYdUIyh2cjO4NzEkl5jWxfOoNPfDSeStPniEKcanu
OZli+U2juMUbhlcAbH9ASKkhxctd88EAgE5GVkzWA+FAQWgMREUyZyTdrkalPY1d
Ef3z8nRtCluslHPhv9ciTXadXv0DigK5VuVaQEDNZIOht8hGwnGXymlBq2g19FHU
QTwrMqCjF+FAZtiwFOMXff+2W/SJ/M4Elpnuc/BEt2xmP21TGpPRVj2JIhW50ker
dduoNFmhUHXggLm1A0e1WvRcgL6DKxkuUGQGB8W+hOXirf9aWQzT9XC7kqI1uGpc
qJGe6DtzHoHpEt8vZWw67dvSfPFp2Uf1TGsMUg4nbo0frWEqkcdmAafV3/2lduGf
7wkf8KDyf7EZIwT1jSYh1rMI4sj58sSCCMBNTC91aQIDAQABo1MwUTAdBgNVHQ4E
FgQUcJFp7w6FXJ1W6omyock+6rrBfZ0wHwYDVR0jBBgwFoAUcJFp7w6FXJ1W6omy
ock+6rrBfZ0wDwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAgEAcsit
uLJRVA+s0xECYfcyoCo7vwTipQf+Lqo5K77UoNdogEddwVqfeR3rJUpUb3M++3+C
mSKbSylCH5SK3w7/c+D6AYlnbwZa+qb8eA8jPPULeUga4jCnbPke6PlrR5VjPJ4z
CBJNtC2JfTmnP5JU+X9V5w3ItnjH9AcvWLFMKYuTu+S7StmZuVr4pCnJZ82igiSs
SsvYvGjzHwVQ/BZph6QT8/G1nkz+f2VFsmoeV3QxaxhTYen7TO10DPRrp6WThWT9
AiwO5E+Z0mk1zTwwC2SI6i6xwhDAd5mSJHiP399QS5+CMVWXrHfH3QAohLUkH/AT
yW4EalSKjsgoFgZmNtyHqigxSvSZLF9HI/+IOw77rn+OeXt5c+30HKzriWv//v1F
VSb6RIHgTgWcwfa6Ng52QEdwOyyuex5bdqrUfshTmSLKZJMry+Rynxg+ebb8na9y
1k/3sclZgTtwLgo/9XB9M70uhdsbbbxZ4b1V7Zb65nqmNPeq3hy6IKmyBOgvzB7v
fruiomk4EzvVkaG//mgAJ/nUueGf33h2hPqRSBq8uFg6wXHBt3bawB7F8bg+KdN4
movrD3Uso+JwycDkEc/WtfZ65oG4EjYDSnl/mkvs1YimVae7YpZ8MO7oRk4KaFvq
ihftC3KByjP1+IwsZ6c9Opuf76x9ZctiYruuXa0=
-----END CERTIFICATE-----
)EOF";

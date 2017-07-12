# `libxyth` - _Fingerprint identification library_

`libxyth` implements an algorithm for fingerprint identification (1:N) that does not perform successive verifications (1:1).


## `mindtct`
`mindtct` is part of [NBIS](https://www.nist.gov/services-resources/software/nist-biometric-image-software-nbis), and is used to extract *minutiae* from fingerprint images. `mindtct` was commited as a static library (`libmindtct.a`), but this should change in the future.

## Unit Tests
There are several tests using [Check](https://github.com/libcheck/check), a unit testing framework for C.



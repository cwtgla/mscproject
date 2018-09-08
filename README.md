# Craig Thomson MSc Project
Comparison of compression methods for scientific floating point data, targeted towards later use on hardware accelerators

## Pre-requisites
```
	1. ZFP installed (https://github.com/LLNL/zfp)
	2. Set ZFP include and library path in Makefile
```
### Run instructions
note: all code has been tested on Ubuntu 14.04 with gcc 4.8.4
To run code tests:
```
	1. make test
	2. ./test
```
To evaluate compression algorithm overhead and compression ratios:
```
	1. make evaluate
	2. ./evaluate
```
To clean:
```
	1. make clean
````
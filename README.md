# regex-exp

Building regex engine for fun and learn.  
Referrence: [Regular Expression Matching Can Be Simple And Fast](https://swtch.com/~rsc/regexp/regexp1.html) got from [build-your-own-x](https://github.com/codecrafters-io/build-your-own-x?tab=readme-ov-file#build-your-own-regex-engine)

## Building and running
Clone the repo
```sh
git clone https://github.com/kshku/regex-exp.git
cd regex-exp
```
Build using CMake
```sh
cmake -S . -B build
cmake --build build
```
### To build documentation, make sure you have doxygen and run
```sh
cmake --build build --target docs
```
and then you can open the `build/docs/output/html/index.html` in browser.

The executable is built in build directory and named regexer. To run, execute
```sh
build/regexer
```

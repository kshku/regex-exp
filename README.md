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
build/regexer "text" "regex-pattern"
```

## Supported regex meta characters
Literal characters  
Dot(.) -> Matches any single character  
Kleene star(*) -> Zero or more repetition of previous character  
Plus(+) -> One or more repetition of previous character  
Optional(?) -> Zero or one repetition of previous character  
Anchors(\^, \$) -> Matches beginning(\^) or end(\$) of the line  
Character classes([]) -> Matches any of the character or character range specified  
Backslash(\\) -> Escape character  
Alternation(|) -> Matches either the expression on left or expression on right  
### TODO
Grouping(()) -> Groups multiple expressions together to apply operators to the entire group  

## Examples
```sh
build/regexer "somebody saw nobody" "saw"
build/regexer "somebody saw nobody" "sa?w"
build/regexer "somebody sw nobody" "sa*w"
build/regexer "somebody saaw nobody" "sa+w"
build/regexer "somebody sa+w nobody" "sa\+w"
build/regexer "somebody saw nobody" "^some"
build/regexer "somebody saw nobody" "^saw"
build/regexer "somebody saw nobody" "y$"
build/regexer "somebody saw nobody" "^some.*y$"
build/regexer "somebody saeiouw nobody" "s[aeiou]*w"
build/regexer "somebody saeiouw nobody" "s[aeiou]w"
build/regexer "somebody saeiouw nobody" "s[aeiou]+w"
build/regexer "somebody saw nobody" "s[^A-Z]w"
build/regexer "somebody saw nobody" "s[^A-Za]w"
build/regexer "somebody saw nobody" "s[^A-Z]w"
build/regexer "somebody saw nobody" "s[^a-z]w"
build/regexer "somebody saw nobody" "s[^a-A]w"
build/regexer "somebody saw nobody" "s[^a\-A]w"
build/regexer "somebody saw nobody" "s[^\-A]w"
build/regexer "somebody sa]w nobody" "s[]a]+w"
build/regexer "somebody sa]-w nobody" "s[]a-]+w"
build/regexer "somebody sa]-w nobody" "s[-\]a]+w"
build/regexer "somebody saw nobody" "nobody|somebody"
build/regexer "somebody saw nobody" "^somebody|^nobody$"
build/regexer "somebody saw nobody" "^somebody$|nobody$"
build/regexer "somebody saw nobody" "^somebody$|^nobody$"
build/regexer "somebody saw nobody" "somebody$|^nobody$"
build/regexer "somebody saw nobody" "somebody|^nobody$"
build/regexer "somebody saw nobody" "somebody|^nobody"
build/regexer "somebody saw nobody" "^somebody|^nobody"
build/regexer "somebody saw nobody" "somebody$|^nobody"
build/regexer "somebody saw nobody" "somebody$|^nobody|saw"
```

The Simple Program 
------
Your simple trivial C++ program to generate ffmpeg filter strings to save time. As of this moment it is only limited to trimming. It is done parsing a condig file containting the time stamps of your requirement for the trimming

### Compiling and setting it up 

It has a `fmtlib` dependency, so either clone it recursively
```
git clone --recursive https://github.com/worthless443/filtergen_ffmpeg 
```
or 
```
git clone https://github.com/worthless443/filtergen_ffmpeg && cd filtergen_ffmpeg
git submodule update --init
```
you are done with downloading. 

#### compilation and setting it up

Compiling fmtlib

```
source ./trim -compile-fmt
```

Compiling 

```
./trim -compile
```

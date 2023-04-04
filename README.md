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

The `trim` script almost about does everything

Compiling fmtlib

```
source ./trim -compile-fmt
```
Compiling the small file

```
./trim -compile
```
### Useage 
the `./trim` script does about everything, it can also be directly used to generate trimmed videos and extract frames all with a simple config file
```bash
./trim <infile> <outfile> <conf_file> [<colomn from config to extract frame|->]
```
[if is required to generate pictures for each column from the config file,add `-` at the end of argument list.

if you just want to generate the `filter_complex` string, run the C++ binary 
```bash
./genfilter -in <infile> [ -n <column to extract from> ]
```
## Further improvements and additions

* add `overlay` keyword with regular timestamp list to overlay images or text

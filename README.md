The Simple Program 
------
Your simple trivial C++ program to generate ffmpeg filter strings to save time. As of this moment it is only limited to trimming. It is done parsing a condig file containting the time stamps of your requirement for the trimming

### Compiling and setting it up 

It has a [fmtlib](https://github.com/fmtlib/fmt) dependency, so either clone it recursively
```bash
git clone --recursive https://github.com/worthless443/filtergen_ffmpeg 
```
or 
```bash
git clone https://github.com/worthless443/filtergen_ffmpeg && cd filtergen_ffmpeg
git submodule update --init
```
you are done with downloading. 

#### compilation and setting it up

The `trim` script does just about everything (however as of recent changes, still picture extraction has not been tested, we will get to it later)

Compiling fmtlib
[run it with `source`, as succesfull build of `fmt` would set a envvar to mark it as done]
```bash
source ./trim -compile-fmt
```
Compiling the small file
```bash
./trim -compile
```
### Useage 
The resulting binary, namely `filtergen` will generate string that can be passed to `-filter_complex`. It parses a config file, converts input format to the format that which is suitable for ffmpeg and alwats spits out outstream labels as `[cv]` for video and `[ca]` for audio.  
### The syntax 
```
gf:[<imagine_idx>]:<filter_name>
<options>
gf:end
```
You would only need to pass stream index when you are using filters that operate on inputs other than the original video (which always should be the first input followed by peripheral inputs), i.e. the `overlay` filter. An example of which:

```
gf:1:overlay
1.2
1.4
gf:end
```
The options here are formatted as `<hour>.<min>.<sec>`. Overlay only takes two arguments and two only, the first line should corrospond tobegining and second end.

Lastly, run the program with the config file
```bash
./filtergen -in <config-file>
```
You can plug the output to the `-filter_complex` directly or use the `./trim` script for more convinience. However as of now, it's better not to until the script is modified to facilitate the new changes as of now.

**[Use old binary to use ./trim]**
As said earlier, the `./trim` script does about everything, it can also be directly used to generate trimmed videos and extract frames all with a simple config file
```bash
./trim <infile> <outfile> <conf_file> [<colomn from config to extract frame|->]
```
[if is required to generate pictures for each column from the config file,add `-` at the end of argument list.

## Further improvements and additions

- [x] add `overlay` keyword with regular timestamp list to overlay images or text
- [ ] add position options in the `overlay` keyword
- [ ] add fade in and out options to overlay
- [ ] add external audio options

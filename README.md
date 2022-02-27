# cerebellum
A brainf*** compiler built in C++ for windows to learn the basics of compiling code
version: 1.1.2

## usage
```
cerebellum [-o/--option=arg] <program.bf>
Options:
[-v/--verbose]
[-V/--version]
[-o/--output=<output_file>]
```
Currently, only object building is supported. In the future, it will automatically link the file to the needed dlls.  
The only dll currently used is `kernel32.dll`  
I have been testing with golink. The go link command is in the coff `tests` bat file.  

## components
There are a few components making up cerebellum. The first is `cough`, which creates the coff objects to be linked. The second component is the assembler, which is a system designed to simplify writing assembly code for the compiling stage.  
These projects can be used on their own. I have a `cough` repository already up on this github, though it is not up to date. The assembler project will be put up when I get to it.  

## TODO
 - Find a way to convert each instruction into assembly
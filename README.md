# WinShell

## Features
This was supposed to be a basic shell designed for windows. Now it has become a collection of tools I have made, ranging from a windows based shell to a file compressor.

Winshell features the following shell commands : cd, echo, exit, type, pwd, help, complete (-C, -d, -f and -p flags only and for now only .exe completer executables, and bash scripts if you have git installed, for now), jobs, history and is capable of finding executables from path variable in type, executing them when called and run processes as well as some shell commands in the background. Plus I've added basic file/error buffer redirections and autocomplete for shell commands and executables. I've also added support for basic functions like tab(autocomplete) and up/down/left/right arrow keys.

cat is basically the linux cat tool reimplemented, it served primarily a testing purpose for Winshell.

wc is the word count tool that supports the -c, -l, -m and -w options.

grep is , you guessed it right, grep reimplemented in cpp with cpp's regex engine(I'll implement my own regex engine in a later project).

compress is a huffman based encoder-decoder to compress your files. It generates a .cmpr file that only the tool can decode.

## How to set up and build
You can build via two ways (I just added cmake just in case the project becomes complicated in the future.)

- Clone the repo using git clone.
```bash
git clone https://github.com/bugger69/WinShell.git
cd WinShell
```

### Using CMake
- Make sure you have cmake installed, and now create a build dir via the following commands.
```bash
mkdir build
cd build
```
- Now if you're using nmake, you can directly run the following:
```bash
cmake ..
cmake --build .
```

- If like me you're using MinGW g++, you'll have to do it this way
```bash
cmake -G "MinGW Makefiles" ..
cmake --build .
```

### Compiling directly
Now since we've added support for modules, cmake method stays the same but in this method you'll have to compile each module seperately.
- You can execute the following:
```bash  
g++ -Iinclude src/main.cpp src/shell_cmds.cpp src/utils.cpp src/parser.cpp src/shell_execute.cpp src/shell_read.cpp src/Trie.cpp src/pipe_stream -o WinShell.exe
cd modules
g++ hello/main.cpp -o hello.exe
g++ cat/main.cpp -o cat.exe
```
### Running the program
- And no matter which way you compiled, run:
```bash
WinShell.exe 
```

### Installation
Per User Installation(replace 'build' with '.' if you're intalling from inside the build directory):
```bash
cmake --install build --prefix "%LOCALAPPDATA%\WinShell\bin"
```
powershell command:
```sh
cmake --install build --prefix "$env=LOCALAPPDATA\WinShell\bin"
```

For admin(root, requires permissions) installation:
```bash
cmake --install build --prefix "%ProgramFiles%\WinShell\bin"
```
powershell command for the same:
```sh
cmake --install build --prefix "$env=ProgramFiles\WinShell\bin"
```
Note: You have it add the respective paths to the path variable yourself atm, will add a installer or something soon.




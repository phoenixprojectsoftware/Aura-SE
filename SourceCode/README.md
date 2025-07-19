# ![Aura logo](../aura.png "aura logo")
# Welcome to the Aura-SErver SourceCode.
# Building
## Windows
To build this DLL on Windows, you must use Visual Studio 2022 with the **Desktop Development for C++** package installed. The Windows SDK Version shouldn't matter, and the latest v143 compilers are used.

The solution is stored in `projects/vs2022/Aura.sln`. It has two simple configs, Debug and Release.

## Linux & WSL2
Linux can be a fussy OS, but we must compile for it nonetheless. Hey, that rhymed.

In the `linux` folder you'll find everything you need. Open a terminal there and run the following steps.
### Required thingies
Use `sudo apt-get` to install the following packages:
```
  build-essential
  gcc
  clang
  libc6-dev-i386
  gcc-multilib g++-multilib
  dos2unix
 ```
### Building
1. Especially if you're on WSL2, you need to convert the `gendbg.sh` to Unix format and add execution permissions.
	- `dos2unix gendbg.sh`
	- `chmod +x gendbg.sh`
2. Then, you can run **`make aura`** to build `aura_linux.so`. It will be popped in the `release/` subdirectory alongside a debug file - use if you want, but keep in mind of course that we don't RTR debug files so it will inevitably be removed during the process if you push it in the main game repository.
	- if you want a fresh start run `make clean`.
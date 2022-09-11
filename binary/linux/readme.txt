This directory contains all the files needed to build Aura Serverside on Linux.

To start, cd to the directory in the terminal.

You will need to install the following packages using sudo apt install:
  build-essential
  gcc
  clang
  libc6-dev-i386
  gcc-multilib g++-multilib
  
once you have installed those packages you can build:

  make aura
  
to do a clean build do make clean then do make aura again.

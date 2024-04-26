## UID: 305932226

## Pipe Up

The Pipe Up program resembles the pipe operator of the shell. It allows the piping of ouputs of a program to the input of another.

## Building

To build, run `make`

## Running

./pipe ls wc
    6       6      51
./pipe
Error: No arguments
./pipe invalid program
execlp: No such file or directory
Child exits with error
./pipe ls cat wc
    6       6      51

## Cleaning up

To clean up built files, run `make clean`

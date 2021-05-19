# Phantom

Phantom is a scripting language I have made for my final year university project.
The focus of this project was to make a just in time compiler. The language itself
is very minimal but that was because the focus of the project was on the compiler
and not implementing a feature complete language.

Although the university project has come to an end development on Phantom has not.
New features, additions, optimisations, and reworks are planned for Phantom and
the current end goal is to have a feature complete embeddable scripting language.


# Running Phantom

## Windows
Included in this repository is a Visual Studio solution to be able to run the
project. By default running the project will run it in REPL mode but if you want
to run any of the test scripts just right click on the project in Visual Studio,
click on properties, then click on debugging, and then under command line arugments
put the path to the test script you want to run. You can also use this snippet:

$(SolutionDir)..\tests\num_guess.ptn


## Linux
A makefile is included in the project but just due to time constraints the project
works better on a Windows platform as development of other assessments for my
other University modules was on the Windows platform and I did not have time to
update Phantom to run on Linux. Running on Windows is recommended. Linux support
will be updated as development continues.

## Mac
Mac is not supported currently.


# SDA OS
*The best (and only) OS for your SDA.*

This repo contains most of the code that runs the SDA. More on that here [SDA project on Hack a Day](https://hackaday.io/project/35165-sda-the-best-new-pda)

You can try the SDA_OS in a work-in-progress browser-based [simulator](https://miskribu.cz/SDA_SIM/SDA_OS.html).

## What is this
This is the part of SDA_OS that is "shared" between all the platforms and devices. It contains main GUI elements and higher functions. It extends the [SVS](https://github.com/stanislavbrtna/svs-script) interpreter and uses it to run the SDA_OS applications.

## Getting Started
Do not clone/download just this! You must have base drivers for SDA_OS to run it on your device (PC or a real SDA).

Don't forget to update the submodules after pull:
git submodule update --init --recursive --force --remote

Then add SVS platform_specific.h file to the SVS folder containing:

#ifndef SVS_PLATFORM_H
#define SVS_PLATFORM_H

#if defined(STM32F405xx)
#include "../SDA_OS.h"
#endif

#endif

## Application development for SDA
SDA applications are written in SVS language using APIs of SDA_OS, documentation on the language can be found in its repo [SVS script syntax](https://github.com/stanislavbrtna/svs-script/blob/master/SYNTAX.md) and documentation for the SDA_OS API is here.

### SDA_OS SVS wrappers
SDA_OS uses wrappers to provide its functions to applications running in the SVS environment. These wrappers extends the **sys** keyword and they have automatically generated documentation. This API is quite stable and its intended to have long backward compatibility.

- [Main functions](Docs/sda_main.md)

- [GUI](Docs/sda_gr2_wrapper.md)

- [Files](Docs/sda_files.md)

- [Time](Docs/sda_time.md)

- [Overlays](Docs/sda_overlays.md)

- [Graphics](Docs/sda_directS.md)


### SDA_OS native API
Native (C) API of SDA_OS is not strictly defined yet and it is work-in-progress. Expect breaking changes.

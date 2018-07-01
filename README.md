# Introduction
This repo contains a utility, written in C, which will take the binary output of [log-client](https://github.com/u-blox/log-client) and convert it to human-readable form.

# Building
Before you can build this utility, you must add four files to it from the repo which generated the log you wish to convert:

1. `log_enum.h` and `log_enum_app.h`: these will be contained in the [log-client](https://github.com/u-blox/log-client) sub-directory of your application's repo.
2. `log_strings.h` and `log_strings_app.h`: contained somewhere in you application's repo, you should know where.

Once you have cloned this repo and copied these files into it, build it on a Windows machine with Microsoft Visual Studio (C++) (2010 or later), or build it on Linux by entering:

`sudo make`

You should end up with a `log-converter` executable. On Linux you may need to make this executable with `sudo chmod +x log-converter`.  For usage instructions, run `log-converter` with no parameters.

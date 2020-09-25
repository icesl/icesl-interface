# IceSL Interface

This is the official repository of IceSL plugins' interface. This document details how to set-up the repository and the basics of creating a plugin.

* The official IceSL website is [here](https://icesl.loria.fr/).

* For the complete list of IceSL slicing settings, follow this [link](https://icesl.loria.fr/parameters/).

* Questions about this interface as well as the inner workings of IceSL should be directed to our [mailing group](https://groups.google.com/forum/#!forum/icesl).

## Disclaimer

This repository is **private** and isn't open to the **general public**. If you have encountered/downloaded this repo without the author's permission, we kindly ask you to report it to [The MFX team](mailto:mfx@inria.fr).

## The Interface

An IceSL plugin is a shared library file (`.dll` on Windows platforms and `.so` on Linux OS's). This file should be placed in the folder `icesl-plugins` which is contained in the IceSL's installation directory. Rhe option _Use plugins_ shoould be activated in IceSL.

These plugin files will be automatically picked up by IceSL at startup. They, however, could be ignored for the following two reasons:
- Missing or incorrect shared interface functions.
- Interface version mismatch.

Make sure you're using the latest interface as well as the latest version of IceSL.

## Setting up the repository

1. Clone the repository from this link: [https://gitlab.inria.fr/mfx/icesl-interface.git](https://gitlab.inria.fr/mfx/icesl-interface.git)
2. Checkout the master branch:
    1. `cd IceSL-Interface`
    2. `checkout master`
3. Checkout the submodules:
    1. `git submodule sync --recursive`
    2. `git submodule update --init --recursive`
    3. `git submodule foreach checkout master`
    4. `git submodule foreach git pull`
4. Execute CMake on the repo:
    * Either use CMake-gui on the _IceSL-Interface_ folder
    * Or execute in your build folder `cmake /src_dir/IceSL-interface` where `src_dir` is the folder where you cloned the repo
5. Compile the sample plugin:
    * Open the resulting `Plugin-Interface` project in your IDE of choice and compile the module `sampleInfillerPlugin`
    * Or execute `make sampleInfillerPlugin` on the build folder

If everything goes well, the output of the sample plugin should be in `/src_dir/IceSL-interface/bin`, either as a `.dll` or a `.so` file.

## Creating your own plugin


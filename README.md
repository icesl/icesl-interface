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
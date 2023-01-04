# IceSL Interface

This is the official repository of IceSL plugins' interface. This document details how to set-up the repository and the basics of creating a plugin.

* The official IceSL website is [here](https://icesl.loria.fr/).

* For the complete list of IceSL slicing settings, follow this [link](https://icesl.loria.fr/parameters/).

* Questions about this interface should be mailed to [The MFX team](mailto:mfx@inria.fr).

* Questions about the inner workings of IceSL should be directed to our [mailing group](https://groups.google.com/forum/#!forum/icesl).

## Disclaimer

Access to this repository is open to the **general public**. If you have problems accessing/cloning this repo we kindly ask you to contact [The MFX team](mailto:mfx@inria.fr).

## The Interface

An IceSL plugin is a shared library file (`.dll` on Windows platforms and `.so` on Linux OS's). This file should be placed in the folder `icesl-plugins` which is contained in the IceSL's installation directory. The option _Use plugins_ should be activated in IceSL.

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
    3. `git submodule foreach git checkout master`
    4. `git submodule foreach git pull`
4. Execute CMake on the repo:
    * Either use CMake-gui on the _IceSL-Interface_ folder.
    * Or execute in your build folder `cmake /src_dir/IceSL-interface` where `src_dir` is the folder where you cloned the repo.
5. Compile the sample plugin:
    * Open the resulting `Plugin-Interface` project in your IDE of choice and compile the module `sampleInfillerPlugin`.
    * Or execute `make sampleInfillerPlugin` on the build folder.

If everything goes well, the output of the sample plugin should be in `/src_dir/IceSL-interface/bin`, either as a `.dll` or a `.so` file.

## Creating your own plugin

For minimal details not covered here, please try first looking how it was done in the sample plugin provided with the repository. Should this fail, please try to contact [the authors](mailto:mfx@inria.fr).

1. First decide the type of plugin to be targeted (e.g., new infill, lua extension, gcode wrapper, etc.).
2. Create a plugin class derived from the according intermediate class (e.g., `class MyInfillerPlugin : public IceSLInterface::IInfillerPlugin`)
    * More intemediate classes will be added with time. If you can't find one apt to your goals, please contact us.
3. More often than note, an intermediate interface also has to be implemented (for memory managment reasons). Please create an interface derived from the according intermediate interface (e.g., `class MyInfillerInterface : public IceSLInterface::IInfillerInterface`).
4. Implement the shared interface to be used by IceSL. It consists of two functions, both declared in the header file and implemented in the source file:
    1. `extern "C" ICESL_PLUGIN_API IceSLInterface::IPlugin * createPlugin();`
    2. `extern "C" ICESL_PLUGIN_API void destroyPlugin(IceSLInterface::IPlugin* plugin);`

### General advice

* Override the information functions of the base plugin class `IPlugin` to give relevant information to IceSL:
    1. `virtual std::string name() const;`
    2. `virtual std::string author() const;`
    3. `virtual std::string comment() const;`
* Try to manage your plugin global resources by overriding the according functions from the base class `IPlugin`. A plugin instance is alive during the whole execution of IceSL, thus these resources are as well.
    1. `virtual bool initialize(IPluginEnvironment &env);`
    2. `virtual void dispose();`
* To manage slicing settings, the interface `EnumerableSettingsInterface` can be used:
    * To query a setting, use the function `getSettingByName`.
        * Once a setting as been queried, you can get its value, type or other information using the `EnumerableSettingsInterface::SettingInterface` interface.
    * To add a setting, use the function `addSettingFromPlugin` or `addSelectionFromPlugin`.
* For resources that are only used during execution invervals (e.g., each slice, execution of a lua function) try to manage them in the intermadiate plugin interface mentioned above.
* All source interfaces (e.g., `IPlugin`, `IInfillerPlugin`, `IInfillerInterface`, `IPath`) are properly documented in the source files. If something is not completely clear, try reading these comments.

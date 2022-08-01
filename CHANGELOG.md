# Changelog

## [2.1.2] - 2022-07-12

### Fixed

- Fixed missing source files (...Datamodel.cpp and ...Logger.cpp) in CMakeLists.txt for Linux C++ template generation

## [2.1.1] - 2022-04-26

### Fixed

- Fixed bug introduced in 2.0.3 when creating a component from a .typ file, where the Package.pkg file (in AS) was updated incorrectly (having multiple definitions of the package, as well as the original .typ file which had been deleted from the disk).

## [2.1.0] - 2022-04-17

### Added

- Added "Run exOS data connection" to simplify the local debugging of exOS packages. The update is done via the exospkg file, as this has the reference to the used WSL distro.

## [2.0.3] - 2022-04-07

### Added

- Added possibillity to add deploy-only Linux component by right-clicking on a folder. Usecase is for creating node/python/C/C++ applications that are only running in Linux.

- Added "update WSL build environment" to simplify the update towards a new technology package. The update is done via the exospkg file, as this has the reference to the used WSL distro.

### Fixed

- Support for multiline comments in .typ files
- Fix missing structs in swig i-file - a combination of array and nested structs could result in missing structs
- Fixed missing EXOS_ASSERT and ERROR messages on dataset/publish errors in the c-static template, that errors become visible in the logger.

## [2.0.2] - 2022-01-25

### Added

- Additional images in the README, and updated chapter regarding python and nodejs installation.

### Changed

- Changed default python version to 3 instead of previous version 2 for newly generated python templates.

### Fixed

- Update mechanism did not work correctly in V2.0.1 because of the name changes.
- Added needed include of stdlib.h in the libxx.c file created by the N-Api generator.
- Max 10 character limitation was added to the GenerateDatamodel command which did not have this limitation before.

## [2.0.1] - 2021-12-14

### Added

- Added gitignore files in the generated packages, especially when handling sources being source controlled in AS and Linux. AS4.11 will have a feature to ignore these files at build (currently it shows a warning)
- Added GCC6.3 check for AS C++ template. This template does not build with GCC4.1.2 because of the newer language features used.

### Changed

- ENUMs are now treated as values in the AS code parts, before they were trated as structures (and compared via memcmp). This has no effect on the runtime functionality.
- NAPI applications are now started using node instead of npm. With this, the target system only needs to install nodejs, and not npm - which should not be needed there.
- NAPI main application is now called e.g. watertank.js instead of index.js, in order to streamline the look-and-feel with other templates (e.g. watertank.py)
- Update is now called "Update All" rather than "Reset" and "Recreate" rather than "Force" because the previous names were misleading

### Fixed

- NAPI template had a timing issue with non-synchronous JS callbacks, that a burst of value changes from AR could result in calling the onChange several times with the same (last) value. Values are now stored internally so that each onChange gets the corresponding (correct) value
- Max 10 character limitation was added to certain library / type / fun files which did not have this limitation before.
- exos_log_init had the wrong "alias-name" when reinitializing AS templates after a download (i.e it was using "WaterTank_0" rather than "gWaterTank_0"). Only has an effect on the filter in the AS Logger.

## [2.0.0] - 2021-12-01

### Changed

- Major rework of all template generators, collecting all specific functionalities into different classes that are used across the various templates
- Templates are now only returning generated source code, and do not create CMakeLists, Packagefiles etc. which is handled in the other classes

### Added

- Added exOS Debug Console as an easier way to access the AS Logger (with additional information) from the VsCode terminal window

## [1.2.0] - 2021-05-31

### Added
- Support for arrays in SWIG python.
- Example code in on_changed in python and singleton
- Update template available for NodeJS NAPI

### Fixed
- Naming of structs in singleton and swig had the library name removed (only in version 1.1.5 and 1.1.6)

### Removed
- SWIG NodeJS as it has been replaced by the more feature rich N-API NodeJS

## [1.1.6] - 2021-04-20

### Fixed

Singleton templates did not create publishers for the Linux side, which meant datasets could be subscribed to in AR but were never sent from Linux.

## [1.1.5] - 2021-04-16
### Changed

- Changed context menu commands to "Instantiatable" and "Singleton"

- Streamlined the AR parts so that all Templates use a Dynamic library with the reference to an external variable of the interface-datatype. 

- Singleton (previously static-c-lib) template log commands have been moved into a log structure rather than having them in the top structure as log_error log_warning and so on.
### Added

- Node.js N-API generator added. Uses the Node-API version 6. Some limitations are known and is mentioned in top of the created Linux/lib"structname".c file. Major things are that value ranges are not checked, multidimentional arrays are not supported and that error checking is not yet fully implemented in a nice way.
Tested with Node.js version 10 (latest).  
Needs node-gyp, python 3.x, npm to be installed in the build environment (WSL).

- Updated SWIG NodeJS template to allow for node-modules like the Node-API version, as well as having CMake creating a debian package instead of loose files.

- Added more description in the README
## [1.1.4] - 2021-03-04
### Added
- This changelog (copied earlier entries from Readme/Details), lets try this: [Keep a Changelog](http://keepachangelog.com/)
- `Update Static C-Lib SWIG Python` to right-click on .exospkg file
- Log functionality as function pointers like process, dispose etc. in the static C-lib including SWIG
- `get_nettime` function as a function pointer like process, dispose etc. in the static C-lib including SWIG
- `nettime` to each SUB value which is updated in the datasetEvent callback in the static C-lib including SWIG
- A local `Connected` variable to the AR template, cyclically updated
- Generation of .gitignore and .gitattributes

### Changed
- Python version in CMakeLists.txt to PythonLibs 3

### Fixed
- Generation of array and string sizes in function blocks
- Wrong datatype for strings was used in the static C-lib including SWIG
- Fixed `is_connected` and `is_operational` flags to be updated in `disconnect` and `dispose` in the static C-lib including SWIG
- - Events doesnt arrive when actively disconnecting (by api design)

## V1.1.3

### added NodeJS SWIG Template

The `exOS Static C-Lib SWIG NodeJS Template` is now available for creating nodejs JavaScript templates. SWIG needs to be installed in the build environment via `sudo apt install swig`. Currently callbacks are not supported, and generator is restricted to node version <= 10.

## V1.1.2

### added python SWIG template

The `exOS Static C-Lib SWIG Python Template` is now available for creating python templates, which also handles callbacks. The callbacks are auto generated in the python scripts. SWIG needs to be installed in the build environment via `sudo apt install swig`

## V1.0.5

### Static C-Lib template added

There is a new function `exOS Static C-Lib Template` which generates a template with a static C-Library representing the configured datamodel for C-tasks on both ends. The benefit of this library is that it allows for being updated (right clicking in the .exospkg file), creating a new static library from the datamodel, without changing the main functions. Also a good starting point to generate SWIG templates.  

### renamed exar- to exos-comp-

Previously, the Debian packages created for deployment to the Linux system were called `exar-`, for example `exar-watertank-1.0.0.deb`. As this prefix had no real meaning, it has been changed to `exos-comp-` for all new templates created, meaning you would get `exos-comp-watertank-1.0.0.deb`, which is a more telling prefix, given that it is an exos component that is being deployed and installed on the remote system.
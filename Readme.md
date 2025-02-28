# FarField

FarField is an open source program for far-field sonic boom prediction using quasi-linear Whitham theory. Program is based on OpenFOAM.

## Disclaimer

This offering is not approved or endorsed by OpenCFD Limited, producer
and distributor of the OpenFOAM® software via www.openfoam.com, and owner of the
OPENFOAM® and OpenCFD® trade marks.

## Installation

Dependencies:
* OpenFOAM 2312. Download and install OpenFOAM 2312 from www.openfoam.com. FarField was not tested with other versions of OpenFOAM.

* catch2. To install on Ubuntu 24.04:
```
sudo apt install catch2
```

Build:
```bash
cd src
./DebugMakeAll
```

## Usage

1. Create `farFieldProperties` file dictionary in `constant` directory of the case. For more details see `tutorials` directory.
1. Run `farFieldFoam` program in case directory.

### Verification and Validation

FarField has been validated against data from 1st and 2nd NASA sonic boom workshops for models: seeb-alr, delta-wing, c25d.
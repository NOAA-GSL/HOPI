# HOPI
High Order Parallel Interpolation

## Contents
* **[Introduction](#introduction)**
* **[Getting Started](#getting-started)**
* **[Documentation](#documentation)**
* **[Status](#status)**
* **[Contributing](#contributing)**
* **[License](#License)**


## Introduction
HOPI is a [C++20](https://en.wikipedia.org/wiki/C%2B%2B20) library for
parallel interpolation of quantities using high order radila basis functions.

## Getting Started

### Building Using CMake
The build system uses traditional CMake commands with 
only a handfull of user defined variables to control the 
library features.  The default selections are shown in **bold**.

Performance Macros:

| Variable                     | Values     | Function                          |
| ---------------------------- |:----------:|:---------------------------------:|
| HOPI_USE_NO_UNIQUE_ADDRESS   | **ON**:OFF | Use C++20 [[no_unique_address]]   |
| HOPI_USE_INLINE              | **ON**:OFF | Inline Marked Functions           |
| HOPI_USE_FORCE_INLINE        | **ON**:OFF | Force Inline Marked Functions     |

Features to Build:

| Variable                     | Values     | Function                          |
| ---------------------------- |:----------:|:---------------------------------:|
| HOPI_BUILD_UNIT              | **ON**:OFF | Build unit tests                  |
| HOPI_BUILD_SCRATCH           | ON:**OFF** | Build scratch tests               |
| HOPI_BUILD_DOXYGEN           | ON:**OFF** | Use Doxygen to generate docs      |

In addition to the helpfull CMake variables:

| Variable                | Values                                              |
| ----------------------- |:---------------------------------------------------:|
| CMAKE_COLOR_MAKEFILE    | **ON**:OFF                                          |
| CMAKE_VERBOSE_MAKEFILE  | **ON**:OFF                                          |
| CMAKE_MESSAGE_LOG_LEVEL | ERROR:WARNING:NOTICE:STATUS:**VERBOSE**:DEBUG:TRACE |
| CMAKE_BUILD_TYPE        | **Release**:Debug:RelWithDebInfo:MinSizeRel         |

Example build sequence:

```bash
> mkdir build; cd build
> cmake .. -DCMAKE_BUILD_TYPE=Debug
> make install 
> make unit-tests
> make test
```

## Documentation

## Status

## Contributing
The HOPI project welcomes contributions from new developers. Contributions can be offered through the standard GitHub pull request model. We strongly encourage you to coordinate large contributions with the HOPI development team early in the process.

## License
See [License](LICENSE) file

# rle_string

Simple Implementation of a Run Length Encoded String. Heavily inspired from https://github.com/nicolaprezza/r-index/blob/master/internal/rle_string.hpp


## Docker
`rleinfo` is available on docker:

```bash
docker pull moliva3/rle_string:latest
docker run moliva3/rle_string:latest rleinfo --help
```

If using singularity:
```bash
singularity pull rlestring_sif docker://moliva3/rleinfo:latest
./rlestring_sif rleinfo --help
```

## Build
`rleinfo` can be built using:

```shell
git clone https://github.com/marco-oliva/rle_string.git
cd rle_string
mkdir build && cd build
cmake ..
make 
```

## How to use `rleinfo`
`rleinfo` reports the number of runs and the size of a run length compressed string. This software is meant to 
help debugging tools written using the `rle` library.

## Usage
We report here all the available parameters for `rleinfo`

```shell
rleinfo
Usage: rleinfo [OPTIONS] input...

Positionals:
  input TEXT:FILE ... REQUIRED
                              List of RLE strings.

Options:
  -h,--help                   Print this help message and exit
  --configure                 Read an ini file
```
# Introduction

For SIP server code practice.

# Directory

```
 ├── sipServer       # root path
 │   └── sipServer   # sipServer main code
 │   ├── testDemo    # third party library test code
 │   ├── thirdParty  # third party library
```

# build

``` command
 1. cd ./sipServer
 2. cmake -B build
 3. cmake --build build
```

# Config

``` command
 template config file in: ./sipServer/conf/servCfg.ini 
```

# Log

``` command
 log directory in: ./sipServer/log/
```

# clean

``` command
 1. rm -rf ./sipServer/build
 2. rm -rf ./sipServer/bin/sipServer
```

# Usage

``` command
 ./bin/sipServer --cfg ./bin/servCfg.ini
```

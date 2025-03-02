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

# clean

``` command
 1. rm -rf ./sipServer/build
 2. rm -rf ./sipServer/bin/sipServer
```

# run

``` command
 ./bin/sipServer --cfg ./bin/servCfg.ini
```

# exit

``` command
 1. ctrl + \ (recommand)
 2. ctrl + c (not recommand. will cause error "Segmentation fault")
```
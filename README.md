# Introduction

For SIP server code practice.

# Note
 1. support  sip upstream and downstream service interaction. see example: sipServer_rootPath/sipServer/testDemo/11_sipServer_test

# Directory

```
 ├── sipServer       # root path
 │   └── sipServer   # sipServer main code
 │   ├── testDemo    # third party library test code
 │   ├── thirdParty  # third party library
```

# build

``` command
 1. cd sipServer_rootPath/sipServer
 2. cmake -B build
 3. cmake --build build
```

# clean

``` command
 1. rm -rf sipServer_rootPath/sipServer/build
 2. rm -rf sipServer_rootPath/sipServer/bin/sipServer
```

# run

``` command
 ./sipServer_rootPath/bin/sipServer --cfg ./sipServer_rootPath/bin/servCfg.ini
```

# exit

``` command
 1. ctrl + \ (recommand)
 2. ctrl + c (not recommand. will cause error "Segmentation fault")
```
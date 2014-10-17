openambit_cli
=============

Command line tool based on openambit project

## Install the udev rules

```
$ wget https://raw.githubusercontent.com/openambitproject/openambit/master/src/openambit/deployment/99-suunto-ambit.rules
# cp 99-suunto-ambit.rules /etc/udev/rules.d/
```

## Setting

Set the right patch for #define DATA_PATH on openambit_cli.cpp

For example: /home/my_user/.openambit/data

## Build

Go to your 'build' directory.

```
git clone https://github.com/jbtheou/openambit_cli.git
cd openambit_cli
git clone https://github.com/jbtheou/libmovescount.git
git clone https://github.com/jbtheou/libambit.git
mkdir build libs
cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=../libs/ ../libambit
make
make install
cd ..
make
```
## Test your installation

With your watch connected:

```
./openambit_cli --model
```

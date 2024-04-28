# vedirect-c
Packet parser written in C for the Victron VE.Direct Protocol

# Overview
The `source` directory contains a library written in C which can process a stream of incoming
data and detect VE.Direct packets. Checksums will be performed on the data and once a frame is
ready, the data can be accessed.

The data isn't actually converted into types or parameters, instead it is left as an array
of key-value pairs in string format. e.g Key = "LOAD" Value = "ON".
Converting into meaningful types is outside of the scope of this lib, since it's original 
purpose was to be used with an application which already contains the conversion from key/value 
to typed parameters (https://github.com/KinDR007/VictronMPPT-ESPHOME).


The `examples` directory contains an application `vedapp` which will read data from
stdin and print complete data frames after parsing is finished. This serves as a way to test 
the library and an example for anyone who wishes to make use of the library.


# Build
Steps to build the example app:

Navigate to the `vedapp` directory
```
cd examples/vedapp
```

Build using make
```
make
```
If successful, the application binary `vedapp` will have been created.


# Usage
The example app will parser any data piped into the application.
To test this, the example VE.Direct data in `resources/victron-log.bin` can be used.
```
cat resources/victron-log.bin | ./examples/vedapp/vedapp 
```
The output should match the file `resources/victron-log-parsed.txt`


This app can also be used with a direct connection to a Victron unit via a serial port
```
cat /dev/ttyS0 | ./examples/vedapp/vedapp 
```
Replace `/dev/ttyS0` with your serial port
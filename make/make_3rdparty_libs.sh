#!/bin/sh

# mkdir ../dependence
# mkdir ../dependence/libs
# mkdir ../dependence/inc

CURRENT_DIR=${PWD}
ROOT_DIR=${CURRENT_DIR}/..
echo "Script executed from: ${CURRENT_DIR}"

# rabbitmq-c
#------------------
echo "make rabbitmq-c ... "
cd ../engine/3rdparty/rabbitmq-c

mkdir build 
cd build

cmake -DCMAKE_INSTALL_PREFIX=${ROOT_DIR}/dependence/rabbitmq-c ..
cmake --build . --target install

# amqpcpp
#------------------
echo "make amqpcpp ... "
cd ${ROOT_DIR}/engine/3rdparty/amqpcpp

mkdir build 
cd build

cmake -DCMAKE_DEPENDENCE_PATH=${ROOT_DIR}/dependence ..
cmake --build . --target install

# cmake .. -DMY_VARIABLE=0

# qpid-proton
#------------------
cd ${ROOT_DIR}/engine/3rdparty/qpid-proton

mkdir build 
cd build

cmake .. -DBUILD_EXAMPLES=0 -DBUILD_STATIC_LIBS=1 -DPROACTOR=libuv -DCMAKE_INSTALL_PREFIX=${ROOT_DIR}/dependence/qpid-proton
cmake --build . --target install

# logging-log4cxx
#------------------
# brew install apr
# brew install apr-util

# cd ${ROOT_DIR}/engine/3rdparty/logging-log4cxx

# mkdir build 
# cd build

# # -DBUILD_SHARED_LIBS=0
# cmake ..  -DBUILD_SHARED_LIBS=0 -DCMAKE_INSTALL_PREFIX=${ROOT_DIR}/dependence/logging-log4cxx
# cmake --build . --target install

# log4cpp
#------------------

cd ${ROOT_DIR}/engine/3rdparty/log4cpp
make clean
./configure --prefix= --enable-threads=yes --enable-shared=no --enable_debug=yes

make
make check
make DESTDIR=${ROOT_DIR}/dependence/log4cpp install

# mkdir build 
# cd build

# -DBUILD_SHARED_LIBS=0
# cmake ..  -DCMAKE_INSTALL_PREFIX=${ROOT_DIR}/dependence/log4cpp
# cmake --build . --target install
#!/bin/sh

# mkdir ../dependence
# mkdir ../dependence/libs
# mkdir ../dependence/inc

CURRENT_DIR=${PWD}
ROOT_DIR=${CURRENT_DIR}/..
echo "Script executed from: ${CURRENT_DIR}"

# # rabbitmq-c
# #------------------
# echo "make rabbitmq-c ... "
# cd ../engine/3rdparty/rabbitmq-c

# mkdir build 
# cd build

# brew install openssl
# brew link --force openssl

# cmake -DCMAKE_INSTALL_PREFIX=${ROOT_DIR}/dependence/rabbitmq-c -DOPENSSL_ROOT_DIR=/usr/bin/openssl ..
# cmake --build . --target install

# # amqpcpp
# #------------------
# echo "make amqpcpp ... "
# cd ${ROOT_DIR}/engine/3rdparty/amqpcpp

# mkdir build 
# cd build

# cmake -DCMAKE_DEPENDENCE_PATH=${ROOT_DIR}/dependence ..
# cmake --build . --target install

# # cmake .. -DMY_VARIABLE=0

# # qpid-proton
# #------------------
# cd ${ROOT_DIR}/engine/3rdparty/qpid-proton

# mkdir build 
# cd build

# brew install libuv

# cmake .. -DBUILD_EXAMPLES=0 -DBUILD_STATIC_LIBS=1 -DPROACTOR=libuv -DCMAKE_INSTALL_PREFIX=${ROOT_DIR}/dependence/qpid-proton
# cmake --build . --target install

# # logging-log4cxx
# #------------------
# # brew install apr
# # brew install apr-util

# # cd ${ROOT_DIR}/engine/3rdparty/logging-log4cxx

# # mkdir build 
# # cd build

# # # -DBUILD_SHARED_LIBS=0
# # cmake ..  -DBUILD_SHARED_LIBS=0 -DCMAKE_INSTALL_PREFIX=${ROOT_DIR}/dependence/logging-log4cxx
# # cmake --build . --target install

# # log4cpp
# #------------------

# cd ${ROOT_DIR}/engine/3rdparty/log4cpp
# # make clean --enable_debug=yes
# ./configure --prefix= --enable-threads=yes --enable-shared=no 

# make
# make check
# make DESTDIR=${ROOT_DIR}/dependence/log4cpp install

# # boost
# #------------------
# if [[ -z "$BOOST_DIR" ]]; then
#   echo "Not found BOOST_DIR variable. You need set it to ~/.bash_profile"
#   exit 0
# else
#   echo "BOOST_DIR has the value: $BOOST_DIR"
#   cd "$BOOST_DIR"
#   ./bootstrap.sh
#   ./b2 install --libdir="${ROOT_DIR}/dependence/boost/lib" --includedir="${ROOT_DIR}/dependence/boost/include"
#   cd ${CURRENT_DIR}
# fi

# # zstd
# #------------------
# cd ${ROOT_DIR}/engine/3rdparty/zstd-1.5.2
# make clean
# # ./configure --prefix=

# make prefix=
# make prefix= DESTDIR=${ROOT_DIR}/dependence/zstd install


# qpid-proton
#------------------
cd ${ROOT_DIR}/engine/3rdparty/rapidjson

mkdir build 
cd build

cmake .. -DCMAKE_INSTALL_PREFIX=${ROOT_DIR}/dependence/rapidjson
cmake --build . --target install

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

cmake -DCMAKE_INSTALL_PREFIX=../../../../dependence/rabbitmq-c ..
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
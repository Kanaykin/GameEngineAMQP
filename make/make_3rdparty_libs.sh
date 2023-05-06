#!/bin/bash

# mkdir ../dependence
# mkdir ../dependence/libs
# mkdir ../dependence/inc

CURRENT_DIR=${PWD}
ROOT_DIR=${CURRENT_DIR}/..
echo "Script executed from: ${CURRENT_DIR}"

declare -a params=()

declare -a params_all=(
	"rabbitmq-c" "amqpcpp" "qpid-proton" "log4cxx" "boost" "zstd" "rapidjson" "rapidxml" "cityhash" "lua" "googletest"
)

NEED_CLEAR=false
echo "NEED_CLEAR !!! $NEED_CLEAR "
while [[ $# -gt 0 ]]; do
	echo "option: $1"
	case "$1" in
    	-a|--all)
		for item in "${params_all[@]}"; do
  			params+=("$item")
		done
      	echo "option: ALLL"
      	shift
    	;;
    	-c|--clear)
		echo "option: clear"
		NEED_CLEAR=true
      	shift
    	;;
    	-r|--rabbitmq)
		echo "option: rabbitmq"
		params+=( "rabbitmq-c")
      	shift
    	;;
    	--amqpcpp)
		echo "option: amqpcpp"
		params+=( "amqpcpp")
      	shift
    	;;
    	--qpid-proton)
		echo "option: qpid-proton"
		params+=( "qpid-proton")
      	shift
    	;;
		-l|--log4cxx)
		echo "option: log4cxx"
		params+=( "log4cxx")
      	shift
    	;;
    	-b|--boost)
		echo "option: boost"
		params+=( "boost")
      	shift
    	;;
    	-z|--zstd)
		echo "option: zstd"
		params+=( "zstd")
      	shift
    	;;
    	--rapidjson)
		echo "option: rapidjson"
		params+=( "rapidjson")
      	shift
    	;;
    	--rapidxml)
		echo "option: rapidxml"
		params+=( "rapidxml")
      	shift
    	;;
    	--cityhash)
		echo "option: cityhash"
		params+=( "cityhash")
      	shift
    	;;
    	--lua)
		echo "option: lua"
		params+=( "lua")
      	shift
    	;;
    	--googletest)
		echo "option: googletest"
		params+=( "googletest")
      	shift
    	;;
	esac	
	# params["$1"]=true
	shift
done	

echo "Params : ${params[@]}"

function param_exists() {
	echo "Hello, $1!"
	for item in "${params[@]}"; do
		echo "Hello, item  "$item""
		if [ "$item" == "$1" ]; then
			echo "Hello, "$item" and $1! found !!!"
    		return 1
  		fi
	done
	echo "Hello, $1! not found !!!"
	return 0
}

# rabbitmq-c
#------------------
echo "rabbitmq-c ... "
cd ${ROOT_DIR}/engine/3rdparty/rabbitmq-c

if [[ "$NEED_CLEAR" = true ]]; then
  rm -r build
  echo "remove rabbitmq-c build folder "
fi

mkdir build 
cd build

param_exists "rabbitmq-c"
if [ $? -eq 1 ]; then
	echo "install rabbitmq-c "
	brew install openssl
	brew link --force openssl

	cmake -DCMAKE_INSTALL_PREFIX=${ROOT_DIR}/dependence/rabbitmq-c -DOPENSSL_ROOT_DIR=/usr/bin/openssl ..
	cmake --build . --target install
fi

# amqpcpp
#------------------
echo "make amqpcpp ... "
cd ${ROOT_DIR}/engine/3rdparty/amqpcpp

if [[ "$NEED_CLEAR" = true ]]; then
  rm -r build
  echo "remove amqpcpp build folder "
fi

mkdir build 
cd build

param_exists "amqpcpp"
if [ $? -eq 1 ]; then
	echo "install amqpcpp "
	cmake -DCMAKE_DEPENDENCE_PATH=${ROOT_DIR}/dependence ..
	cmake --build . --target install
fi

# qpid-proton
#------------------
cd ${ROOT_DIR}/engine/3rdparty/qpid-proton

if [[ "$NEED_CLEAR" = true ]]; then
  rm -r build
  echo "remove qpid-proton build folder "
fi

mkdir build 
cd build

param_exists "qpid-proton"
if [ $? -eq 1 ]; then
	brew install libuv

	cmake .. -DBUILD_EXAMPLES=0 -DBUILD_STATIC_LIBS=1 -DPROACTOR=libuv -DCMAKE_INSTALL_PREFIX=${ROOT_DIR}/dependence/qpid-proton
	cmake --build . --target install
fi

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
echo "make log4cpp ... "

cd ${ROOT_DIR}/engine/3rdparty/log4cpp


param_exists "log4cxx"
if [ $? -eq 1 ]; then
	make clean 
	brew install autoconf
	./configure --prefix= --enable-static CXXFLAGS=-std=c++20

	make DESTDIR=${ROOT_DIR}/dependence/log4cpp install
fi

# boost
#------------------
echo "make boost ... "

param_exists "boost"
if [ $? -eq 1 ]; then

	if [[ -z "$BOOST_DIR" ]]; then
  		echo "Not found BOOST_DIR variable. You need set it to ~/.bash_profile"
  	exit 0
	else
  		echo "BOOST_DIR has the value: $BOOST_DIR"
  		cd "$BOOST_DIR"
  		./bootstrap.sh
  		./b2 install --libdir="${ROOT_DIR}/dependence/boost/lib" --includedir="${ROOT_DIR}/dependence/boost/include"
  		cd ${CURRENT_DIR}
	fi
fi

# zstd
#------------------
echo "make zstd ... "

cd ${ROOT_DIR}/engine/3rdparty/zstd-1.5.2

param_exists "zstd"
if [ $? -eq 1 ]; then
	make clean
	# ./configure --prefix=

	make prefix=
	make prefix= DESTDIR=${ROOT_DIR}/dependence/zstd MOREFLAGS="-static" install
fi

# rapidjson
#------------------
echo "make rapidjson ... "
cd ${ROOT_DIR}/engine/3rdparty/rapidjson

if [[ "$NEED_CLEAR" = true ]]; then
  rm -r build
  echo "remove rapidjson build folder "
fi

mkdir build 
cd build

param_exists "rapidjson"
if [ $? -eq 1 ]; then
	cmake .. -DCMAKE_INSTALL_PREFIX=${ROOT_DIR}/dependence/rapidjson -DRAPIDJSON_BUILD_TESTS=OFF -DRAPIDJSON_BUILD_EXAMPLES=OFF
	cmake --build . --target install
fi

# rapidxml
#------------------

echo "make rapidxml ... "

param_exists "rapidxml"
if [ $? -eq 1 ]; then

	cd ${ROOT_DIR}/engine/3rdparty/rapidxml-1.13

	mkdir ${ROOT_DIR}/dependence/rapidxml
	mkdir ${ROOT_DIR}/dependence/rapidxml/include
	mkdir ${ROOT_DIR}/dependence/rapidxml/include/rapidxml

	cp * ${ROOT_DIR}/dependence/rapidxml/include/rapidxml

fi

# cityhash
#------------------
echo "make cityhash ... "

param_exists "cityhash"
if [ $? -eq 1 ]; then

	cd ${ROOT_DIR}/engine/3rdparty/cityhash
	./configure
	make all check CXXFLAGS="-g -O3" prefix=
	make prefix= DESTDIR=${ROOT_DIR}/dependence/cityhash install

fi

# lua
#------------------
echo "make lua ... "

param_exists "lua"
if [ $? -eq 1 ]; then

	cd ${ROOT_DIR}/engine/3rdparty/lua-5.4.4

	mkdir ${ROOT_DIR}/dependence/lua
	mkdir ${ROOT_DIR}/dependence/lua/include
	mkdir ${ROOT_DIR}/dependence/lua/include/Lua

	make all prefix=
	make prefix= INSTALL_TOP=${ROOT_DIR}/dependence/lua INSTALL_INC=${ROOT_DIR}/dependence/lua/include/Lua install

fi

# googletest
#------------------
echo "make googletest ... "

if [[ "$NEED_CLEAR" = true ]]; then
  rm -r build
  echo "remove googletest build folder "
fi

param_exists "googletest"
if [ $? -eq 1 ]; then

	cd ${ROOT_DIR}/engine/3rdparty/googletest

	mkdir build 
	cd build

	cmake .. -DCMAKE_INSTALL_PREFIX=${ROOT_DIR}/dependence/googletest
	cmake --build . --target install
fi

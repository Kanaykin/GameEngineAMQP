# GameEngineAMQP

This is game engine in which modules don't have hard binding. Modules communicate with each other by amgp(Advanced Message Queuing Protocol).

## Installation

### boost

go to https://www.boost.org/ and download last release

./bootstrap.sh --prefix=/Users/sergeykanaykin/Documents/Home/Engines/boost

./b2 install

open ~/.bash_profile

set BOOST_DIR variable

source ~/.bash_profile

cmake .. -GXcode
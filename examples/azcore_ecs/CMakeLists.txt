cmake_minimum_required(VERSION 3.10)

include(zstd)
include(rapidjson)
include(rapidxml)
include(cityhash)
include(lua)
include(googletest)
include(azcore)
include(example_app)

list(APPEND modules log4cpp)
cmake_language(CALL MAKE_EXAMPLE_APP azcore_ecs modules)

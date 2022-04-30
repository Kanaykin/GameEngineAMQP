cmake_minimum_required(VERSION 3.10)

include(example_app)

list(APPEND modules messages_system qpid-proton)
cmake_language(CALL MAKE_EXAMPLE_APP peer-to-peer-client modules)

cmake_minimum_required(VERSION 3.10)

include(example_app)

list(APPEND modules rabbitmq)
cmake_language(CALL MAKE_EXAMPLE_APP hello_world_rabbitmq modules)

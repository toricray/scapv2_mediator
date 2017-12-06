cmake_minimum_required(VERSION 3.0)
project(scapv2_mediator)
set(EXECUTABLE_OUTPUT_PATH "../bin")
add_definitions(-Wall -Wno-unknown-pragmas -Wno-unused-function -lrt)
set(CMAKE_CXX_STANDARD 11)

file(GLOB   LIBRARIES
        ./3d_party/lib/tinyxml2/libtinyxml2.a
        ./3d_party/lib/SimpleAmqpClient/libSimpleAmqpClientStatic.a
        ./3d_party/lib/librabbitmq/librabbitmq.a
        ./3d_party/lib/boost/libboost_thread.a
        ./3d_party/lib/boost/libboost_chrono.a
        ./3d_party/lib/boost/libboost_system.a
        ./3d_party/lib/boost/libboost_date_time.a
        )

include_directories(
        ./3d_party/include/SimpleAmqpClient/
        ./3d_party/include/librabbitmq/
        ./3d_party/include/json/
        ./3d_party/include/tinyxml2/
        ./src/
        ./src/Diameter/Interface
        )

file(GLOB   SETTINGS_SRC            ./src/Settings/*.*)
file(GLOB   REQUEST_HANDLER_SRC     ./src/RequestHandler/*.*)
file(GLOB   UTILLS_SRC              ./src/Utills/*.*)
file(GLOB   MQ_SRC                  ./src/MQAdapter/*.*)
file(GLOB   LOGGER_SRC              ./src/Logger/*.*)
file(GLOB   DIAMETER1_SRC           ./src/Diameter/Source_/*.*)
file(GLOB   DIAMETER2_SRC           ./src/Diameter/Source_/CCR/*.*)
file(GLOB   DIAMETER3_SRC           ./src/Diameter/Source_/Coder/*.*)
file(GLOB   DIAMETER4_SRC           ./src/Diameter/Source_/RawMessage/*.*)

set(SOURCE_FILES
        ./src/main.cpp
        ${LOGGER_SRC}
        ${SETTINGS_SRC}
        ${UTILLS_SRC}
        ${MQ_SRC}
        ${DIAMETER1_SRC}
        ${DIAMETER2_SRC}
        ${DIAMETER3_SRC}
        ${DIAMETER4_SRC})

add_executable(scapv2_mediator ${SOURCE_FILES})

target_link_libraries( scapv2_mediator  pthread   ${LIBRARIES} )
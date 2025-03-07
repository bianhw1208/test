cmake_minimum_required(VERSION 3.8)

# Make use of c++11
SET(CMAKE_CXX_STANDARD 11)

set(ProjectName sip-gateway)  
project(${ProjectName})

# 检查文件或目录是否存在
if(EXISTS "${CMAKE_SOURCE_DIR}/conf/config.ini")
    set(CONFIG_PATH "${CMAKE_SOURCE_DIR}/conf/config.ini")
endif()

# 设置库文件路径
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
# 设置可执行程序路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# 明确列出所有源文件
file(GLOB GB_SRC_LIST
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sipserver/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sipserver/event_handler/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sipserver/event_handler/sdp/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sipserver/manscdp/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sipserver/manscdp/defs/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sipserver/request_manager/*.cpp
  
  ${CMAKE_CURRENT_SOURCE_DIR}/src/mediaserver/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/ptz/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/sqldb/*.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/StreamResolver/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/StreamResolver/h26x/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/StreamResolver/ps/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/StreamResolver/rtp/*.cpp
)

# 添加头文件目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sipserver
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mediaserver
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils

    ${CMAKE_CURRENT_SOURCE_DIR}/src/StreamResolver
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StreamResolver/h26x
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StreamResolver/ps
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StreamResolver/rtp
)

# 添加库目录
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/linux
)

# 添加可执行文件
add_executable(${ProjectName} ${GB_SRC_LIST})

# 设置链接库
target_link_libraries(${ProjectName} 
    -Wl,--whole-archive
    pthread
    -Wl,--no-whole-archive
    json 
    eXosip2 
    osip2 
    osipparser2 
    uuid 
    ssl 
    crypto 
    resolv 
    #StreamResolver
)

add_custom_command(
  TARGET ${ProjectName} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CONFIG_PATH}
  "$<TARGET_FILE_DIR:${ProjectName}>/config.ini"
)

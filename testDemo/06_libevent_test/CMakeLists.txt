# cmake版本要求
cmake_minimum_required(VERSION 3.22)

# 工程名称
project(DEMO_LIBEVENT_TEST)

# 设置为debug可调试
set(CMAKE_BUILD_TYPE Debug)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)

# 可执行程序输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# 参与编译的文件
aux_source_directory(${CMAKE_SOURCE_DIR} SRC_FILES)

# 生成可执行程序
add_executable(${PROJECT_NAME} ${SRC_FILES})

# 链接依赖库
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../../thirdParty/libevent/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../../thirdParty/libevent/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE event)
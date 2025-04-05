# cmake版本要求
cmake_minimum_required(VERSION 3.22)

# 工程名称
project(sipServer)

# 设置编译器
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 关闭三方库警告
add_definitions(-w)

# 设置为debug可调试
set(CMAKE_BUILD_TYPE Debug)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)

# 可执行程序输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# 参与编译的文件
aux_source_directory(${CMAKE_SOURCE_DIR}/src SRC_FILES)

# 生成可执行程序
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# 链接glog依赖库
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/glog/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/glog/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE glog)

# 链接gflags依赖库
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/gflags/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/gflags/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE gflags)

# 链接boost依赖库
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/boost_1_71_0/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/boost_1_71_0/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE boost_system)
target_link_libraries(${PROJECT_NAME} PRIVATE boost_thread)

# 链接Simpleini依赖库
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/simpleini/include)

# 链接jsoncpp依赖库
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/jsoncpp/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/jsoncpp/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE jsoncpp)

# 链接tinyxml2依赖库
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/tinyxml2/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/tinyxml2/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE tinyxml2)

# 链接jrtplib依赖库
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/jthread/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/jthread/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE jthread)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/jrtplib/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/jrtplib/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE jrtp)

# 链接ZLToolkit依赖库
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/ZLToolKit/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/ZLToolKit/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE ZLToolKit)

# 链接pthread库
target_link_libraries(${PROJECT_NAME} PRIVATE pthread)

# 链接pjsip依赖库
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/pjproject-2.15.1/pjlib/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/pjproject-2.15.1/pjlib/lib)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/pjproject-2.15.1/pjlib-util/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/pjproject-2.15.1/pjlib-util/lib)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/pjproject-2.15.1/pjmedia/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/pjproject-2.15.1/pjmedia/lib)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/pjproject-2.15.1/pjnath/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/pjproject-2.15.1/pjnath/lib)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/pjproject-2.15.1/pjsip/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/pjproject-2.15.1/pjsip/lib)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/pjproject-2.15.1/pjsip-apps/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/pjproject-2.15.1/pjsip-apps/lib)

target_link_libraries(${PROJECT_NAME} PRIVATE pjsua2-x86_64-unknown-linux-gnu)
target_link_libraries(${PROJECT_NAME} PRIVATE pjsua-x86_64-unknown-linux-gnu)
target_link_libraries(${PROJECT_NAME} PRIVATE pjsip-ua-x86_64-unknown-linux-gnu)
target_link_libraries(${PROJECT_NAME} PRIVATE pjsip-simple-x86_64-unknown-linux-gnu)
target_link_libraries(${PROJECT_NAME} PRIVATE pjsip-x86_64-unknown-linux-gnu)
target_link_libraries(${PROJECT_NAME} PRIVATE pjmedia-codec-x86_64-unknown-linux-gnu)
target_link_libraries(${PROJECT_NAME} PRIVATE pjmedia-videodev-x86_64-unknown-linux-gnu)
target_link_libraries(${PROJECT_NAME} PRIVATE pjmedia-audiodev-x86_64-unknown-linux-gnu)
target_link_libraries(${PROJECT_NAME} PRIVATE pjmedia-x86_64-unknown-linux-gnu) 
target_link_libraries(${PROJECT_NAME} PRIVATE pjnath-x86_64-unknown-linux-gnu)
target_link_libraries(${PROJECT_NAME} PRIVATE pjlib-util-x86_64-unknown-linux-gnu)
target_link_libraries(${PROJECT_NAME} PRIVATE pj-x86_64-unknown-linux-gnu)
target_link_libraries(${PROJECT_NAME} PRIVATE srtp-x86_64-unknown-linux-gnu)
target_link_libraries(${PROJECT_NAME} PRIVATE resample-x86_64-unknown-linux-gnu)
target_link_libraries(${PROJECT_NAME} PRIVATE gsmcodec-x86_64-unknown-linux-gnu)
target_link_libraries(${PROJECT_NAME} PRIVATE speex-x86_64-unknown-linux-gnu)
target_link_libraries(${PROJECT_NAME} PRIVATE ilbccodec-x86_64-unknown-linux-gnu)
target_link_libraries(${PROJECT_NAME} PRIVATE g7221codec-x86_64-unknown-linux-gnu)
target_link_libraries(${PROJECT_NAME} PRIVATE yuv-x86_64-unknown-linux-gnu)
target_link_libraries(${PROJECT_NAME} PRIVATE webrtc-x86_64-unknown-linux-gnu)

# 链接bcg729库
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/bcg729/include)
# target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/bcg729/lib)
# target_link_libraries(${PROJECT_NAME} PRIVATE bcg729)

# 链接opus库
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/opus-1.3/include)
# target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/opus-1.3/lib)
# target_link_libraries(${PROJECT_NAME} PRIVATE opus)

# 链接openssl库
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/openssl-3.0.0/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/openssl-3.0.0/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE ssl crypto)

# 链接vpx库
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/vpx/include)
# target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/vpx/lib)
# target_link_libraries(${PROJECT_NAME} PRIVATE vpx)

# 链接openh264库
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/openh264/include)
# target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/openh264/lib)
# target_link_libraries(${PROJECT_NAME} PRIVATE openh264)

# 链接uuid库
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/uuid/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/uuid/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE uuid)

# 链接alsa库, 用于获取声卡设备
# 如果不存在: sudo apt install libasound2-dev
# target_link_libraries(${PROJECT_NAME} PRIVATE asound)

# 链接sdl2库
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/sdl2/include)
# target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/sdl2/lib)
# target_link_libraries(${PROJECT_NAME} PRIVATE SDL2)

# 链接v4l2库
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/v4l2/include)
# target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/v4l2/lib)
# target_link_libraries(${PROJECT_NAME} PRIVATE v4l2)

# 链接opencore-amr库
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/opencore-amr-0.1.6/include)
# target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/opencore-amr-0.1.6/lib)
# target_link_libraries(${PROJECT_NAME} PRIVATE opencore-amrnb opencore-amrwb)

# 链接vo-amrwbenc库
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/vo-amrwbenc-0.1.3/include)
# target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdParty/vo-amrwbenc-0.1.3/lib)
# target_link_libraries(${PROJECT_NAME} PRIVATE vo-amrwbenc)
#-DCMAKE_BUILD_TYPE=Debug
#-DCMAKE_BUILD_TYPE=Release
cmake_minimum_required(VERSION 2.8.11.2)

project(sirius)
add_definitions(-std=c99)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

#stndup warning
set (CMAKE_C_FLAGS "-D_GNU_SOURCE")
set(HEADER_FILES ../inih/ini.h ../pcrs/pcrs.h ../ask_question.h ../post_file.h ../file2memory.h ../globaldefs.h)
add_executable(sirius ../sirius.c ../inih/ini.c ../pcrs/pcrs.c ../ask_question.c ../post_file.c ../file2memory.c ${HEADER_FILES})

target_link_libraries (sirius pcre curl)
install(FILES .sirius.ini DESTINATION ~ COMPONENT config)
install(TARGETS sirius DESTINATION bin COMPONENT binaries)
#INSTALL_TARGETS(/bin sirius)

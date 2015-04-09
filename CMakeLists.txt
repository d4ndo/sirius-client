#-DCMAKE_BUILD_TYPE=Debug
#-DCMAKE_BUILD_TYPE=Release
cmake_minimum_required(VERSION 2.8.11.2)

project(sirius)
add_definitions(-std=c99)

set (CMAKE_C_FLAGS "-D_GNU_SOURCE")
#header(parseSVG.h globaldef.h)
add_executable(sirius ../sirius.c ../pcrs.c ../ask_question.c ../speech_recognition.c ../image_matching.c ../globaldefs.h)

target_link_libraries (sirius pcre curl)
#target_link_libraries (customQR pugixml)
INSTALL_TARGETS(/bin sirius)

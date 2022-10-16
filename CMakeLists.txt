
cmake_minimum_required(VERSION 3.8)     # 版本号
project(MedicalDemo)                     # 项目名

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 设置编译目录

 # 设置 静态链接库的输出目录
set(publish_lib_debug 			${CMAKE_CURRENT_SOURCE_DIR}/Lib/Debug)
set(publish_lib_release 		${CMAKE_CURRENT_SOURCE_DIR}/Lib/Release)
 
# 设置 可执行程序 和 动态链接库 的输出目录
set(publish_bin_debug 			${CMAKE_CURRENT_SOURCE_DIR}/Bin)
set(publish_bin_release 		${CMAKE_CURRENT_SOURCE_DIR}/Bin)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${publish_lib_debug})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${publish_lib_release})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${publish_bin_debug})   
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${publish_bin_release}) 

# 将当前目录命名为 PROJECT_DIR
set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# 添加所有子目录到 allSubDir
list(APPEND allSubDir "VRT")

# 遍历生成
foreach(dirname ${allSubDir})
    add_subdirectory(${dirname})
endforeach()

# 设置头文件的包含目录
include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# 将第三方库中的dll文件拷贝到可执行文件的生成目录
file(GLOB dll_src "${CMAKE_CURRENT_SOURCE_DIR}/3rd/*.dll" )
file(COPY ${dll_src} DESTINATION ${PROJECT_DIR}/Bin)


# 最终生成的项目文件. 会有 ALL_BUILD 和 ZERO_CHECK. 将它们放置在 Other 文件夹下
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "Other")
project(VRT)    

file(GLOB_RECURSE HEADER_FILES *.h *.hpp *.ini) 

file(GLOB_RECURSE SOURCE_FILES *.cpp *.c)

set(AllFile ${HEADER_FILES} ${SOURCE_FILES})

source_group("Header Files" FILES ${HEADER_FILES})

link_directories(${PROJECT_DIR}/3rd)



add_executable(VRT ${AllFile})

# 头文件目录
target_include_directories(VRT PUBLIC 
    ${PROJECT_DIR}
)

set_target_properties(VRT PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")

option(USE_SOLUTION_FOLDERS  ON)
set_target_properties(VRT PROPERTIES FOLDER "VRT")


cmake_minimum_required(VERSION 3.20)

project(LLVMOptimizationToolkit LANGUAGES C CXX)

find_package(LLVM 18 REQUIRED CONFIG)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_program(
    LLVM_OPT_EXECUTABLE
    NAMES opt-18 opt
    HINTS "${LLVM_TOOLS_BINARY_DIR}"
    REQUIRED
)

message(STATUS "Using LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM CMake directory: ${LLVM_DIR}")

add_library(LLVMOptimizationToolkit MODULE
    src/ConstantFoldingPass.cpp
    src/DeadCodeEliminationPass.cpp
    src/LocalCSEPass.cpp
    src/Plugin.cpp
)

target_include_directories(LLVMOptimizationToolkit
    SYSTEM
    PRIVATE
        ${LLVM_INCLUDE_DIRS}
)
target_include_directories(LLVMOptimizationToolkit
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_features(LLVMOptimizationToolkit PRIVATE cxx_std_17)
add_definitions(${LLVM_DEFINITIONS})
set_target_properties(LLVMOptimizationToolkit PROPERTIES PREFIX "")

if(TARGET LLVM)
    target_link_libraries(LLVMOptimizationToolkit PRIVATE LLVM)
else()
    llvm_map_components_to_libnames(LLVM_LIBRARIES
        Analysis
        Core
        Passes
        Support
        TransformUtils
    )
    target_link_libraries(LLVMOptimizationToolkit PRIVATE ${LLVM_LIBRARIES})
endif()

enable_testing()
add_test(
    NAME plugin-regressions
    COMMAND
        "${Python3_EXECUTABLE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/run_tests.py"
        --opt "${LLVM_OPT_EXECUTABLE}"
        --plugin "$<TARGET_FILE:LLVMOptimizationToolkit>"
        --tests-dir "${CMAKE_CURRENT_SOURCE_DIR}/tests"
)

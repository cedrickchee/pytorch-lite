cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(custom_ops)

find_package(Torch REQUIRED)

add_executable(resnet18-app resnet18-app.cpp)
target_link_libraries(resnet18-app "${TORCH_LIBRARIES}")
set_property(TARGET resnet18-app PROPERTY CXX_STANDARD 11)
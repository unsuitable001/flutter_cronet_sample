cmake_minimum_required(VERSION 3.10)  # for example
project(flutter_cronet)
add_library(wrapper

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             "../wrapper/wrapper.cc"
             "../wrapper/wrapper.h"
             "../wrapper/sample_executor.cc"
             "../wrapper/sample_executor.h"
             "/usr/lib/dart/include/dart_api_dl.c"
)

include_directories(/usr/lib/dart/include/)

add_compile_options(-fPIC -ldl -rdynamic -DDART_SHARED_LIB -fpermissive)
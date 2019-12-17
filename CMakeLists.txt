cmake_minimum_required(VERSION 3.15)
project(functionCloner)

enable_testing()

set (CMAKE_CXX_STANDARD 17)

set (ORKA_ROSE_INST_DIR /home/florian/A_rose/rose_install/rose-git)
set (CMAKE_INSTALL_PREFIX /opt/orka)

# This is needed for Cmake to include the -pthread flag in GCC
# g++ needs -pthread in order to set some preprocessor flags (-D_REENTRANT)
# which is needed for things to work out
set (CMAKE_THREAD_PREFER_PTHREAD TRUE)
set (THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package (Threads REQUIRED)
# find_package (Boost REQUIRED)

#pkg_check_modules(REQUIRED tinyxml2)

set (PROJECT_DYN_LIBS librose.so
                      libquadmath.so
                      libboost_system.so
                      libboost_filesystem.so
                      libtinyxml2.so)

# absolute paths for the time being
link_directories(
        /usr/lib/jvm/java-8-openjdk/jre/lib/amd64/server/
        /usr/lib/
        ${ORKA_ROSE_INST_DIR}/lib
        /opt/boost/1.67.0/lib
)

include_directories(
        ${ORKA_ROSE_INST_DIR}/include/rose
        ./procxx/include
        ./json/single_include
        ./
        /opt/boost/1.67.0/include
)

add_executable(failing_clone failing_clone.cpp)
target_link_libraries(failing_clone Threads::Threads
                                    ${PROJECT_DYN_LIBS}
                                    ${PROJECT_STAT_LIBS})

add_executable(succeeding_clone succeeding_clone.cpp)
target_link_libraries(succeeding_clone Threads::Threads
                                    ${PROJECT_DYN_LIBS}
                                    ${PROJECT_STAT_LIBS})


cmake_minimum_required(VERSION 3.23)
project(cryptography)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE HEADERS "*.h")
add_executable(diffie-hellman diffie-hellman.cpp ${HEADERS})
add_executable(shamir shamir.cpp ${HEADERS})
add_executable(elgamal elgamal.cpp ${HEADERS})
add_executable(rsa rsa.cpp ${HEADERS})
add_executable(mental-poker mental-poker.cpp ${HEADERS})
add_executable(dig-sig-rsa dig-sig-rsa.cpp ${HEADERS})
add_executable(dig-sig-elgamal dig-sig-elgamal.cpp ${HEADERS})
add_executable(coin-flip coin-flip.cpp ${HEADERS})
add_executable(digital-cash digital-cash.cpp ${HEADERS})
add_executable(baby-step-giant-step baby-step-giant-step.cpp ${HEADERS})
add_executable(one-time-pad one-time-pad.cpp ${HEADERS})

add_executable(test test.cpp ${HEADERS})

add_executable(prod_build prod_build.cpp)

cmake_minimum_required(VERSION 3.8)
project(Rho)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/ast/Ast.cpp
        src/ast/Ast.h
        src/ast/Atom.h
        src/ast/AtomTable.cpp
        src/ast/AtomTable.h
        src/parser/Parser.cpp
        src/parser/Parser.h
        src/parser/RhoContext.h
        src/parser/Scope.cpp
        src/parser/Scope.h
        src/scanner/Scanner.cpp
        src/scanner/Scanner.h
        src/token/Location.cpp
        src/token/Location.h
        src/token/Token.cpp
        src/token/Token.h
        src/type/Entity.cpp
        src/type/Entity.h
        src/type/Type.cpp
        src/type/Type.h
        src/utils/File.cpp
        src/utils/File.h
        src/utils/string.cpp
        src/utils/string.h
        src/Rho.cpp
        src/Rho.h
        main.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wall -I./src")
#message("${CMAKE_CXX_FLAGS}")
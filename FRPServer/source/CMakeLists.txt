include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(FRPServer 
"lib/netlib/netlib.cpp" 
"lib/baselib/baselib.cpp" 
"src/main.cpp" 
"src/app/app.cpp" 
"src/client/client.cpp" 
"src/msg/msg.cpp" 
"src/session/session.cpp")
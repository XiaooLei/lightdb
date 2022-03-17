cmake_minimum_required(VERSION 2.8.12)
project(MyProject)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-std=c++11 -pthread ")
add_compile_options(-std=c++11)
add_compile_options(-lpthread)
add_compile_options(-Wl,--no-as-needed)
add_compile_options(-g)
add_compile_options(-fexceptions)
add_compile_options(-Wall)
add_compile_options(-fpermissive)
add_executable(MyProject  lightdb.cpp db_hash.cpp db_list.cpp db_set.cpp db_str.cpp db_zset.cpp storage/db_file.cpp
        storage/db_file.h ds/hash/ds_hash.h ds/list/ds_list.h ds/set/ds_set.h ds/str/ds_strlist.h ds/zset/ds_zset.h
        include/common.h include/status.h include/types.h storage/db_file.h storage/entry.h idx.cpp util/sync/CTimer.h util/sync/CTimer.cpp  util/sync/waitGroup.h dump.cpp server/epoll.h server/epoll.cpp server/server.h server/server.cpp server/main.cpp
        server/cmd/cmd_hash.cpp server/cmd/cmd_list.cpp server/cmd/cmd_set.cpp server/cmd/cmd_str.cpp server/cmd/cmd_zset.cpp server/cmd/cmd.h server/cmd/cmd.cpp server/cmd/requestHandler.h server/cmd/LightdbRequestHandler.cpp server/cmd/LightdbRequestHandler.h
        )

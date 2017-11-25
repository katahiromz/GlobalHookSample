add_executable(hooker WIN32 hooker.cpp hooker_res.rc)
target_link_libraries(hooker comctl32 payload)

add_library(payload SHARED payload.cpp)
target_compile_definitions(payload PRIVATE PAYLOAD_BUILD)

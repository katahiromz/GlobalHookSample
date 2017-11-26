add_library(payload SHARED payload.cpp)
target_compile_definitions(payload PRIVATE PAYLOAD_BUILD)
set_target_properties(payload PROPERTIES PREFIX "")

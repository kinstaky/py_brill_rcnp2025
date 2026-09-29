nanobind_add_module(_bindings STABLE_ABI NB_STATIC LTO bindings.cpp)
target_link_libraries(_bindings PUBLIC t0_match_viewer)
set_target_properties(_bindings PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

install(TARGETS _bindings LIBRARY DESTINATION ${SKBUILD_PROJECT_NAME})

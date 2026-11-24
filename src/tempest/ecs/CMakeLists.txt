set(TEMPEST_ECS_LIB TempestECS${TEMPEST_INTERNAL_TARGET_SUFFIX})
add_library(
        ${TEMPEST_ECS_LIB} INTERFACE
        Entity.h
        components/Component.h
        components/Transform.h
        EntityManager.h
        ComponentArray.h
        ComponentManager.h
        Coordinator.h
        System.h
        SystemManager.h
)


add_library(Tempest::Internal::ECS ALIAS ${TEMPEST_ECS_LIB})

target_compile_features(${TEMPEST_ECS_LIB} INTERFACE cxx_std_23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_include_directories(${TEMPEST_ECS_LIB} INTERFACE "${TEMPEST_ROOT_DIR}")
target_link_libraries(
        ${TEMPEST_ECS_LIB}
        INTERFACE
        Falcon::Falcon
)
target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        locks.cppm
        create_lock_group.ixx
        CriticalSection.ixx
        CriticalSectionType.ixx
        DependencyStack.ixx
        ExclusiveCriticalSection.ixx
        has_lock_group_c.ixx
        Lock.ixx
        LockedResourceID.ixx
        LockGroup.ixx
        LockOwnerIndex.ixx
        SharedCriticalSection.ixx
        TopOfLockStack.ixx
)

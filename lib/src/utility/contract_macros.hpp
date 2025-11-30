#pragma once

#include <source_location>

#define PRECOND(condition, ...)                                                           \
    ddge::util::assert_precondition(                                                      \
        condition, #condition, std::source_location::current() __VA_OPT__(, ) __VA_ARGS__ \
    )

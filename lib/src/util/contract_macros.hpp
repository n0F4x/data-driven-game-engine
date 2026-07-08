#pragma once

#ifdef ENGINE_DEBUG
  #include <source_location>

  #define PRECOND_AS(condition, Exception_T, ...)                                \
      ((void)((!!(condition))                                                    \
              || (::ddge::util::propagate_precondition_violation<Exception_T>(   \
                      #condition,                                                \
                      std::source_location::current() __VA_OPT__(, ) __VA_ARGS__ \
                  ),                                                             \
                  false)))

  #define PRECOND(condition, ...)                                        \
      PRECOND_AS(                                                        \
          condition,                                                     \
          ::ddge::util::PreconditionViolation __VA_OPT__(, ) __VA_ARGS__ \
      )
#else
  #define PRECOND_AS(condition, Exception_T, ...) ((void)0)   // NOLINT(*-macro-usage)
  #define PRECOND(condition, ...)                 ((void)0)   // NOLINT(*-macro-usage)
#endif

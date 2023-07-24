# Project guidelines

## Compilation

The code must compile with the latest gcc version.
Please use modern features if possible.

## Style Guide

### Clang-format

Use the provided .clang-format.

### Naming conventions

The following naming conventions apply:
- Namespace: camelCase
- Macro: SCREAMING_SNAKE_CASE
- Class: PascalCase
- Struct: PascalCase
- Enum: PascalCase
- Enumerator: camelCase
- Typedef: PascalCase
- Union: PascalCase
- Class Member Function: snake_case
- Struct Member Function: snake_case
- Class Member Field: snake_case
- Struct Member Field: snake_case
- Global Function: snake_case
- Global Variable: snake_case
- Parameter: snake_case
- Local Variable: snake_case

Please use the following Hungarian notations:
- Enumerator: eEnumerator
- Class/Struct Private Member Field: m_field
- Class/Struct Static Variable: s_variable
- Global Variable: g_variable
- Parameter: t_parameter

### Exceptions

This is an exception-free codebase. Mark functions as `noexcept`.
In some constructors exceptions are unavoidable, mark them `noexcept(false)`!

Terminating the program in case of an unsuccessful dynamic memory allocation is fine.
(This is what noexcept functions do by default instead of throwing.)

### Other

- Don't make class fields protected. Make a protected getter/setter instead.
- Make a static helper function called `create` in case a normal constructor may fail.
And mark the constructor private!
- Use the Builder Pattern instead of complex constructors.
The builder should be constructed by the class's `create` function, 
which takes no arguments in this case.


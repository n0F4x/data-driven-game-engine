# Project guidelines

## Compilation

The code must compile with the latest Clang version.
Please use modern features if possible.

## Style Guide

### Clang-format

Use the provided .clang-format.

### Naming conventions

The following naming conventions apply:

- Namespace: snake_case
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

### Comments

Make your code self-documenting!  
Mark each section as seen in the rest of the files.

### Class Scope Ordering

Declare everything that is public first!  
The order within public/protected/private parts is the following:

1. Type Aliases
2. Nested Classes
3. Friend Classes
4. Static Variables
5. Variables
6. Static Methods
7. Constructors / Destructors
8. Operators
9. Methods

Declare friend classes as private!

### Other

- Don't make class fields protected. Make a protected getter/setter instead.

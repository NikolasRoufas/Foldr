# Foldr Programming Language

```
  ███████╗ ██████╗ ██╗     ██████╗ ██████╗ 
  ██╔════╝██╔═══██╗██║     ██╔══██╗██╔══██╗
  █████╗  ██║   ██║██║     ██║  ██║██████╔╝
  ██╔══╝  ██║   ██║██║     ██║  ██║██╔══██╗
  ██║     ╚██████╔╝███████╗██████╔╝██║  ██║
  ╚═╝      ╚═════╝ ╚══════╝╚═════╝ ╚═╝  ╚═╝
```

**Version 1.0.0**

A modern, statically-typed programming language with C++-inspired syntax, designed for clarity, performance, and ease of use.

---

## Table of Contents

- [Introduction](#introduction)
- [Installation](#installation)
- [Getting Started](#getting-started)
- [Language Syntax](#language-syntax)
  - [Comments](#comments)
  - [Variables](#variables)
  - [Data Types](#data-types)
  - [Operators](#operators)
  - [Functions](#functions)
  - [Control Flow](#control-flow)
  - [Arrays](#arrays)
- [Built-in Functions](#built-in-functions)
- [Standard Library](#standard-library)
- [Example Programs](#example-programs)
- [CLI Reference](#cli-reference)
- [Language Specification](#language-specification)
- [Contributing](#contributing)
- [License](#license)

---

## Introduction

Foldr is a programming language that combines the power and familiarity of C++ syntax with modern language design principles. It features:

- **Static typing** with type inference
- **C++-inspired syntax** for familiarity
- **Clean, readable code** structure
- **Fast interpretation** with tree-walk evaluation
- **Built-in array support**
- **First-class functions**

### File Extensions

- **`.fld`** - Primary source file extension
- **`.flr`** - Alternative runtime file extension

---

## Installation

### Prerequisites

- GCC compiler (or any C compiler)
- Make (optional)

### Build from Source

```bash
# Clone the repository
git clone https://github.com/yourusername/foldr.git
cd foldr

# Compile the interpreter
gcc -o foldr foldr.c -lm

# Move to system PATH (optional)
sudo mv foldr /usr/local/bin/

# Verify installation
foldr --version
```

### Windows

```bash
# Using MinGW or similar
gcc -o foldr.exe foldr.c -lm
```

---

## Getting Started

### Your First Program

Create a file named `hello.fld`:

```foldr
func main() -> int {
    print("Hello, World!");
    return 0;
}

main();
```

Run it:

```bash
foldr hello.fld
```

Output:
```
Hello, World!
```

### Interactive Mode

Simply run `foldr` without arguments to see the ASCII logo and version:

```bash
$ foldr
```

---

## Language Syntax

### Comments

Single-line comments start with `#`:

```foldr
# This is a comment
let x: int = 10;  # Inline comment
```

### Variables

#### Declaration

Variables are declared using `let` or `const` keywords with type annotations:

```foldr
let name: string = "Alice";
let age: int = 25;
let price: float = 19.99;
let active: bool = true;
const PI: float = 3.14159;
```

#### Type Inference

Types can be inferred from the initialization value:

```foldr
let count = 10;        # Inferred as int
let message = "Hello"; # Inferred as string
```

#### Constants

Use `const` for immutable values:

```foldr
const MAX_SIZE: int = 100;
const APP_NAME: string = "Foldr";
```

### Data Types

#### Primitive Types

| Type | Description | Example |
|------|-------------|---------|
| `int` | Integer numbers | `42`, `-17`, `0` |
| `float` | Floating-point numbers | `3.14`, `-0.5`, `2.0` |
| `string` | Text strings | `"Hello"`, `"World"` |
| `bool` | Boolean values | `true`, `false` |
| `void` | No value (function return) | N/A |

#### Complex Types

| Type | Description | Example |
|------|-------------|---------|
| `array` | Dynamic arrays | `[1, 2, 3]` |

#### Type Examples

```foldr
let integer: int = 42;
let decimal: float = 3.14;
let text: string = "Foldr";
let flag: bool = true;
let numbers: array = [1, 2, 3, 4, 5];
```

### Operators

#### Arithmetic Operators

```foldr
let a: int = 10 + 5;   # Addition: 15
let b: int = 10 - 5;   # Subtraction: 5
let c: int = 10 * 5;   # Multiplication: 50
let d: int = 10 / 5;   # Division: 2
let e: int = 10 % 3;   # Modulo: 1
```

#### Comparison Operators

```foldr
let eq: bool = 10 == 10;  # Equal: true
let ne: bool = 10 != 5;   # Not equal: true
let lt: bool = 5 < 10;    # Less than: true
let gt: bool = 10 > 5;    # Greater than: true
let le: bool = 5 <= 5;    # Less or equal: true
let ge: bool = 10 >= 10;  # Greater or equal: true
```

#### Logical Operators

```foldr
let and: bool = true && false;  # Logical AND: false
let or: bool = true || false;   # Logical OR: true
let not: bool = !true;          # Logical NOT: false
```

#### Assignment Operators

```foldr
let x: int = 10;
x = 20;      # Assignment
x += 5;      # Add and assign: x = x + 5
x -= 3;      # Subtract and assign: x = x - 3
```

#### Operator Precedence

From highest to lowest:

1. Parentheses `()`
2. Unary operators `!`, `-`
3. Multiplicative `*`, `/`, `%`
4. Additive `+`, `-`
5. Comparison `<`, `>`, `<=`, `>=`
6. Equality `==`, `!=`
7. Logical AND `&&`
8. Logical OR `||`
9. Assignment `=`, `+=`, `-=`

### Functions

#### Function Declaration

```foldr
func functionName(param1: type1, param2: type2) -> returnType {
    # Function body
    return value;
}
```

#### Example Functions

```foldr
# Function with no parameters
func greet() -> void {
    print("Hello!");
}

# Function with parameters
func add(a: int, b: int) -> int {
    return a + b;
}

# Function with string parameter
func greetUser(name: string) -> void {
    print("Hello, " + name + "!");
}
```

#### Calling Functions

```foldr
greet();                    # Call with no arguments
let sum: int = add(5, 3);   # Call with arguments
greetUser("Alice");         # Call with string argument
```

#### Main Function

Programs typically have a `main()` function as the entry point:

```foldr
func main() -> int {
    print("Program started");
    return 0;
}

main();  # Execute main function
```

### Control Flow

#### If-Else Statements

```foldr
if (condition) {
    # Code if condition is true
} else {
    # Code if condition is false
}
```

Example:

```foldr
let age: int = 18;

if (age >= 18) {
    print("You are an adult");
} else {
    print("You are a minor");
}
```

#### Nested If-Else

```foldr
let score: int = 85;

if (score >= 90) {
    print("Grade: A");
} else {
    if (score >= 80) {
        print("Grade: B");
    } else {
        if (score >= 70) {
            print("Grade: C");
        } else {
            print("Grade: F");
        }
    }
}
```

#### For Loops

Iterate over arrays or ranges:

```foldr
for (variable in iterable) {
    # Loop body
}
```

Example:

```foldr
let numbers: array = [1, 2, 3, 4, 5];

for (num in numbers) {
    print(str(num));
}
```

#### While Loops

```foldr
while (condition) {
    # Loop body
}
```

Example:

```foldr
let count: int = 0;

while (count < 5) {
    print("Count: " + str(count));
    count += 1;
}
```

### Arrays

#### Array Declaration

```foldr
let numbers: array = [1, 2, 3, 4, 5];
let names: array = ["Alice", "Bob", "Charlie"];
let mixed: array = [1, "two", 3];
```

#### Array Access

```foldr
let first: int = numbers[0];    # Get first element
let second: int = numbers[1];   # Get second element
```

#### Array Iteration

```foldr
let fruits: array = ["apple", "banana", "cherry"];

for (fruit in fruits) {
    print(fruit);
}
```

#### Array Length

```foldr
let items: array = [10, 20, 30];
let size: int = len(items);  # Returns 3
```

---

## Built-in Functions

Foldr provides several built-in functions for common operations:

### `print(...)`

Output values to console.

```foldr
print("Hello, World!");
print("Age: " + str(25));
```

**Parameters:** Variable number of arguments  
**Returns:** `void`

### `str(value)`

Convert a value to string.

```foldr
let num: int = 42;
let text: string = str(num);  # "42"
```

**Parameters:** Any value  
**Returns:** `string`

### `int(value)`

Convert a value to integer.

```foldr
let text: string = "123";
let num: int = int(text);  # 123
```

**Parameters:** `string` or `float`  
**Returns:** `int`

### `len(array)`

Get the length of an array.

```foldr
let items: array = [1, 2, 3, 4];
let count: int = len(items);  # 4
```

**Parameters:** `array`  
**Returns:** `int`

---

## Standard Library

### String Operations

```foldr
# Concatenation
let fullName: string = "John" + " " + "Doe";

# String with numbers
let message: string = "Count: " + str(10);
```

### Mathematical Operations

```foldr
# Basic arithmetic
let sum: int = 10 + 5;
let product: int = 10 * 5;
let quotient: int = 10 / 5;
let remainder: int = 10 % 3;

# Order of operations
let result: int = (10 + 5) * 2;  # 30
```

---

## Example Programs

### Hello World

```foldr
func main() -> int {
    print("Hello, World!");
    return 0;
}

main();
```

### Sum of Array

```foldr
func main() -> int {
    let numbers: array = [1, 2, 3, 4, 5];
    let sum: int = 0;
    
    for (num in numbers) {
        sum += num;
    }
    
    print("Sum: " + str(sum));
    return 0;
}

main();
```

### Fibonacci Sequence

```foldr
func fibonacci(n: int) -> int {
    if (n <= 1) {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

func main() -> int {
    let n: int = 10;
    print("Fibonacci(" + str(n) + ") = " + str(fibonacci(n)));
    return 0;
}

main();
```

### Factorial

```foldr
func factorial(n: int) -> int {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

func main() -> int {
    let num: int = 5;
    print("Factorial of " + str(num) + " is " + str(factorial(num)));
    return 0;
}

main();
```

### Grade Calculator

```foldr
func getGrade(score: int) -> string {
    if (score >= 90) {
        return "A";
    } else {
        if (score >= 80) {
            return "B";
        } else {
            if (score >= 70) {
                return "C";
            } else {
                if (score >= 60) {
                    return "D";
                } else {
                    return "F";
                }
            }
        }
    }
}

func main() -> int {
    let scores: array = [95, 87, 72, 64, 55];
    
    for (score in scores) {
        let grade: string = getGrade(score);
        print("Score: " + str(score) + " = Grade: " + grade);
    }
    
    return 0;
}

main();
```

### Even/Odd Checker

```foldr
func isEven(n: int) -> bool {
    return (n % 2) == 0;
}

func main() -> int {
    let numbers: array = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
    
    for (num in numbers) {
        if (isEven(num)) {
            print(str(num) + " is even");
        } else {
            print(str(num) + " is odd");
        }
    }
    
    return 0;
}

main();
```

### Array Maximum

```foldr
func findMax(arr: array) -> int {
    let max: int = arr[0];
    
    for (num in arr) {
        if (num > max) {
            max = num;
        }
    }
    
    return max;
}

func main() -> int {
    let numbers: array = [45, 23, 78, 12, 90, 34];
    let maximum: int = findMax(numbers);
    
    print("Maximum value: " + str(maximum));
    return 0;
}

main();
```

---

## CLI Reference

### Commands

#### Show Logo and Version

```bash
foldr
```

Displays the ASCII logo and version information.

#### Run a Program

```bash
foldr <filename.fld>
```

Execute a Foldr source file.

Example:
```bash
foldr hello.fld
```

#### Show Help

```bash
foldr --help
foldr -h
```

Display usage information and available commands.

#### Show Version

```bash
foldr --version
foldr -v
```

Display the current version of Foldr.

### Usage Examples

```bash
# Run a program
$ foldr program.fld

# Check version
$ foldr --version
Foldr v1.0.0

# Get help
$ foldr --help
```

---

## Language Specification

### Lexical Structure

#### Keywords

Reserved words in Foldr:

```
func    let     const   if      else    for     while   return
in      int     float   string  bool    array   void    true    false
```

#### Identifiers

- Must start with a letter or underscore
- Can contain letters, numbers, and underscores
- Case-sensitive

Valid identifiers:
```
myVariable
_privateVar
userName123
MAX_SIZE
```

#### Literals

**Integer Literals:**
```foldr
42
-17
0
```

**Float Literals:**
```foldr
3.14
-0.5
2.0
```

**String Literals:**
```foldr
"Hello, World!"
"Foldr is awesome"
""
```

**Boolean Literals:**
```foldr
true
false
```

### Grammar

#### Program Structure

```
Program     → Statement*
Statement   → FuncDecl | VarDecl | IfStmt | ForStmt | WhileStmt | ReturnStmt | ExprStmt
```

#### Declarations

```
FuncDecl    → "func" IDENTIFIER "(" Parameters? ")" "->" Type Block
VarDecl     → ("let" | "const") IDENTIFIER ":" Type "=" Expression ";"
Parameters  → IDENTIFIER ":" Type ("," IDENTIFIER ":" Type)*
```

#### Statements

```
IfStmt      → "if" "(" Expression ")" Block ("else" Block)?
ForStmt     → "for" "(" IDENTIFIER "in" Expression ")" Block
WhileStmt   → "while" "(" Expression ")" Block
ReturnStmt  → "return" Expression ";"
ExprStmt    → Expression ";"
Block       → "{" Statement* "}"
```

#### Expressions

```
Expression  → Assignment
Assignment  → IDENTIFIER ("=" | "+=" | "-=") Expression | LogicalOr
LogicalOr   → LogicalAnd ("||" LogicalAnd)*
LogicalAnd  → Equality ("&&" Equality)*
Equality    → Comparison (("==" | "!=") Comparison)*
Comparison  → Addition (("<" | ">" | "<=" | ">=") Addition)*
Addition    → Multiplication (("+" | "-") Multiplication)*
Multiplication → Unary (("*" | "/" | "%") Unary)*
Unary       → ("!" | "-") Unary | Call
Call        → Primary "(" Arguments? ")"
Primary     → NUMBER | STRING | "true" | "false" | IDENTIFIER | Array | "(" Expression ")"
Array       → "[" (Expression ("," Expression)*)? "]"
Arguments   → Expression ("," Expression)*
```

### Type System

Foldr is statically typed with the following type hierarchy:

```
Type
├── Primitive
│   ├── int
│   ├── float
│   ├── string
│   ├── bool
│   └── void
└── Complex
    └── array
```

#### Type Compatibility

- `int` and `float` can be used in arithmetic operations
- `string` can be concatenated with any type using `+`
- `bool` can be used in logical operations

---

## Compiler Architecture

### Components

1. **Lexer (Tokenizer)**: Converts source code into tokens
2. **Parser**: Builds Abstract Syntax Tree (AST) from tokens
3. **Interpreter**: Executes the AST

### Compilation Pipeline

```
Source Code (.fld)
    ↓
Lexical Analysis (Tokenizer)
    ↓
Tokens
    ↓
Syntax Analysis (Parser)
    ↓
Abstract Syntax Tree
    ↓
Semantic Analysis
    ↓
Execution (Interpreter)
    ↓
Output
```

### Implementation Details

- **Language**: C
- **Parsing**: Recursive Descent Parser
- **Execution**: Tree-Walk Interpreter
- **Memory**: Dynamic allocation with malloc

---

## Best Practices

### Code Style

#### Naming Conventions

```foldr
# Variables and functions: camelCase
let userName: string = "Alice";
func calculateSum() -> int { }

# Constants: UPPER_SNAKE_CASE
const MAX_VALUE: int = 100;

# Types: lowercase
let count: int = 0;
```

#### Indentation

Use 4 spaces for indentation:

```foldr
func main() -> int {
    if (true) {
        print("Indented");
    }
    return 0;
}
```

#### Comments

Add comments for complex logic:

```foldr
# Calculate the factorial of n recursively
func factorial(n: int) -> int {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}
```

### Performance Tips

1. **Avoid excessive recursion** - Use loops when possible
2. **Minimize string concatenation in loops** - Build strings efficiently
3. **Use appropriate data types** - Choose `int` over `float` when decimals aren't needed
4. **Reuse variables** - Avoid unnecessary allocations

### Error Handling

Always validate inputs:

```foldr
func divide(a: int, b: int) -> int {
    if (b == 0) {
        print("Error: Division by zero");
        return 0;
    }
    return a / b;
}
```

---

## Troubleshooting

### Common Errors

#### Syntax Error

```
Error: Unexpected token in expression
```

**Solution:** Check for missing semicolons, parentheses, or braces.

#### Undefined Variable

```
Error: Variable 'x' not found
```

**Solution:** Ensure variables are declared before use.

#### Type Mismatch

```
Error: Cannot perform operation on incompatible types
```

**Solution:** Ensure operands are compatible types.

### Debugging Tips

1. Add `print()` statements to trace execution
2. Check variable values at key points
3. Verify function return values
4. Ensure loops terminate correctly

---

## Contributing

We welcome contributions to Foldr! Here's how you can help:

### Reporting Bugs

1. Check existing issues on GitHub
2. Create a new issue with:
   - Clear description
   - Steps to reproduce
   - Expected vs actual behavior
   - Code sample

### Submitting Pull Requests

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

### Code Guidelines

- Follow existing code style
- Add comments for complex logic
- Update documentation as needed
- Ensure all tests pass

---

## Roadmap

### Version 1.1 (Planned)

- [ ] Multi-line comments
- [ ] String escape sequences
- [ ] Break and continue statements
- [ ] Do-while loops

### Version 2.0 (Future)

- [ ] Classes and objects
- [ ] Modules and imports
- [ ] Standard library expansion
- [ ] Compile to bytecode
- [ ] Garbage collection

---

## License

Foldr is open-source software released under the MIT License.

```
MIT License

Copyright (c) 2025 Foldr Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## Resources

- **GitHub Repository**: https://github.com/nikolasroufas/foldr
- **Issue Tracker**: https://github.com/nikolasroufas/foldr/issues

---

## Credits

Created and maintained by the Foldr community.

Special thanks to all contributors who have helped make Foldr better!

---

## Changelog

### Version 1.0.0 (2025)

- Initial release
- Core language features
- Basic standard library
- C-based interpreter
- Command-line interface

---

**Happy Coding with Foldr! 🚀**
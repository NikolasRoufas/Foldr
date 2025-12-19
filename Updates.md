# Foldr Programming Language — Update 1.0.1

## 🐛 Bug Fixes
- **Fixed parser crash on built-in function calls**  
  Built-in functions such as `int()`, `str()`, `len()`, and `input()` are now correctly parsed in expressions.  
  Previously, `int(input())` caused a parser error because `int` was tokenized as a type keyword instead of a callable identifier.

- **Improved parser error diagnostics**  
  Parser errors now report:
  - line number
  - offending token
  - token type  
  This makes debugging syntax errors significantly easier.

- **String literal handling improved**  
  The tokenizer now supports both double-quoted (`"..."`) and single-quoted (`'...'`) strings, preventing unexpected token errors from valid input.

---

## ✨ New Features
- **Interactive CLI input**
  - Added built-in `input()` function to read user input from `stdin`
  - Optional prompt support: `input("Enter value: ")`
  - Enables interactive Foldr programs

- **Control flow enhancements**
  - Added full support for `while` loops
  - Added `break;` and `continue;` statements for `for` and `while` loops

- **Const enforcement**
  - `const` variables can no longer be reassigned
  - Reassigning a `const` variable now results in a runtime error

---

## 🔧 Internal Improvements
- Interpreter now correctly respects `return`, `break`, and `continue` control flow flags
- Loop execution logic was refactored for correct early exit and iteration skipping

---

## 🧪 Example (Now Working)
```fld
print("Enter a number:");
let x: int = int(input());
print("You entered: " + str(x));


# Akao Pure Logic Language (.a) Specification v1.0

**akao:artifact:architectural-reformation:phase-3:step-1:akao-language-spec:v1**

## Table of Contents

1. [Introduction](#introduction)
2. [Language Philosophy](#language-philosophy)
3. [Lexical Structure](#lexical-structure)
4. [Syntax Specification](#syntax-specification)
5. [Type System](#type-system)
6. [Logical Operators](#logical-operators)
7. [Quantifiers](#quantifiers)
8. [Predicate Logic](#predicate-logic)
9. [Data Structures](#data-structures)
10. [Control Flow](#control-flow)
11. [Module System](#module-system)
12. [Interoperability](#interoperability)
13. [Standard Library](#standard-library)
14. [Examples](#examples)
15. [Formal Grammar](#formal-grammar)

## Introduction

The Akao Pure Logic Language (.a) is a mathematically-founded programming language designed for logical reasoning, theorem proving, and formal verification. It combines the expressiveness of first-order predicate logic with the practicality of modern programming languages, featuring Python-like syntax with optional mathematical notation.

### Design Goals

- **Mathematical Foundation**: Based on first-order predicate logic with extensions
- **Readability**: Python-like syntax with natural language flow
- **Dual Input Modes**: Unicode mathematical symbols with ASCII fallbacks
- **Zero Dependencies**: No external configuration formats or dependencies
- **Universal Accessibility**: Complete keyboard accessibility for all features
- **Integration Ready**: Seamless integration with C++, Python, and JavaScript

### Key Features

- Formal logical reasoning with automated theorem proving
- Type inference with optional type annotations
- Pattern matching and unification
- Lazy evaluation with memoization
- Built-in constraint satisfaction
- Foreign function interface
- Comprehensive standard library

## Language Philosophy

The .a language embodies the philosophy that logical reasoning should be as natural as writing prose, while maintaining mathematical rigor. Every construct in the language has a clear logical interpretation, enabling both human reasoning and automated verification.

### Core Principles

1. **Logic First**: Every language construct has clear logical semantics
2. **Human Readable**: Code should read like mathematical proofs
3. **Machine Verifiable**: All logic can be mechanically verified
4. **Universally Accessible**: No barriers to entry based on keyboard or display limitations
5. **Interoperable**: Seamless integration with existing ecosystems

## Lexical Structure

### Character Set

The .a language uses UTF-8 encoding and supports the full Unicode character set. Source files should use Unix LF line endings (Windows CRLF is supported).

### Identifiers

```
identifier := [a-zA-Z_][a-zA-Z0-9_]*
namespaced_identifier := identifier(':' identifier)*
```

Examples:
```a
variable_name
predicate_name
akao:core:engine:validator
```

### Literals

#### Numeric Literals
```a
integer := [0-9]+ | 0x[0-9a-fA-F]+ | 0b[01]+
real := [0-9]+\.[0-9]+ | [0-9]+\.[0-9]+[eE][+-]?[0-9]+
```

Examples:
```a
42          // Integer
3.14159     // Real
1.23e-4     // Scientific notation
0xFF        // Hexadecimal
0b1010      // Binary
```

#### String Literals
```a
string := '"' [^"]* '"' | "'" [^']* "'" | '"""' .* '"""'
```

Examples:
```a
"Hello, world!"
'Single quoted string'
"""
Multi-line string
with line breaks preserved
"""
```

#### Boolean Literals
```a
boolean := true | false | ⊤ | ⊥
```

### Comments

```a
// Single line comment
/* Multi-line comment */
/** Documentation comment with metadata */
-- Mathematical annotation
```

### Symbols and Operators

The language supports both Unicode mathematical symbols and ASCII equivalents:

```a
// Logical operators
P ∧ Q    // P and Q
P ∨ Q    // P or Q  
¬P       // not P
P → Q    // P implies Q
P ↔ Q    // P iff Q

// Quantifiers
∀x: P(x)    // forall x: P(x)
∃x: P(x)    // exists x: P(x)

// Set operations
x ∈ S       // x in S
A ⊆ B       // A subset B
A ∪ B       // A union B
A ∩ B       // A intersection B
```

## Syntax Specification

### Basic Structure

A .a file consists of:
1. Optional file metadata
2. Import declarations
3. Type definitions
4. Predicate definitions
5. Facts and rules
6. Main program logic

```a
// File: example.a
/** 
 * akao:example:basic:logic:v1
 * A simple logical reasoning example
 */

import akao:core:logic:predicates
import akao:std:collections

// Type definitions
type Person = {name: String, age: Integer}
type Relationship = Person → Person → Proposition

// Predicate definitions
predicate parent(p: Person, c: Person): Proposition
predicate ancestor(a: Person, d: Person): Proposition := 
    parent(a, d) ∨ ∃x: parent(a, x) ∧ ancestor(x, d)

// Facts
fact parent(alice, bob)
fact parent(bob, charlie)

// Queries and proofs
prove ancestor(alice, charlie)  // Should succeed
```

### Expressions

#### Primary Expressions
```a
primary_expression := 
    | identifier
    | literal
    | '(' expression ')'
    | list_expression
    | set_expression
    | map_expression
```

#### Logical Expressions
```a
logical_expression :=
    | logical_term
    | logical_expression '∧' logical_expression    // and
    | logical_expression '∨' logical_expression    // or
    | '¬' logical_expression                       // not
    | logical_expression '→' logical_expression    // implies
    | logical_expression '↔' logical_expression    // iff
```

#### Quantified Expressions
```a
quantified_expression :=
    | '∀' variable_list ':' expression
    | '∃' variable_list ':' expression
    | '∃!' variable_list ':' expression          // unique existence
```

### Statements

#### Variable Declarations
```a
let variable_name: Type = expression
let x: Integer = 42
let P: Person → Proposition = λp: p.age > 18
```

#### Predicate Definitions
```a
predicate name(parameters): return_type := body

predicate even(n: Integer): Proposition := n % 2 = 0
predicate prime(n: Integer): Proposition := 
    n > 1 ∧ ∀d ∈ 2..sqrt(n): ¬(n % d = 0)
```

#### Facts and Rules
```a
fact expression                    // Assert as true
rule premise → conclusion          // Logical rule
```

#### Proof Blocks
```a
prove expression:
    // Proof steps
    step1: premise1
    step2: premise2 
    conclusion: step1 ∧ step2 → target
```

## Type System

The .a language features a rich type system with inference, parametric polymorphism, and dependent types.

### Primitive Types

```a
Integer     // Arbitrary precision integers
Real        // Floating-point numbers  
String      // Unicode text strings
Boolean     // True/false values
Proposition // Logical propositions
```

### Composite Types

#### Function Types
```a
Type1 → Type2              // Function from Type1 to Type2
(Type1, Type2) → Type3     // Multi-argument function
```

#### Collection Types
```a
List[T]        // Ordered list of T
Set[T]         // Unordered set of T
Map[K, V]      // Key-value mapping
```

#### Product Types
```a
type Person = {
    name: String,
    age: Integer,
    email: String?    // Optional field
}
```

#### Sum Types
```a
type Option[T] = Some(T) | None
type Result[T, E] = Ok(T) | Error(E)
```

### Type Inference

The language uses Hindley-Milner type inference with extensions for logical types:

```a
// Type inferred as Integer → Boolean
let is_positive = λx: x > 0

// Type inferred as ∀T: List[T] → Integer
let length = λlist: match list:
    | [] → 0
    | head :: tail → 1 + length(tail)
```

## Logical Operators

### Truth-Functional Operators

```a
// Conjunction (AND)
P ∧ Q    // P and Q
true ∧ false = false

// Disjunction (OR)  
P ∨ Q    // P or Q
true ∨ false = true

// Negation (NOT)
¬P       // not P
¬true = false

// Implication
P → Q    // P implies Q
false → true = true

// Biconditional  
P ↔ Q    // P iff Q
true ↔ true = true

// Exclusive OR
P ⊕ Q    // P xor Q
true ⊕ true = false
```

### Operator Precedence

1. Negation (`¬`, `not`) - highest precedence
2. Conjunction (`∧`, `and`)
3. Disjunction (`∨`, `or`)
4. Exclusive OR (`⊕`, `xor`)
5. Implication (`→`, `implies`)
6. Biconditional (`↔`, `iff`) - lowest precedence

### Short-Circuit Evaluation

Logical operators use short-circuit evaluation for efficiency:

```a
expensive_computation() ∨ true    // expensive_computation() not called
false ∧ expensive_computation()   // expensive_computation() not called
```

## Quantifiers

### Universal Quantification

```a
∀x: P(x)           // forall x: P(x)
∀x ∈ S: P(x)       // forall x in S: P(x)
∀x,y: P(x,y)       // forall x, y: P(x, y)
```

Examples:
```a
// All natural numbers are non-negative
∀n ∈ ℕ: n ≥ 0

// Transitivity of less-than
∀x,y,z ∈ ℝ: (x < y ∧ y < z) → x < z
```

### Existential Quantification

```a
∃x: P(x)           // exists x: P(x)
∃x ∈ S: P(x)       // exists x in S: P(x)
∃!x: P(x)          // exists_unique x: P(x)
```

Examples:
```a
// There exists a prime number greater than 100
∃p ∈ ℕ: p > 100 ∧ prime(p)

// There is exactly one identity element
∃!e: ∀x: x + e = x ∧ e + x = x
```

### Bounded Quantification

```a
∀x ∈ 1..10: even(x) ∨ odd(x)     // Range quantification
∃x ∈ primes: x > 1000            // Set quantification  
∀p ∈ people where p.age > 18: can_vote(p)  // Filtered quantification
```

## Predicate Logic

### Predicate Definitions

```a
// Simple predicate
predicate even(n: Integer): Proposition := n % 2 = 0

// Recursive predicate
predicate factorial(n: Integer): Integer := match n:
    | 0 → 1
    | n where n > 0 → n * factorial(n - 1)

// Higher-order predicate
predicate all_satisfy(list: List[T], p: T → Proposition): Proposition :=
    ∀x ∈ list: p(x)
```

### Pattern Matching

```a
predicate classify_number(n: Integer): String := match n:
    | 0 → "zero"
    | n where n > 0 → "positive"
    | n where n < 0 → "negative"

predicate list_length(list: List[T]): Integer := match list:
    | [] → 0
    | head :: tail → 1 + list_length(tail)
```

### Unification

The language supports automatic unification for logical variables:

```a
// Variables starting with ? are logical variables
predicate append(xs: List[T], ys: List[T], result: List[T]): Proposition := match xs:
    | [] → result = ys
    | head :: tail → ∃?rest: append(tail, ys, ?rest) ∧ result = head :: ?rest

// Query with unification
query append([1, 2], [3, 4], ?result)  // ?result = [1, 2, 3, 4]
query append(?prefix, [3, 4], [1, 2, 3, 4])  // ?prefix = [1, 2]
```

## Data Structures

### Lists

```a
// List literals
let numbers = [1, 2, 3, 4, 5]
let names = ["Alice", "Bob", "Charlie"]
let empty_list: List[Integer] = []

// List operations
let head = numbers[0]              // First element
let tail = numbers[1..]            // All but first
let length = |numbers|             // Length operator
let concatenated = [1, 2] ++ [3, 4]  // Concatenation

// List comprehensions
let squares = [x * x | x ∈ 1..10]
let evens = [x | x ∈ numbers where even(x)]
```

### Sets

```a
// Set literals
let prime_digits = {2, 3, 5, 7}
let vowels = {'a', 'e', 'i', 'o', 'u'}

// Set operations
let is_member = 3 ∈ prime_digits        // Membership
let subset_check = {2, 3} ⊆ prime_digits  // Subset
let union_set = {1, 2} ∪ {2, 3}        // Union: {1, 2, 3}
let intersection = {1, 2, 3} ∩ {2, 3, 4}  // Intersection: {2, 3}
let difference = {1, 2, 3} ∖ {2}       // Difference: {1, 3}

// Set comprehensions
let even_squares = {x * x | x ∈ 1..10 where even(x)}
```

### Maps

```a
// Map literals
let ages = {"Alice": 25, "Bob": 30, "Charlie": 35}
let empty_map: Map[String, Integer] = {}

// Map operations
let alice_age = ages["Alice"]          // Lookup
let updated_ages = ages + {"Alice": 26}  // Update
let has_key = "Alice" ∈ ages          // Key membership
let keys = keys(ages)                  // Get all keys
let values = values(ages)              // Get all values
```

### Records

```a
type Person = {
    name: String,
    age: Integer,
    email: String?,
    address: Address
}

type Address = {
    street: String,
    city: String,
    zipcode: String
}

// Record creation
let alice = Person {
    name: "Alice",
    age: 25,
    email: Some("alice@example.com"),
    address: Address {
        street: "123 Main St",
        city: "Anytown", 
        zipcode: "12345"
    }
}

// Field access
let name = alice.name
let city = alice.address.city

// Record update
let older_alice = alice with {age: 26}
```

## Control Flow

### Conditional Expressions

```a
// If expressions
let result = if condition then value1 else value2

let status = if age ≥ 18 then "adult" else "minor"

// Multi-way conditionals
let grade = match score:
    | s where s ≥ 90 → "A"
    | s where s ≥ 80 → "B"
    | s where s ≥ 70 → "C"
    | s where s ≥ 60 → "D"
    | _ → "F"
```

### Loops and Iteration

```a
// For loops with list comprehension syntax
for x ∈ 1..10:
    print(x)

for person ∈ people where person.age > 18:
    print(person.name + " is an adult")

// While loops
let i = 0
while i < 10:
    print(i)
    i := i + 1

// Recursive iteration (preferred style)
predicate print_numbers(n: Integer): Unit := match n:
    | 0 → unit
    | n where n > 0 → 
        print(n)
        print_numbers(n - 1)
```

### Exception Handling

```a
type Result[T, E] = Ok(T) | Error(E)

predicate safe_divide(x: Real, y: Real): Result[Real, String] :=
    if y = 0 then 
        Error("Division by zero")
    else 
        Ok(x / y)

// Pattern matching on results
let result = safe_divide(10, 2)
match result:
    | Ok(value) → print("Result: " + string(value))
    | Error(message) → print("Error: " + message)
```

## Module System

### Module Declaration

```a
// File: math/geometry.a
module akao:math:geometry

import akao:std:math
import akao:std:collections

export Point, Line, Circle, distance, area
```

### Imports

```a
// Import entire module
import akao:math:geometry

// Import specific items
import akao:math:geometry:Point, distance

// Import with alias
import akao:very:long:module:name as short_name

// Qualified import
import qualified akao:math:geometry as geo
let point = geo:Point(3, 4)
```

### Exports

```a
// Export specific items
export Point, Line, distance

// Export all
export *

// Export with alias
export distance as euclidean_distance

// Conditional exports
export debug_functions if DEBUG_MODE
```

## Interoperability

### C++ Integration

```a
// Foreign function declarations
foreign cpp:
    function sqrt(x: Real): Real
    function malloc(size: Integer): Pointer[Byte]
    
// C++ class binding
foreign cpp class Vector3:
    constructor(x: Real, y: Real, z: Real)
    method length(): Real
    method normalize(): Vector3

// Usage
let v = Vector3(1.0, 2.0, 3.0)
let length = v.length()
```

### Python Integration

```a
// Python function binding
foreign python:
    import numpy as np
    function matrix_multiply(a: Matrix, b: Matrix): Matrix = "np.matmul"

// Python class binding  
foreign python class DataFrame:
    constructor(data: Map[String, List[Any]])
    method head(n: Integer = 5): DataFrame
    method to_csv(filename: String): Unit
```

### JavaScript Integration

```a
// JavaScript function binding (for web/Node.js)
foreign javascript:
    function fetch(url: String): Promise[Response]
    function JSON.parse(text: String): Any
    function console.log(message: String): Unit
```

## Standard Library

### Core Logic
```a
akao:std:logic         // Logical reasoning utilities
akao:std:proof         // Proof construction and verification
akao:std:unification   // Unification algorithms
akao:std:resolution    // Resolution theorem proving
```

### Data Structures
```a
akao:std:collections   // Lists, sets, maps, queues, stacks
akao:std:trees         // Tree data structures
akao:std:graphs        // Graph algorithms
akao:std:strings       // String manipulation
```

### Mathematical Libraries
```a
akao:std:math          // Basic mathematical functions
akao:std:statistics    // Statistical functions
akao:std:linear_algebra // Linear algebra operations
akao:std:calculus      // Symbolic calculus
```

### I/O and System
```a
akao:std:io            // File and network I/O
akao:std:system        // System operations
akao:std:process       // Process management
akao:std:time          // Date and time utilities
```

## Examples

### Basic Logic Programming

```a
// File: family.a
/** Family relationships using logical predicates */

type Person = String

// Basic relationships
predicate parent(p: Person, c: Person): Proposition
predicate male(p: Person): Proposition  
predicate female(p: Person): Proposition

// Derived relationships
predicate father(f: Person, c: Person): Proposition := 
    parent(f, c) ∧ male(f)

predicate mother(m: Person, c: Person): Proposition := 
    parent(m, c) ∧ female(m)

predicate grandparent(gp: Person, gc: Person): Proposition := 
    ∃p: parent(gp, p) ∧ parent(p, gc)

predicate sibling(s1: Person, s2: Person): Proposition := 
    s1 ≠ s2 ∧ ∃p: parent(p, s1) ∧ parent(p, s2)

// Facts
fact parent("John", "Alice")
fact parent("Mary", "Alice") 
fact parent("Alice", "Bob")
fact parent("Alice", "Charlie")
fact male("John")
fact male("Bob")
fact female("Mary")
fact female("Alice")
fact female("Charlie")

// Queries
prove father("John", "Alice")        // true
prove sibling("Bob", "Charlie")      // true
prove grandparent("John", "Bob")     // true

query ∃x: grandparent(x, "Bob")      // x = "John", "Mary"
```

### Mathematical Reasoning

```a
// File: number_theory.a
/** Number theory predicates and proofs */

predicate divides(a: Integer, b: Integer): Proposition := 
    ∃k: b = k * a

predicate prime(n: Integer): Proposition := 
    n > 1 ∧ ∀d ∈ 2..(n-1): ¬divides(d, n)

predicate gcd(a: Integer, b: Integer, result: Integer): Proposition := match (a, b):
    | (a, 0) → result = a
    | (a, b) where b > 0 → gcd(b, a % b, result)

// Euclidean algorithm theorem
theorem euclidean_algorithm(a: Integer, b: Integer): 
    ∃g: gcd(a, b, g) ∧ ∃x,y: g = x*a + y*b :=
proof:
    // Proof by strong induction on b
    base_case: b = 0 → g = a, x = 1, y = 0
    inductive_step: 
        assume ∃g': gcd(b, a % b, g') ∧ ∃x',y': g' = x'*b + y'*(a % b)
        let g = g', x = y', y = x' - (a / b) * y'
        show g = x*a + y*b by algebraic_manipulation
```

### Constraint Satisfaction

```a
// File: sudoku.a
/** Sudoku solver using constraint logic programming */

type Cell = Integer  // 1-9
type Grid = Array[9, Array[9, Cell?]]

predicate valid_row(grid: Grid, row: Integer): Proposition := 
    ∀c1,c2 ∈ 0..8: c1 ≠ c2 → (grid[row][c1] ≠ grid[row][c2] ∨ 
                                 grid[row][c1] = None ∨ 
                                 grid[row][c2] = None)

predicate valid_column(grid: Grid, col: Integer): Proposition := 
    ∀r1,r2 ∈ 0..8: r1 ≠ r2 → (grid[r1][col] ≠ grid[r2][col] ∨ 
                                 grid[r1][col] = None ∨ 
                                 grid[r2][col] = None)

predicate valid_box(grid: Grid, box_row: Integer, box_col: Integer): Proposition := 
    ∀r1,c1,r2,c2 ∈ box_cells(box_row, box_col): 
        (r1,c1) ≠ (r2,c2) → (grid[r1][c1] ≠ grid[r2][c2] ∨ 
                              grid[r1][c1] = None ∨ 
                              grid[r2][c2] = None)

predicate valid_sudoku(grid: Grid): Proposition := 
    (∀r ∈ 0..8: valid_row(grid, r)) ∧
    (∀c ∈ 0..8: valid_column(grid, c)) ∧  
    (∀br,bc ∈ 0..2: valid_box(grid, br, bc))

predicate complete_sudoku(grid: Grid): Proposition := 
    valid_sudoku(grid) ∧ (∀r,c ∈ 0..8: grid[r][c] ≠ None)

// Solver using constraint satisfaction
solve sudoku(partial_grid: Grid): Grid? := 
    search grid where complete_sudoku(grid) and extends(grid, partial_grid)
```

## Formal Grammar

### Lexical Grammar

```ebnf
(* Lexical tokens *)
IDENTIFIER = letter (letter | digit | '_')* ;
INTEGER = digit+ | '0x' hex_digit+ | '0b' binary_digit+ ;
REAL = digit+ '.' digit+ ('e' ['+' | '-'] digit+)? ;
STRING = '"' (char - '"')* '"' | "'" (char - "'")* "'" ;
COMMENT = '//' (char - newline)* newline | '/*' (char - '*/')* '*/' ;

(* Character classes *)
letter = 'a'..'z' | 'A'..'Z' | '_' ;
digit = '0'..'9' ;
hex_digit = digit | 'a'..'f' | 'A'..'F' ;
binary_digit = '0' | '1' ;
```

### Syntactic Grammar

```ebnf
(* Program structure *)
program = module_header? import_list? item_list? ;
module_header = 'module' qualified_identifier ;
import_list = import_declaration* ;
item_list = item* ;

(* Imports *)
import_declaration = 'import' qualified_identifier ('as' IDENTIFIER)? ;

(* Items *)
item = type_definition | predicate_definition | fact_declaration | 
       rule_declaration | proof_block ;

(* Type definitions *)
type_definition = 'type' IDENTIFIER type_params? '=' type_expression ;
type_params = '[' IDENTIFIER (',' IDENTIFIER)* ']' ;
type_expression = function_type | product_type | sum_type | type_reference ;

(* Expressions *)
expression = logical_expression | arithmetic_expression | application | 
             literal | identifier | parenthesized_expression ;

logical_expression = quantified_expression | binary_logical | unary_logical ;
quantified_expression = quantifier variable_list ':' expression ;
quantifier = '∀' | 'forall' | '∃' | 'exists' | '∃!' | 'exists_unique' ;

binary_logical = expression logical_operator expression ;
logical_operator = '∧' | 'and' | '∨' | 'or' | '→' | 'implies' | 
                   '↔' | 'iff' | '⊕' | 'xor' ;

unary_logical = ('¬' | 'not') expression ;

(* Statements *)
statement = let_binding | assignment | expression_statement | 
            if_statement | match_statement | for_statement | while_statement ;

let_binding = 'let' pattern type_annotation? '=' expression ;
pattern = IDENTIFIER | tuple_pattern | list_pattern | record_pattern ;
```

---

## Conclusion

The Akao Pure Logic Language (.a) provides a powerful foundation for logical reasoning and formal verification while maintaining the accessibility and readability of modern programming languages. Its dual-mode input system ensures universal accessibility while its rich type system and logical foundation enable sophisticated mathematical reasoning.

This specification serves as the complete reference for the .a language implementation in the Akao project's Phase 3 technology evolution.

**Version**: 1.0  
**Date**: 2025-08-05  
**Authors**: Akao Architecture Team  
**Status**: Active Development  

---

*For implementation details, see the corresponding grammar files and parser implementation in the core engine.*

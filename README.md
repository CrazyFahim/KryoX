# KryoX the linear operator compiler (LOC)

An experimental **compiler + runtime** for linear operator algebra, inspired by
machine-learning systems such as **PyTorch**, **XLA**, and **MLIR**.

LOC parses a small domain-specific language (DSL) for linear operators,
lowers it into an intermediate representation (IR), applies optimization
passes, and executes it on a simple matrix runtime.

This project is primarily **educational and exploratory**, focusing on how
compiler techniques map naturally to ML workloads.

---

## Features

### Language (DSL)

```loc
operator D = [[0, 1], [-1, 0]];
operator F = [[1, 0], [0, 1]];

# Supports comments
L = 3 * (F @ D);
print L;
```

### More Examples

**Scalar Operations & Precedence**
```loc
operator A = [[1, 2], [3, 4]];
# Scalars multiply with matrices
B = 2.5 * A; 
# Precedence: (B @ A) happens first
C = 10 * (B @ A);
print C;
```

**Runtime Memoization (DAG Reuse)**
```loc
operator A = [[2, 0], [0, 2]];
# 'B' is computed once and cached
B = A @ A;
# 'B' is reused from cache, not recomputed
C = B + B;
print C; 
```

**Error Handling**
```loc
operator A = [[1, 2]]; # 1x2
operator B = [[1], [2]]; # 2x1
# Error: shape mismatch (1x2 + 2x1)
C = A + B; 
```

### Key Capabilities
- **Matrix Literals**: Define matrices directly in code, including negative values.
- **Composition**: Use `@` for matrix multiplication/composition.
- **Optimizations**:
    - **Constant Folding**: Pre-calculates constant expressions.
    - **Dead Code Elimination**: Removes unused variables.
    - **Runtime Memoization**: Caches intermediate results to avoid redundant computations in DAGs.
- **Runtime Safety**: Checks for shape mismatches and syntax errors with line number reporting.

## Included Tests

The `examples/` folder covers:
- Matrix literals (including negative values)
- Operator composition and precedence
- Constant folding
- Dead code elimination
- Non-commutativity of composition
- Runtime shape mismatch errors
- Runtime memoization (DAG reuse)

---

## Build & Run

### Prerequisites
- CMake (3.16+)
- C++17 Compiler (GCC/Clang)
- Flex & Bison

### Building
```bash
mkdir build && cd build
cmake ..
make
```

### Running
Run the compiler on a `.loc` file:
```bash
./build/loc examples/test.loc
```

### Running Tests
Use the automated test runner to execute the full suite:
```bash
python3 tests/runner.py
```
This will run all `.loc` files in `examples/` and check for expected success or failure.

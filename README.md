** # KryoX the linear operator compiler (LOC)**

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

L = 3 * (F @ D);
print L;

```

## Included tests cover:

- Matrix literals (including negative values)
- Operator composition and precedence
- Constant folding
- Dead code elimination
- Non-commutativity of composition
- Runtime shape mismatch errors

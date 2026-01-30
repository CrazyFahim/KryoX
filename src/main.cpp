// MINIMAL PRINT + RUNTIME (matrix literals enabled)
#include <iostream>
#include <stdexcept>

#include "loc/frontend/ast.hpp"
#include "loc/passes/simplify.hpp"
#include "loc/passes/resolve_prints.hpp"

#include "loc/ir/lower.hpp"
#include "loc/ir/pass_manager.hpp"
#include "loc/ir/passes/const_fold.hpp"
#include "loc/ir/passes/dce.hpp"

// RUNTIME (matrix backend)
#include "loc/runtime/registry.hpp"
#include "loc/runtime/executor.hpp"
#include "loc/runtime/matrix.hpp"

int yyparse();
extern loc::ast::Program* g_program;
extern FILE* yyin;

// Convert AST MatrixLiteral -> runtime Matrix
static loc::rt::Matrix to_matrix(const loc::ast::MatrixLiteral& lit) {
    const auto& r = lit.rows;
    if (r.empty() || r[0].empty()) {
        throw std::runtime_error("Empty matrix literal");
    }

    const size_t R = r.size();
    const size_t C = r[0].size();

    for (size_t i = 0; i < R; ++i) {
        if (r[i].size() != C) {
            throw std::runtime_error("Non-rectangular matrix literal");
        }
    }

    loc::rt::Matrix M(R, C);
    for (size_t i = 0; i < R; ++i) {
        for (size_t j = 0; j < C; ++j) {
            M(i, j) = r[i][j];
        }
    }
    return M;
}

int main(int argc, char** argv) {
    // 0) Handle Input
    if (argc > 1) {
        FILE* f = fopen(argv[1], "r");
        if (!f) {
            std::cerr << "Error: could not open file " << argv[1] << "\n";
            return 1;
        }
        yyin = f;
    }

    // 1) Parse
    if (yyparse() != 0) {
        return 1;
    }
    if (!g_program) return 1;

    // 2) AST passes
    loc::passes::simplify_program(*g_program);
    loc::passes::resolve_prints(*g_program);

    // 3) Lower to IR
    auto ir = loc::ir::lower_program(*g_program);

    // 4) IR passes
    loc::ir::PassManager pm;
    pm.add("const_fold", loc::ir::passes::const_fold);
    pm.add("dce",        loc::ir::passes::dead_code_elim);
    pm.run(ir);

    // 5) Dump IR (debug)
    ir.dump();

    // 6) Runtime: build registry from operator declarations
    loc::rt::Registry reg;

    // Default size for fallback identity (only used if operator has no init)
    const size_t DEFAULT_N = 2;

    for (const auto& st : g_program->statements) {
        if (auto* od = dynamic_cast<loc::ast::OperatorDecl*>(st.get())) {
            if (od->init) {
                reg.set(od->name, to_matrix(*od->init));
            } else {
                // Optional fallback: operator declared but not defined
                reg.set(od->name, loc::rt::Matrix::identity(DEFAULT_N));
            }
        }
    }

    // 7) Execute (catch runtime errors so tests don't "Abort")
    try {
        loc::rt::Executor ex(reg);
        ex.run(ir);
    } catch (const std::exception& e) {
        std::cerr << "[runtime error] " << e.what() << "\n";
        return 2; // clean nonzero exit (useful for expected-fail tests)
    }

    return 0;
}

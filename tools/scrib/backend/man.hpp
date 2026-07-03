#pragma once

#include <ast.hpp>
#include <ostream>

void GenerateManTroff(
    std::ostream& out,
    const Document& document,
    const char* title,
    const char* section,
    const char* source,
    const char* manual
);

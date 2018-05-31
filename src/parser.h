#ifndef WS_PARSER_H_
#define WS_PARSER_H_

#include <cstdio>
#include "instruction.h"

namespace WS {

struct ParseException {
    const char* what;
    const size_t line;
    const size_t col;

    ParseException(const char* what, size_t line, size_t col) : what(what), line(line), col(col) {}
};

class Parser {
public:
    Parser(FILE* stream);

    Instruction next();
    bool next(Instruction& instr);
    bool isEOF() const;

private:
    FILE* stream_;
    size_t line_ = 1;
    size_t col_ = 0;
    bool isEOF_ = false;

    Instruction parseStack();
    Instruction parseArith();
    Instruction parseHeap();
    Instruction parseFlow();
    Instruction parseIO();

    char nextChar();
    integer_t readInteger();
    integer_t readUnsignedInteger();

    ParseException unexpectedException() const;
    ParseException badCharException() const;
};

} // namespace WS

#endif

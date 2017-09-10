// Flex Demo 2 - main.cpp

#include "lexer-decls.hpp" // Token type enum (T_*)
#include "lexer.hpp"       // yy* types and methods

#include <cassert>  // assert
#include <cstdio>   // perror
#include <cstdlib>  // exit
#include <iostream> // cout, endl
#include <sstream>  // ostringstream
#include <string>   // string

namespace {
std::string describe(int tokenNumber) {
  switch (tokenNumber) {
  case END_OF_INPUT:
    return "(end of input)";
  case T_EQ:
    return "=";
  case T_EQ_EQ:
    return "==";
  case T_BREAK:
    return "break";
  default: {
    std::ostringstream out;
    out << static_cast<char>(tokenNumber);
    return out.str();
  }
  }
}

void describe_tokens(const char *string) {
  // Create a new lexer
  yyscan_t scanner;
  if (yylex_init(&scanner)) {
    perror("Error initializing lexer");
    exit(1);
  }

  // Copy the string to scan into a buffer
  // (note: copying may not be desirable in production)
  YY_BUFFER_STATE buf = yy_scan_string(string, scanner);

  std::cout << "Tokens for \"" << string << "\" are:" << std::endl;

  // Repeatedly get the next token from the lexer, stopping after the
  // end-of-input token
  int tokenNumber;
  do {
    tokenNumber = yylex(scanner);
    std::cout << describe(tokenNumber) << ' ';
  } while (tokenNumber != END_OF_INPUT);

  std::cout << std::endl << std::endl;

  // Destroy the buffer and the lexer
  yy_delete_buffer(buf, scanner);
  yylex_destroy(scanner);
}
} // namespace

int main(int argc, char **argv) {
  describe_tokens("");
  describe_tokens("break===\n\nx");
  describe_tokens("=====");
  describe_tokens("breakbreak");
  describe_tokens("\x84"); // Non-ASCII
  return 0;
}

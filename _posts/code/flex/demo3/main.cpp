// Flex Demo 3 - main.cpp

#include "lexer-decls.hpp" // YYSTYPE must be defined before including lexer.hpp
#include "lexer.hpp"       // yy* types and methods

#include <cassert>  // assert
#include <cstdio>   // perror
#include <cstdlib>  // exit
#include <iostream> // cout, endl

namespace {
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
  YYSTYPE tokenData;
  int tokenNumber;
  do {
    tokenNumber = yylex(&tokenData, scanner);
    std::cout << tokenNumber << " (" << tokenData.text << ")" << std::endl;
  } while (tokenNumber != END_OF_INPUT);

  std::cout << std::endl;

  // Destroy the buffer and the lexer
  yy_delete_buffer(buf, scanner);
  yylex_destroy(scanner);
}
} // namespace

int main(int argc, char **argv) {
  describe_tokens("");
  describe_tokens("foofo");
  describe_tokens("foofobarf");
  describe_tokens("Hi\nfoo!");
  describe_tokens("\x84"); // Non-ASCII
  return 0;
}

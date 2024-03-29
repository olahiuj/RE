/* A Regular Expression Engine
 * Valid Characters:
 * (,),*,|
 * @ stands for \epsilon
 */

#include <cctype>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "DFA.hpp"
#include "Expr.hpp"
#include "NFA.hpp"

std::string preProc(std::string input) {
  std::stringstream result;
  char last = 0;
  for (char ch : input) {
    if (last) {
      if (last != '|' && last != '(' && ch != '|' && ch != '*' && ch != ')') {
        result << '+';
      }
    }
    result << ch;
    last = ch;
  }
  return result.str();
}

std::string proc(const std::string &input) {
  std::stack<char> char_stack;
  std::string processed_input = preProc(input);
  std::stringstream result;
  std::map<char, int> priority{{'|', 1}, {'+', 2}, {'*', 3}};

  for (char ch : processed_input) {
    switch (ch) {
    case '(': {
      char_stack.push(ch);
      break;
    }
    case ')': {
      while (!char_stack.empty() && char_stack.top() != '(') {
        result << char_stack.top();
        char_stack.pop();
      }
      char_stack.pop();
      break;
    }
    case '|':
    case '*':
    case '+': {
      while (!char_stack.empty() &&
             priority[char_stack.top()] >= priority[ch]) {
        result << char_stack.top();
        char_stack.pop();
      }
      char_stack.push(ch);
      break;
    }
    default: {
      result << ch;
      break;
    }
    }
  }
  while (!char_stack.empty()) {
    result << char_stack.top();
    char_stack.pop();
  }
  std::cout << result.str() << std::endl;
  return result.str();
}

std::shared_ptr<Expr> parse(const std::string &input) {
  std::stack<std::shared_ptr<Expr>> expr_stack;

  for (char ch : proc(input)) {
    switch (ch) {
    case '|': {
      auto right_expr = expr_stack.top();
      expr_stack.pop();
      auto left_expr = expr_stack.top();
      expr_stack.pop();
      expr_stack.push(std::make_shared<OrExpr>(OrExpr(left_expr, right_expr)));
      break;
    }
    case '+': {
      auto right_expr = expr_stack.top();
      expr_stack.pop();
      auto left_expr = expr_stack.top();
      expr_stack.pop();
      expr_stack.push(
          std::make_shared<ThenExpr>(ThenExpr(left_expr, right_expr)));
      break;
    }
    case '*': {
      auto sub = expr_stack.top();
      expr_stack.pop();
      expr_stack.push(std::make_shared<StarExpr>(StarExpr(sub)));
      break;
    }
    default: {
      expr_stack.push(std::make_shared<CharExpr>(CharExpr(ch)));
    }
    }
  }
  return expr_stack.top();
}

std::shared_ptr<NFA> buildNFA(std::shared_ptr<Expr> root) {
  std::size_t size = root->calc_nfa_size();
  std::cout << "size = " << size << std::endl;

  std::shared_ptr<NFA> nfa = std::make_shared<NFA>(NFA(size));
  root->build(nfa, 0);

  nfa->print();
  return nfa;
}

extern int repl(std::shared_ptr<DFA> dfa);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: RE pattern");
    exit(1);
  }

  auto expr = parse(std::string{argv[1]});
  auto nfa = buildNFA(expr);

  auto dfa = nfa->toDFA();
  dfa->print_to_file("origin_DFA");

  auto min_dfa = dfa->minimize();
  min_dfa->print_to_file("minimized_DFA");

  return repl(min_dfa);
}
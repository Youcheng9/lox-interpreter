package com.craftinginterpreters.lox;

class InterpolationToken {
  final InterpolationTokenType type;
  final String lexeme;
  final int line;

  InterpolationToken(InterpolationTokenType type, String lexeme, int line) {
    this.type = type;
    this.lexeme = lexeme;
    this.line = line;
  }

  @Override
  public String toString() { return type + " " + lexeme; }
}

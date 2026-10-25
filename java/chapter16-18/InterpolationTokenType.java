package com.craftinginterpreters.lox;

enum InterpolationTokenType {
  STRING_START, STRING_TEXT, INTERPOLATION_START, INTERPOLATION_END,
  STRING_END, IDENTIFIER, NUMBER, PLUS, EOF
}

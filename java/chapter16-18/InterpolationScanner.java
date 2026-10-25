package com.craftinginterpreters.lox;

import java.util.ArrayList;
import java.util.List;

class InterpolationScanner {
  private final String source;
  private final List<InterpolationToken> tokens = new ArrayList<>();
  private int current;
  private int line = 1;

  InterpolationScanner(String source) { this.source = source; }

  List<InterpolationToken> scanTokens() {
    scanCode(false);
    tokens.add(new InterpolationToken(InterpolationTokenType.EOF, "", line));
    return tokens;
  }

  private void scanCode(boolean interpolation) {
    while (!isAtEnd()) {
      if (interpolation && peek() == '}') {
        add(InterpolationTokenType.INTERPOLATION_END, String.valueOf(advance()));
        return;
      }
      char c = advance();
      if (c == '"') scanString();
      else if (c == '+') add(InterpolationTokenType.PLUS, "+");
      else if (Character.isLetter(c) || c == '_') identifier(c);
      else if (Character.isDigit(c)) number(c);
      else if (c == '{' && interpolation) scanCode(true);
      else if (c == ' ' || c == '\r' || c == '\t') { }
      else if (c == '\n') line++;
    }
  }

  private void scanString() {
    add(InterpolationTokenType.STRING_START, "\"");
    int textStart = current;
    while (!isAtEnd()) {
      if (peek() == '"') {
        text(textStart, current);
        add(InterpolationTokenType.STRING_END, String.valueOf(advance()));
        return;
      }
      if (peek() == '$' && peekNext() == '{') {
        text(textStart, current);
        advance(); advance();
        add(InterpolationTokenType.INTERPOLATION_START, "${");
        scanCode(true);
        textStart = current;
      } else {
        if (peek() == '\n') line++;
        advance();
      }
    }
  }

  private void identifier(char first) {
    int start = current - 1;
    while (Character.isLetterOrDigit(peek()) || peek() == '_') advance();
    add(InterpolationTokenType.IDENTIFIER, source.substring(start, current));
  }

  private void number(char first) {
    int start = current - 1;
    while (Character.isDigit(peek())) advance();
    add(InterpolationTokenType.NUMBER, source.substring(start, current));
  }

  private void text(int start, int end) {
    if (end > start) add(InterpolationTokenType.STRING_TEXT, source.substring(start, end));
  }
  private char peek() { return isAtEnd() ? '\0' : source.charAt(current); }
  private char peekNext() { return current + 1 >= source.length() ? '\0' : source.charAt(current + 1); }
  private char advance() { return source.charAt(current++); }
  private boolean isAtEnd() { return current >= source.length(); }
  private void add(InterpolationTokenType type, String lexeme) { tokens.add(new InterpolationToken(type, lexeme, line)); }
}

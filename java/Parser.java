private Expr unary() {
  if (match(BANG, MINUS)) {
    Token operator = previous();
    Expr right = unary();
    return new Expr.Unary(operator, right);
  }

  // Error productions for binary operators
  if (match(BANG_EQUAL, EQUAL_EQUAL)) {
    Token operator = previous();
    error(operator, "Missing left-hand operand.");
    comparison();
    return null;
  }

  if (match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
    Token operator = previous();
    error(operator, "Missing left-hand operand.");
    term();
    return null;
  }

  if (match(PLUS)) {
    Token operator = previous();
    error(operator, "Missing left-hand operand.");
    factor();
    return null;
  }

  if (match(SLASH, STAR)) {
    Token operator = previous();
    error(operator, "Missing left-hand operand.");
    unary();
    return null;
  }

  return primary();
}
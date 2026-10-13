//> Functions lox-function
package com.craftinginterpreters.lox;

import java.util.List;

class LoxFunction implements LoxCallable {
  private final Stmt.Function declaration;
  private final Expr.Function expressionDeclaration;
//> closure-field
  private final Environment closure;
  
//< closure-field
/* Functions lox-function < Functions closure-constructor
  LoxFunction(Stmt.Function declaration) {
*/
/* Functions closure-constructor < Classes is-initializer-field
  LoxFunction(Stmt.Function declaration, Environment closure) {
*/
//> Classes is-initializer-field
  private final boolean isInitializer;

  LoxFunction(Stmt.Function declaration, Environment closure,
              boolean isInitializer) {
    this.isInitializer = isInitializer;
//< Classes is-initializer-field
//> closure-constructor
    this.closure = closure;
//< closure-constructor
    this.declaration = declaration;
    this.expressionDeclaration = null;
  }

  LoxFunction(Expr.Function declaration, Environment closure) {
    this.declaration = null;
    this.expressionDeclaration = declaration;
    this.closure = closure;
    this.isInitializer = false;
  }
//> Classes bind-instance
  LoxFunction bind(LoxInstance instance) {
    Environment environment = new Environment(closure);
    environment.define(instance);
/* Classes bind-instance < Classes lox-function-bind-with-initializer
    return new LoxFunction(declaration, environment);
*/
//> lox-function-bind-with-initializer
    return new LoxFunction(declaration, environment,
                           isInitializer);
//< lox-function-bind-with-initializer
  }
//< Classes bind-instance
//> function-to-string
  @Override
  public String toString() {
    if (expressionDeclaration != null) return "<fn>";
    return "<fn " + declaration.name.lexeme + ">";
  }
//< function-to-string
//> function-arity
  @Override
  public int arity() {
    if (expressionDeclaration != null) return expressionDeclaration.parameters.size();
    return declaration.params.size();
  }
//< function-arity
//> function-call
  @Override
  public Object call(Interpreter interpreter,
                     List<Object> arguments) {
/* Functions function-call < Functions call-closure
    Environment environment = new Environment(interpreter.globals);
*/
//> call-closure
    Environment environment = new Environment(closure);
//< call-closure
    if (expressionDeclaration != null) {
      for (int i = 0; i < expressionDeclaration.parameters.size(); i++) {
        environment.define(arguments.get(i));
      }
    } else {
      for (int i = 0; i < declaration.params.size(); i++) {
        environment.define(arguments.get(i));
      }
    }

/* Functions function-call < Functions catch-return
    interpreter.executeBlock(declaration.body, environment);
*/
//> catch-return
    try {
      interpreter.executeBlock(expressionDeclaration != null
          ? expressionDeclaration.body : declaration.body, environment);
    } catch (Return returnValue) {
//> Classes early-return-this
      if (isInitializer) return closure.getAt(0, 0);

//< Classes early-return-this
      return returnValue.value;
    }
//< catch-return
//> Classes return-this

    if (isInitializer) return closure.getAt(0, 0);
//< Classes return-this
    return null;
  }
//< function-call
}

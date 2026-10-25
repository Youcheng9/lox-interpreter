package com.craftinginterpreters.lox;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

public class InterpolationScannerDemo {
  public static void main(String[] args) throws IOException {
    String source;
    if (args.length == 0) {
      source = "\"Nested ${\"interpolation?! Are you ${\"mad?!\"}\"}\"";
    } else if (args.length == 1 && args[0].equals("--example1")) {
      source = "\"${drink} will be ready in ${steep + cool} minutes.\"";
    } else if (args.length == 1 && Files.exists(Path.of(args[0]))) {
      source = Files.readString(Path.of(args[0]));
    } else {
      source = String.join(" ", args);
    }
    for (InterpolationToken token : new InterpolationScanner(source).scanTokens()) {
      System.out.println(token);
    }
  }
}

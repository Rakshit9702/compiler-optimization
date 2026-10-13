define i32 @fold_constants() {
entry:
  %sum = add i32 19, 23
  %answer = mul i32 %sum, 2
  ret i32 %answer
}

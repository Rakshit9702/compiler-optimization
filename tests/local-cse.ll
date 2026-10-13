define i32 @reuse_expression(i32 %x, i32 %y) {
entry:
  %first = add i32 %x, %y
  %second = add i32 %x, %y
  %result = mul i32 %second, 3
  ret i32 %result
}

define i32 @distinct_allocations() {
entry:
  %slot_a = alloca i32
  %slot_b = alloca i32
  %same_address = icmp eq ptr %slot_a, %slot_b
  %result = zext i1 %same_address to i32
  ret i32 %result
}

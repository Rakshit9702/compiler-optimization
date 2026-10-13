declare void @observe(i32)

define i32 @preserve_observable_operations(ptr %address, i32 %value) {
entry:
  %dead = add i32 %value, 99
  %volatile_value = load volatile i32, ptr %address
  call void @observe(i32 %value)
  store i32 %value, ptr %address
  ret i32 %value
}

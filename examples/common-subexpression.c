int common_subexpression_example(int left, int right) {
  int first = left * right;
  int second = left * right;
  return first + second;
}

int sum(int *nums, int n) {
  int total = 0;
  for (int i = 0; i < n; i++)
    total += nums[i];
  return total;
}

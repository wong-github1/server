--source include/maybe_split_stack.inc

if ($have_split_stack)
{
  --skip Not running with -fsplit-stack
}

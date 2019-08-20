# dynamic-b-tree-bit-vector

## Changes
### Smarter counter updating (SPSI)
- New version around lines 1598-1622
- Old version commented out around lines 1636-1664
- Updates counters during traversal to bottom when searching for to correct leaft to insert to, instead of blindly after insertion has been done
- Uses SSE2 instructions to speed counter updates up. AVX2 would do this even further, but limits usable hardware. Note: some compilers could have been doing this already, but unreliably.

### Bit shifting (packed vector)
- Changes packed vector shifts to generate masks for setting values in the middle of words instead of repeated set() calls, also improves wordwise processing a bit in similar fashion (shift_right())

### Buffers (packed vector)
- Adds various sizes of buffers for insertions to delay shifting operations (insert() and insert_proper()), also changes shifting to happen n at a time to improve performance

### "Branchless" binary search (SPSI)
- Changes array scan (find_child()) to use "branchless" binary search instead of linear search. Branchless in this context means compiling conditionals to conditional moves instead of jumps. Library binary search also beats linear with B over 128.

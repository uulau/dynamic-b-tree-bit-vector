# dynamic-b-tree-bit-vector

## Buffers
- 3: packed_vector8.hpp
- 2: packed_vector6.hpp
- 4: packed_vector7.hpp
- 1: packed_vector5.hpp
- 0: packed_vector4.hpp
- Vanilla DYNAMIC (bad): packed_vector2.hpp
- AVX popcounts (bad): packed_vector3.hpp
- Backwards buffering of 4 items with bitfields (bad): packed_vector.hpp

## Changes
### packed_vector 4-8
- Changes packed vector shifts to generate masks for setting values in the middle of words instead of repeated set() calls, also improves wordwise processing a bit in similar fashion (shift_right())
### packed-vector 5-8
- Adds various sizes of buffers for insertions to delay shifting operations, also changes shifting to happen n at a time to improve performance
### spsi
- Updates array counters in nodes during traversal for insertions top-to-bottom, and not afterwards bottom-to-top
- Uses SIMD instructions (SSE2) for array updating to improve performance. Note: AVX2 and/or AVX512 would improve performance further. Trouble is using 64 bit integers, of which only 2 fit in SSE2 registers at one time. Another note: current code has plenty of loops unfriendly to compile auto-vectorization. E.g. setting for-loop limit variable to const helps in many cases.
- Changes array scan (find_child()) to use "branchless" binary search instead of linear search. Branchless in this context means compiling conditionals to conditional moves instead of jumps. Library binary search also beats linear with B over 128.

# STL Extras

File              | Use
------------------|------------------------------------------
vector_map.h      | map-style container based off std::vector
vector_set.h      | set-style container based off std::vector

The standard STL map and set classes perform a separate allocation for every
element, and consequently have poor memory coherency, and particularly read
performance. One solution to this issue is to use EASTL rather than the vendor
STL (https://github.com/electronicarts/EASTL), as it provides vector_set and
vector_map, which store all keys and values in a sorted linear array, and use
binary search for item lookup. This has the advantage that iteration over all
elements is coherent, and there are O(log(n)) allocations, or if you reserve()
accurately up front, one. (Bearing in mind that particularly in a multithreaded
environment heap allocation is often much more expensive than people think.)
EASTL additionally has other useful extras such as fixed_vector, intrusive
variants of STL containers, and readable source code.

If you have to use a vendor STL however, this repo provides vector_set and
vector_map equivalents based on top of std::vector<>. Use them the same way as
map/set, but be aware that insertion and deletion invalidates existing
iterators.

If your values are large, it may well be worth modifying vector_map to store the
keys in a separate array, or instead store indices into a larger array.

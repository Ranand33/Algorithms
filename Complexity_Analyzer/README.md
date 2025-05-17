Basic algorithm complexity analyzer

Limitation:
- performs a basic static analysis without full parsing.
- uses simple pattern matching to identify functions, loops, and memory allocations, which may not always be accurate.
- can't detect, algorithmic complexity nuances (e.g., divide and conquer algorithms), loop bounds that depend on input sizes, or amortized complexity considerations.
- doesn't account for function call chains, library function complexities, or constant factors that might be significant in practice.

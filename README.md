# Summary of Recursive Patterns for the Report

## 1. Name Permutation (`namePermutation`)

This module uses a technique called **Backtracking** to generate every possible arrangement of characters in a name.

### How it works:
- **Fixing and Swapping:** The function iterates through the string, swapping the character at the current index `l` with every character from `l` to `r`.
- **The Recursive Step:** After a swap, it calls itself for the next position (`l + 1`).
- **Backtracking:** It swaps the characters back to their original positions after the recursive call.
- **Base Case:** When `l == r`, a complete permutation is printed.

### How to use it:
```c
namePermutation("Ali"); // Prints: Ali, Ail, Lai, Lia, Ila, Ial
```

---

## 2. Event Subsequences (`subseqName`)

Unlike permutations, subsequences do not use all letters.

### How it works:
- **Inclusion/Exclusion Principle:**
  - Exclude current character
  - Include current character
- **Base Case:** When the end of the word is reached (`\0`), the subsequence is printed.

### How to use it:
```c
subseqName("8May"); // Prints: 8, M, a, y, 8M, 8Ma, 8May...
```

---

## 3. Chronological Overlap (`longestSubyear`)

Filters historical events based on a time range.

### How it works:
- **Date Extraction:** Uses `strrchr`
- **Recursive Scanning:** Reads file line by line
- **Base Case:** Stops at end of file

### How to use it:
```c
longestSubyear("1954", "1962");
```

---

## 4. Distinct Subsequences (`distinctSubseqWord`)

Avoids counting duplicate subsequences.

### How it works:
- Uses recursion with `lastPos`
- Subtracts duplicates

### How to use it:
```c
int count = distinctSubseqWord("BBA"); // Returns 5
```

---

## 5. Personality/Event Correlation (`isPalindromeWord`)

Checks if an event happened during a personality's lifetime.

### How it works:
- **Recursive Search:** Scans file
- **Lifespan Check:** Compares years
- **Base Case:** Returns true/false

### How to use it:
```c
if(isPalindromeWord("1956")) {
    printf("Event coincides with a historical figure's life.");
}
```

---

## Summary Table

| Function | Logic Pattern | Purpose in Project |
|----------|-------------|--------------------|
| `namePermutation` | Backtracking | Name variety analysis |
| `subseqName` | Inclusion/Exclusion | Pattern matching for events |
| `longestSubyear` | File Traversal | Chronological filtering |
| `distinctSubseqWord` | Dynamic Recursion | Data deduplication |
| `isPalindromeWord` | Cross-referencing | Historical correlation |

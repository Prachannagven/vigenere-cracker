A vigenere cipher cracker based on the method of Kasiski.

Uses pattern analysis followed by frequency analysis to find three most likeley keys, before running frequency analysis on the decrypted text with the keys to identify which is the most likeley to be the actual key.

Compile by:
```
make
```

You can optionally compile it in debug mode (with a bunch of extra print statements) using
```
make DEBUG_MODE_RUN=1
```


## TODO
- Modularize and sanitify so that it's not just monolithic code
- Add better funciton naming
- Seperate key cracking from key finalization

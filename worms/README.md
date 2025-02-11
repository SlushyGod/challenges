# worms

Challenge based off of finger protocol and named after Morris Worm.
This challenge adds in memory protections in each step without changing the core binary.

- worm-1 - no memory protections (other than ASLR)
- worm-2 - stack canary
- worm-3 - stack canary, NX
- worm-4 - stack canary, NX, PIE

Build challenges

```
make
```

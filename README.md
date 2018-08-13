# mscproject
Run-length encoding on GPUs for weather simulations using OpenCL.

## State of play

### TO-DO list
TO-DOs as of 13/8/18.

Monday/Tuesday
```
1. Implement 24 bit compression -> take value before dp, cast to void pointer, cast to 4 chars (8*4->32) and use the relevant chars
2. Tidy up existing scripts and my analysis results
3. Create some test data sets and look at C testing -> AceUnit
4. Send update email including structure limits
```

Wednesday
```
5. Implement tests for existing code
6. Implement variable <=24bit compression
```

Thursday
```
7. Gather papers
8. Fix transformation code
9. Start thinking about report
```

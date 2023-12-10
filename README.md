Simple VM following https://www.youtube.com/watch?v=zAsn8q-X9HM&list=PLP29wDx6QmW5DdwpdwHCRJsEubS5NrQ9b

...
0x09: call 0x2B                <- ip
0x0A:
0x0B:
...
0x2B:
_________________
...
|               |
|               | <- fp    <|
|               |           |
|               |           | n
|               |           |
|               |           |
...                        <|
|arg1           |
|arg2           |
|arg3           |
|argc=3         |
|               | <- sp
|               |
|               |
|               |
|               |
|               |
|               |
|               |
|               |

call
_________________
...
|               |
|               | <- old_fp
|               |
|               |
|               |
|               |
...
|arg1           |
|arg2           |
|arg3           |
|argc=3         |
|r1             |
|r2             |
|r3             |
|r4             |
|r5             |
|r6             |
|r7             |
|r8             |
|ip             |
|(14 + n)*2     |
|               | <- sp, fp
|               |

ret
_________________
...
|               |
|               | <- old_fp
|               |
|               |
|               |
|               |
...
|arg1           |  <- sp_restore_popped
|arg2           |
|arg3           |
|argc=3         |
|r1             |
|r2             |
|r3             |
|r4             |
|r5             |
|r6             |
|r7             |
|r8             |
|ip             |
|(14 + n)*2     |
|               | <- fp   <| <- sp_restore
...                        |
|               |          |
|               |          | /* move sp back to fp to remove local variables */
|               |          |
|               |          |
|               |          |
|               | <- sp   -
...


.define t = -5
MAIN: mov r3, LIST
LOOP: jmp L1
 prn #t
 mov STR, STR
 sub r1, r4
 cmp r3, #2
 bne END
L1: inc K
 bne LOOP
END: hlt
STR: .string "abcdef"
LIST: .data 6, -9, 4
K: .data 22
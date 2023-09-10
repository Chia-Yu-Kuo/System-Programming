.data
test1: .word 0, 5
test2: .word 5, 0
test3: .word 1, 9
test4: .word 9, 1
test5: .word 3, 8
test6: .word 10, 2
test7: .word -1, 2
test8: .word -1, 3
test9: .word -3, 0
test10: .word -5, 5

.text
    li    ra, -1
main:

    # s0 = test1
    # t0 = n   
    # t1 = result

       
    # Callee saved
    addi  sp, sp, -4            # sp = @sp - 4
    sw    s0, 0(sp)             # @s0 -> MEM[@sp-4]
    #

    # test value 
    la      s0, test1
    lw      s1, 0(s0)   # base
    lw      s2, 4(s0)   # exponenet

    blt     s2, x0, returnError # 防呆 exponent<0
    beq     s1, x0, setAnsZero
    
   # Caller saved
    addi    sp, sp  ,-4
    sw      ra, 0(sp)

    mv      a0, s1  # base
    mv      a1, s2  # exponenet
    li      t1, 1           # 確保0次方=1
    jal     ra, func_power

    mv      t1, a0

    lw      ra, 0(sp)
    lw      s0, 4(sp)
    addi    sp, sp, 8



    ret

returnError:
    li      t1, -1
    li      ra, -1
    ret

setAnsZero:
    li      t1, 0
    li      ra, -1
    ret

func_power:
    # t0 = n
    
    mv      t0, a1  # t0=exponent
    mv      t2, a0  # t2=base
    beq     t0, x0, mulPower
    # t1 = n-1
    addi     t1, t0, -1

mulRecur:
    # Caller saved
    addi    sp, sp, -8
    sw      ra, 4(sp)
    sw      t0, 0(sp)

    mv      a1, t1

    jal     ra, func_power

    mv      t1, a0          # t1 =f(n-1)

    # Retrieve Caller saved
    lw      t0, 0(sp)
    # Retrieve ra & Callee saved
    lw      ra, 4(sp)
    addi    sp, sp, 8

mulPower:

    beq     t0, x0, set_zero        # expon=0 or!=0

    addi    sp, sp, -16
    sw      ra, 8(sp)
    sw      t0, 4(sp)
    sw      t1, 0(sp)
    sw      t2, 12(sp)

    mv      a0, t2          # t2=base
    mv      a1, t1          # if t0=1 ==>t1==1   
    jal     ra, func_mul


    lw      ra, 8(sp)
    lw      t0, 4(sp)
    lw      t1, 0(sp)
    lw      t2, 12(sp)


    addi    sp, sp, 16
   
    ret

set_zero:
    li      a0, 1
    ret





func_two_sort:
    ####
    # a0 : Addr(array)
    ####
    
    lw   t0, 0(a0)
    lw   t1, 4(a0)
    
    bgeu t1, t0, endSwap   # if t0 <= t1 => Does not need to swap
swap:
    sw   t1, 0(a0)
    sw   t0  4(a0)
endSwap:
    ret                    # pc = ra
    
    


func_mul:
    ####
    # a0 : Multiplicand / return value
    # a1 : Multiplier
    # s0 : result
    ####
    
    # Callee saved
    addi  sp, sp, -4    # sp = @sp - 4
    sw    s0, 0(sp)     # @s0 -> MEM[@sp-4]
    
    # Set result = 0
    li    s0, 0         # result = 0

    # t0 = abs(Multiplicand)
    srai  t3, a0, 31
    xor   t0, a0, t3
    sub   t0, t0, t3    # t0 = abs(Multiplicand)
    
    # t1 = abs(Multplier)
    srai  t4, a1, 31
    xor   t1, a1, t4
    sub   t1, t1, t4    # t1 = abs(Multiplier)
    
    # t2 = (is_result_positive) ? 0 : -1
    xor   t2, t3, t4    # t2 = (is_result_positive) ? 0 : -1
    
    ## sort t0, t1
    # Caller saved
    addi  sp, sp, -16          # sp = @sp - 20
    sw    ra, 12(sp)           # ra -> MEM[@sp-8]
    sw    t2, 8(sp)            # t2 -> MEM[@sp-12]
    sw    t1, 4(sp)            # t1 -> MEM[@sp-16]
    sw    t0, 0(sp)            # t0 -> MEM[@sp-20]
    
    # Pass the parameters
    addi  a0, sp, 0            # a0 = Addr({t0, t1})
    
    # Jump to func_two_sort
    jal   ra, func_two_sort    # ra = Addr(lw  t0, 0(sp))
    ###
    
    # t0 < t1
    lw    t0, 0(sp)            # t0 = min(abs(Multiplicand), abs(Multiplier))
    lw    t1, 4(sp)            # t1 = max(abs(Multiplicand), abs(Multiplier))
    lw    t2, 8(sp)            # t2 = t2 = (is_result_positive) ? 0 : -1 
    
    # Consecutive addition to implement multiplication
    li    t3, 0                # i = 0
    bgeu  t3, t0, endMulLoop   # if i >= min(abs(Multiplicand), abs(Multiplier))
                               # => Go to endMulLooop
mulLoop:
    add   s0, s0, t1           # result += max(abs(Multiplicand), abs(Multiplier))
    addi  t3, t3, 1            # i++
    bltu  t3, t0, mulLoop      # if i < min(abs(Multiplicand), abs(Multiplier))
                               # => Go to mulLooop
endMulLoop:
    # s0 is abs(Multiplicand * Multiplier) now
    # According t2 to keep s0 positive or turn s0 to negative
    xor   s0, s0, t2
    sub   s0, s0, t2    # s0 = Multiplicand * Multiplier
    
    # Store return value in a0
    mv    a0, s0        # a0 = Multiplicand * Multiplier  
    
    # Retrieve ra & Callee saved
    lw    ra, 12(sp)    # ra = @ra
    lw    s0, 16(sp)    # s0 = @s0
    addi  sp, sp, 20    # sp = @sp
    
    ret                 # pc = ra

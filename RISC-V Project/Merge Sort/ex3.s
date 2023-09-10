.data

test1: .word 16,2,4,16,4,10,12,2,14,8,4,14,6,4,2,10,12,6,10,2,14,14,6,8,16,8,16,6,12,10,8,123
test2: .word 470,405,225,197,126,122,56,33,-81,-275,-379,-409,-416,-496,-500
test3: .word 412,-474,443,171,-23,247,221,7,40,221,-90,61,-9,49,-80,-80,221,-379,-161,-397,-173,276,-197,221,-12,-145,101
TEST1_SIZE: .word 32
TEST2_SIZE: .word 15
TEST3_SIZE: .word 27

.text
    li    ra, -1
main:

    # s0=test_label
    # s1=test_size

    # callee save
    addi    sp, sp, -8
    sw      s0, 4(sp)
    sw      s1, 0(sp)

    # ###### test1 ###########################

    la      s0, test1
    lw      s1, TEST1_SIZE

    # caller save
    addi    sp, sp, -4
    sw      ra, 0(sp)

    # 傳參數
    # a0 label
    # a1 x0
    # a2  size-1
    mv      a0, s0
    mv      a1, x0
    addi    a2, s1, -1

    jal     ra, mergesort

    # caller retrieve
    lw      ra,0(sp)
    addi    sp, sp, 4
    # ###### test2 ###########################

    la      s0, test2
    lw      s1, TEST2_SIZE


    # caller save
    addi    sp, sp, -4
    sw      ra, 0(sp)

    mv      a0, s0
    mv      a1, x0
    addi    a2, s1, -1

    jal     ra, mergesort

    # caller retrieve
    lw      ra,0(sp)
    addi    sp, sp, 4

    # ###### test3 ###########################

    la      s0, test3
    lw      s1, TEST3_SIZE


    # caller save
    addi    sp, sp, -4
    sw      ra, 0(sp)

    mv      a0, s0
    mv      a1, x0
    addi    a2, s1, -1

    jal     ra, mergesort

    # caller retrieve
    lw      ra,0(sp)
    addi    sp, sp, 4

    # ################# End ##############
    lw      s0, 4(sp)
    lw      s1, 0(sp)
    addi    sp, sp, 8


    ret


# divide
mergesort:
    mv  t0, a0  # *arr
    mv  t1, a1  #  start
    mv  t2, a2  #  end

    bge t1, t2, end_mergesort
    # start<end

    # t3    mid
    add     t3, t1, t2
    srli    t3, t3, 1

# ########## left side###########
    # caller save
    addi    sp, sp, -16
    sw      t1, 12(sp)
    sw      t2, 8(sp)
    sw      t3, 4(sp)
    sw      ra, 0(sp)
 

    # pass parameter
    mv  a0, t0
    mv  a1, t1
    mv  a2, t3
    jal ra, mergesort

    # retrieve caller save
    lw  t1, 12(sp)
    lw  t2, 8(sp)
    lw  t3, 4(sp)
    lw  ra, 0(sp)
    addi    sp, sp, 16    


# ########## right side###########
    # caller save
    addi    sp, sp, -16
    sw      t1, 12(sp)
    sw      t2, 8(sp)
    sw      t3, 4(sp)
    sw      ra, 0(sp)
 

    # pass parameter
    mv      a0, t0
    addi    t3, t3, 1
    mv      a1, t3
    mv      a2, t2
    jal     ra, mergesort

    # retrieve caller save
    lw  t1, 12(sp)
    lw  t2, 8(sp)
    lw  t3, 4(sp)
    lw  ra, 0(sp)
    addi    sp, sp, 16    

#  conquer 
merge:
    # t0:*arr
    # t1:start
    # t2:end
    # t3:mid
    # t4:i
    #  ####### need callee save######
    # s0:temp_size
    # s1:left_index     (used as temp reg)  
    # s2:right_index    (used as temp reg)  
    # s3:left_max    
    # s4:right_max
    # s5:arr_index      (used as temp reg)
    # s6:temp[0]address  
    # s7:array size (byte)for sp
    # s8:element address in *arr    /in *temp
    # s9:the value of element address in *arr   /in *temp


    # callee saved
    addi    sp, sp, -44
    sw      s0, 40(sp)
    sw      s1, 36(sp)
    sw      s2, 32(sp)
    sw      s3, 28(sp)
    sw      s4, 24(sp)
    sw      s5, 20(sp)
    sw      s6, 16(sp)
    sw      s7, 12(sp)
    sw      s8, 8(sp)
    sw      s9, 4(sp)
    sw      s10, 0(sp)




    # #assign value
    sub     s0, t2, t1  #temp_size
    addi    s0, s0, 1
    mv      t4, x0      #i
    mv      s1, x0      #left_index
    sub     s2, t3, t1  # s2:right_index
    addi    s2, s2, 1
    sub     s3, t3, t1  #left_max 
    sub     s4, t2, t1  # s4:right_max
    mv      s5, t1      # s5:arr_index

    # build temp arr     
    slli    s7, s0, 2       # temp_size*4
    sub     sp, sp, s7
    mv      s6, sp          # temp[0]address  

   

    # set temp array value
    bge     t4, s0, endforloop

    forloop:        # lw from arr
    # t5算index位置(byte)
    # s8(實際位置)
    # s9(value)
    add     t5, t1, t4  # index of i+start
    slli    t5, t5, 2   # element index*4
    add     s8, t0, t5  # element address(+arr[0])
    lw      s9, 0(s8)
    # store to temp
    slli    t5, t4, 2   # t4:i
    add     s8, s6, t5  # s6:temp[0]
    sw      s9, 0(s8)

    addi    t4, t4, 1
    blt     t4, s0, forloop
    endforloop:


    # ########## while ############    
 
    while1loop:         #   while(left_index <= left_max && right_index <= right_max)
    bgt     s1, s3, while3loop  
    bgt     s2, s4, while2loop


    # temp[left_index]value
    slli    t5, s1, 2       # element index*4
    add     s8, s6, t5  # element address(+temp[0])
    lw      s9, 0(s8)
    # temp[right_index]value
    slli    t5, s2, 2
    add     s8, s6, t5
    lw      s10, 0(s8)
    # compare temp[left_index]  (s9) temp[right_index]  (s10)
    bgt     s9, s10,    while1loop_else
    # temp[left_index] <= temp[right_index]
    while1loop_if:
    # sw  arr[arr_index]
    slli    t5, s5, 2
    add     s8, t0, t5
    sw      s9, 0(s8)
    # ++
    addi    s5, s5, 1
    addi    s1, s1, 1

    jal     x0, while1loop
 

    while1loop_else:  # temp[left_index] > temp[right_index]
    # sw arr[arr_index]
    slli    t5, s5, 2
    add     s8, t0, t5
    sw      s10, 0(s8)
    # ++
    addi    s5, s5, 1
    addi    s2, s2, 1

    jal     x0, while1loop




    while2loop:     # right_index>right_max 補左邊剩的  
    bgt     s1, s3, endwhile
     # lw temp[left_index]
    slli    t5, s1, 2       # element index*4
    add     s8, s6, t5  # element address(+temp[0])
    lw      s9, 0(s8)
    # sw  arr[arr_index]
    slli    t5, s5, 2
    add     s8, t0, t5
    sw      s9, 0(s8)
    # ++
    addi    s5, s5, 1
    addi    s1, s1, 1

    jal     x0, while2loop


    while3loop:     # left_index>left_max   補齊右邊剩的 
    bgt     s2, s4, endwhile
     # lw temp[right_index]
    slli    t5, s2, 2
    add     s8, s6, t5
    lw      s9, 0(s8)
    # sw arr[arr_index]
    slli    t5, s5, 2
    add     s8, t0, t5
    sw      s9, 0(s8)
    # ++
    addi    s5, s5, 1
    addi    s2, s2, 1

    jal     x0, while3loop


    endwhile:
    # retrieve temp arr
    add     sp, sp, s7      # s7:array size (byte)for sp
    # retrieve callee save       
    lw      s0, 40(sp)
    lw      s1, 36(sp)
    lw      s2, 32(sp)
    lw      s3, 28(sp)
    lw      s4, 24(sp)
    lw      s5, 20(sp)
    lw      s6, 16(sp)
    lw      s7, 12(sp)
    lw      s8, 8(sp)
    lw      s9, 4(sp)
    lw      s10, 0(sp)
    addi    sp, sp, 44




end_mergesort:

    ret



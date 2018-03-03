CC = gcc
CFLAGS = -O3
# GC_MODE = MARK_N_SWEEP
# GC_MODE = REF_COUNT

#mark and sweep gc edition
vm_mns.out: source/vm.c 
	$(CC) $(CFLAGS) -DMARK_N_SWEEP $? -o out/$@

vm_mns_dstack.out: source/vm.c 
	$(CC) $(CFLAGS) -DMARK_N_SWEEP -DDYNAMIC_STACK $? -o out/$@

#reference counter gc edition
vm_rc.out: source/vm.c 
	$(CC) $(CFLAGS) -DREF_COUNT $? -o out/$@

vm_rc_dstack.out: source/vm.c 
	$(CC) $(CFLAGS) -DREF_COUNT -DDYNAMIC_STACK $? -o out/$@

vm.out: source/vm.c 
	$(CC) $(CFLAGS) $? -o out/$@

clear:
	rm -rf out/*


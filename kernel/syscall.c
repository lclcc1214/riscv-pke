/*
 * contains the implementation of all syscalls.
 */

#include <stdint.h>
#include <errno.h>

#include "util/types.h"
#include "syscall.h"
#include "string.h"
#include "process.h"
#include "util/functions.h"

#include "elf.h"

#include "spike_interface/spike_utils.h"

//add @lab1_challenge1
extern elf_ctx elfloader;

//
// implement the SYS_user_print syscall
//
ssize_t sys_user_print(const char* buf, size_t n) {
  sprint(buf);
  return 0;
}

//
// implement the SYS_user_exit syscall
//
ssize_t sys_user_exit(uint64 code) {
  sprint("User exit with code:%d.\n", code);
  // in lab1, PKE considers only one app (one process). 
  // therefore, shutdown the system when the app calls exit()
  shutdown(code);
}

//
// add @lab1_challenge1
//
int backtrace_symbol(uint64 ra){
  uint64 func_va = 0;
  int idx = -1;
  for(int i = 0; i < elfloader.syms_num; i++){
    if(elfloader.symtab[i].st_info == STT_FUNC 
       && elfloader.symtab[i].st_value < ra
       && elfloader.symtab[i].st_value > func_va){
      idx = i;
      func_va = elfloader.symtab[i].st_value;
    }
  }
  return idx;
}




//
// add @lab1_challenge1 implement the SYS_user_backtrace syscall
//
ssize_t sys_user_backtrace(uint64 depth) {
  //uint64* fp = (uint64 *)current->trapframe->regs.s0;
  //sprint("s0=%lx, sp=%lx",current->trapframe->regs.s0,current->trapframe->regs.sp);

  //16表示叶函数(即print_backtrace)调用堆栈帧的长度，其中没有返回地址可用，
  //8表示前一个函数调用堆栈帧的空白空间的长度。
  //uint64 user_sp = current->trapframe->regs.sp + 16 + 8;
  uint64 user_sp = current->trapframe->regs.sp + 32 + 8;
  uint64 p = user_sp;
  for(int i = 0; i < depth; i++, p += 16){
    if(*(uint64 *)p == 0)
      break;
    //get the index in symtab accroding to the return address 
    int symtab_idx = backtrace_symbol(*(uint64*)p);
    //sprint("%lx\n",*(uint64*)p);
    if(symtab_idx == -1){
      sprint("fail to backtrace symbol %lx\n", *(uint64 *)p);
      continue;
    }
    sprint("%s\n", &elfloader.strtab[elfloader.symtab[symtab_idx].st_name]);
  } 
  return 0;
}

//
// [a0]: the syscall number; [a1] ... [a7]: arguments to the syscalls.
// returns the code of success, (e.g., 0 means success, fail for otherwise)
//
long do_syscall(long a0, long a1, long a2, long a3, long a4, long a5, long a6, long a7) {
  switch (a0) {
    case SYS_user_print:
      return sys_user_print((const char*)a1, a2);
    case SYS_user_exit:
      return sys_user_exit(a1);
    case SYS_user_backtrace:
      return sys_user_backtrace(a1);
    default:
      panic("Unknown syscall %ld \n", a0);
  }
}

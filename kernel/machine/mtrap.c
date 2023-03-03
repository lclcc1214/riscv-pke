#include "kernel/riscv.h"
#include "kernel/process.h"
#include "spike_interface/spike_utils.h"

// add @lab1_challlenge2
#define max_code_size 1024

static void handle_instruction_access_fault() { panic("Instruction access fault!"); }

static void handle_load_access_fault() { panic("Load access fault!"); }

static void handle_store_access_fault() { panic("Store/AMO access fault!"); }

static void handle_illegal_instruction() { panic("Illegal instruction!"); }

static void handle_misaligned_load() { panic("Misaligned Load!"); }

static void handle_misaligned_store() { panic("Misaligned AMO!"); }

// added @lab1_3
static void handle_timer() {
  int cpuid = 0;
  // setup the timer fired at next time (TIMER_INTERVAL from now)
  *(uint64*)CLINT_MTIMECMP(cpuid) = *(uint64*)CLINT_MTIMECMP(cpuid) + TIMER_INTERVAL;

  // setup a soft interrupt in sip (S-mode Interrupt Pending) to be handled in S-mode
  write_csr(sip, SIP_SSIP);
}

//
// add @lab1_challenge2
//
void illegal_line_print(addr_line *illegal_line){

  //目录长度
  //error 1
  int dir_len = strlen(current->dir[current->file[illegal_line->file].dir]);
  //文件长度
  int file_len = strlen(current->file[illegal_line->file].file);
  //总的路径长度
  int path_len = dir_len + file_len;
  
  char path[path_len + 1];

  //拷贝目录
  strcpy(path, current->dir[current->file[illegal_line->file].dir]);
  path[dir_len] = '/';
  //拷贝文件
  strcpy(path + dir_len + 1, current->file[illegal_line->file].file);
  //error 2
  //path[path_len] = '\0';

  //打开文件
  spike_file_t *file = spike_file_open(path, O_RDONLY, 0);

  //读取文件
  char code[max_code_size];
  spike_file_read(file, code, max_code_size);
  for(int i = 0, current_line = 1, line_head = 0; i<max_code_size; i++){
    if(code[i] == '\n'){
      //实际从第一行开始
      if(current_line == illegal_line->line){
        code[i] = '\0';
        //error 3
        //sprint("Runtime error at %s:%d\n%s\n", path, current_line, code + line_head);
        sprint("Runtime error at %s:%d\n%s\n", path, illegal_line->line, code + line_head);
        break;
      }
      else{
        current_line ++;
        line_head = i + 1;
      } 
    }
  }// end of for
}





//
// add @lab1_challenge2
//
void error_print(){
  //使用read_csr函数找到断点,断点的位置就是在mepc这个地方.
  uint64 mepc = read_csr(mepc);
  //找到断点这条指令对应的addr_line结构体
  for(int i = 0;i < current->line_ind; i++){
    if(mepc < current->line[i].addr){
      illegal_line_print(current->line + i - 1);
      break;
    }
  }

}


//
// handle_mtrap calls a handling function according to the type of a machine mode interrupt (trap).
//
void handle_mtrap() {
  uint64 mcause = read_csr(mcause);
  switch (mcause) {
    case CAUSE_MTIMER:
      handle_timer();
      break;
    case CAUSE_FETCH_ACCESS:
      // add @lab1_challenge2
      error_print();
      handle_instruction_access_fault();
      break;
    case CAUSE_LOAD_ACCESS:
      // add @lab1_challenge2
      error_print();
      handle_load_access_fault();
    case CAUSE_STORE_ACCESS:
      // add @lab1_challenge2
      error_print();
      handle_store_access_fault();
      break;
    case CAUSE_ILLEGAL_INSTRUCTION:
      // add @lab1_challenge2
      error_print();

      // TODO (lab1_2): call handle_illegal_instruction to implement illegal instruction
      // interception, and finish lab1_2.
      //panic( "call handle_illegal_instruction to accomplish illegal instruction interception for lab1_2.\n" );
      handle_illegal_instruction();
      break;
    case CAUSE_MISALIGNED_LOAD:
      // add @lab1_challenge2
      error_print();
      handle_misaligned_load();
      break;
    case CAUSE_MISALIGNED_STORE:
      // add @lab1_challenge2
      error_print();
      handle_misaligned_store();
      break;

    default:
      sprint("machine trap(): unexpected mscause %p\n", mcause);
      sprint("            mepc=%p mtval=%p\n", read_csr(mepc), read_csr(mtval));
      panic( "unexpected exception happened in M-mode.\n" );
      break;
  }
}

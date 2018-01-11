#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <malloc.h>

#define debug_print(fmt, ...) do { if (DEBUG) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)

#define STACK_GROWTH_NUM 131072 //2^17
#define CON_ADDR 1
#define INTGER 0
//Always check if (is_con_addr)
#ifndef REF_COUNT
    #ifndef MARK_N_SWEEP
        #define MARK_N_SWEEP
    #endif
#endif

#define NEXT_INSTRUCTION \
    goto *(label_tab[*pc])

//operation codes
    #define _halt   0x00
    #define _jump   0x01
    #define _jnz    0x02
    #define _dup    0x03
    #define _drop   0x04
    #define _push4  0x05
    #define _push2  0x06
    #define _push1  0x07
    #define _add    0x08
    #define _sub    0x09
    #define _mul    0x0a
    #define _div    0x0b
    #define _mod    0x0c
    #define _eq     0x0d
    #define _ne     0x0e
    #define _lt     0x0f
    #define _gt     0x10
    #define _le     0x11
    #define _ge     0x12
    #define _not    0x13
    #define _and    0x14
    #define _or     0x15
    #define _input  0x16
    #define _output 0x17
    #define _clock  0x2a
    #define _cons   0x2b
    #define _hd     0x2c
    #define _tl     0x2d
//end of operation codes

//operation sizes
    #define _halt_s   1
    #define _jump_s   3
    #define _jnz_s    3
    #define _dup_s    2
    #define _drop_s   1
    #define _push4_s  5
    #define _push2_s  3
    #define _push1_s  2
    #define _add_s    1
    #define _sub_s    1
    #define _mul_s    1
    #define _div_s    1
    #define _mod_s    1
    #define _eq_s     1
    #define _ne_s     1
    #define _lt_s     1
    #define _gt_s     1
    #define _le_s     1
    #define _ge_s     1
    #define _not_s    1
    #define _and_s    1
    #define _or_s     1
    #define _input_s  1
    #define _output_s 1
    #define _clock_s  1
    #define _cons_s   1
    #define _hd_s     1
    #define _tl_s     1
//end of operation sizes

struct con_cell;

struct stack_element{
    char is_con_addr; //=CON_ADDR or =INTGER
    union {
        struct con_cell *con_addr;
        int32_t integer;
    } value;
};

typedef struct stack_element stackelement_t;

struct con_cell{
    #ifdef REF_COUNT 
        int ref_num;
    #endif
    #ifdef MARK_N_SWEEP 
        char reachable;
        struct con_cell *next;
    #endif
    stackelement_t head;
    stackelement_t tail;
};

typedef struct con_cell concell_t;

struct {
    stackelement_t *stack;
    int size;
    int current;
} stack;

#ifdef MARK_N_SWEEP 
    #define REACHABLE 1
    #define UNREACHABLE 0
    #define GC_MALLOC_LIMIT 1000000

    long long int freed;
    long long int malloced;

    concell_t *gc_mns_list;

    void mark_sweep();
    void mark();
    void sweep();
    void traverse(stackelement_t el);
#endif

#ifdef REF_COUNT
    void ref_butcher(stackelement_t connie);//recursively substracts reference counters
#endif

void initialize_stack();
void project_stack_change(int change_number);
stackelement_t pop_stack();
void push_stack(stackelement_t newel);
// (un)signed from n bytes
uint32_t uf1b(char* mpos);
uint32_t uf2b(char* mpos);
int32_t sf1b(char* mpos);
int32_t sf2b(char* mpos);
int32_t sf4b(char* mpos);


int main(int argc, char* argv[]){

    clock_t start = clock();
    char program[65537];
    int i,psize;
    FILE *pfile;
    for(i=0;i<65537;i++){
        program[i] = 0;
    }

    if (argc < 2){
        printf("FATAL ERROR:You must specify an input file\n");
    }

    pfile = fopen(argv[1],"rb");
    if(pfile==NULL){
        printf("FATAL ERROR:Error while reading file\n");
        exit(-1);
    }
    psize = 0;
    while(!feof(pfile))
        psize+=fread(program,sizeof(char),65536,pfile);
    fclose(pfile);
    
    static char* commands[] = {	
        "halt",
        "jump",
        "jnz",
        "dup",
        "drop",
        "push4",
        "push2",
        "push1",
        "add",
        "sub",
        "mul",
        "div",
        "mod",
        "eq",
        "ne",
        "lt",
        "gt",
        "le",
        "ge",
        "not",
        "and",
        "or",
        "input",
        "output",
        "NULL",
        "NULL",
        "NULL",
        "NULL",
        "NULL",
        "NULL",
        "NULL",
        "NULL",
        "NULL",
        "NULL",
        "NULL",
        "NULL",
        "NULL",
        "NULL",
        "NULL",
        "NULL",
        "NULL",
        "NULL",
        "clock",
        "cons",
        "hd",
        "tld"
    };

    static int lens[] = {	
        1,
        3,
        3,
        2,
        1,
        5,
        3,
        2,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1
    };

    static void *label_tab[] = {	
        &&halt_label, // 00
        &&jump_label, // 01
        &&jnz_label,  // 02
        &&dup_label, // 03
        &&drop_label, // 04
        &&push4_label, // 05
        &&push2_label, // 06
        &&push1_label, // 07
        &&add_label, // 08
        &&sub_label, // 09
        &&mul_label, // 0a
        &&div_label, // 0b
        &&mod_label, // 0c
        &&eq_label, // 0d
        &&ne_label, // 0e
        &&lt_label, // 0f
        &&gt_label, // 10
        &&le_label, // 11
        &&ge_label, // 12
        &&not_label, // 13
        &&and_label, // 14
        &&or_label, // 15
        &&input_label, // 16
        &&output_label, // 17
        NULL, //18
        NULL, //19
        NULL, //1a
        NULL, //1b
        NULL, //1c
        NULL, //1d
        NULL, //1e
        NULL, //1f
        NULL, //20
        NULL, //21
        NULL, //22
        NULL, //23
        NULL, //24
        NULL, //25
        NULL, //26
        NULL, //27
        NULL, //28
        NULL, //29
        &&clock_label, // 2a
        &&cons_label, // 2b
        &&hd_label, // 2c
        &&tl_label // 2d
    };

    /////////////////////////
    // printf("Size of program: %d\nProgram:\n",psize);
    // i=0;
    // while(i<psize){
    //     printf("Line %#x:: %s :: ",i,commands[program[i]]);
    //     int j;
    //     if (program[i] == 1 || program[i] == 2){
    //         printf("(%#x) ",uf2b(&program[i+1]));
    //     }
    //     if (program[i] == 5) printf("(%#x) ",sf4b(&program[i+1]));
    //     if (program[i] == 6) printf("(%#x) ",sf2b(&program[i+1]));
    //     for(j=1;j<(lens[program[i]]);j++){
    //         printf("%#x ",program[i+1]);
    //     }
    //     printf("\n");
    //     i+=lens[program[i]];
    // }
    /////////////////////////

    #ifdef MARK_N_SWEEP
        freed = 0;
        malloced = 0;
        gc_mns_list = NULL;
    #endif

    char *pc = program;
    char x;
    stackelement_t elem,newel,el1,el2;
    concell_t *cc;
    initialize_stack();
    
    while(1){
        char opcode = pc[0];
        switch(opcode){
            ///////////////////

            case _halt: // HALT
            halt_label:
                // debug_print("--halt\n");
                exit(0);

            ///////////////////

            case _jump: // JUMP
            jump_label:
                // debug_print("--jump %d (%d %d)\n",uf2b(pc+1),uf1b(pc+1),uf1b(pc+2));
                pc = &program[uf2b(pc+1)];
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _jnz: // JNZ
            jnz_label:
                // debug_print("--jnz %d (%d %d)\n",uf2b(pc+1),uf1b(pc+1),uf1b(pc+2));
                elem=pop_stack();
                if (elem.value.integer!=0){
                    pc = &program[uf2b(pc+1)];
                }
                else{
                    pc+=_jnz_s;
                }
                NEXT_INSTRUCTION;

            ///////////////////

            case _dup: // DUP
            dup_label:
                // debug_print("--dup %d \n",uf1b(pc+1));
                newel = stack.stack[stack.current-uf1b(pc+1)];
                #ifdef REF_COUNT
                    if (newel.is_con_addr){
                        newel.value.con_addr->ref_num++;
                    }
                #endif
                push_stack(newel);
                pc+=_dup_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _drop: // DROP
            drop_label:
                // pop_stack();
                el1 = pop_stack();
                #ifdef REF_COUNT
                    if (el1.is_con_addr){
                        ref_butcher(el1);
                    }
                #endif
                pc+=_drop_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _push4: // PUSH 4
            push4_label:
                newel.is_con_addr = INTGER;
                newel.value.integer = sf4b(pc+1);
                push_stack(newel);
                pc+=_push4_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _push2: // PUSH 2
            push2_label:
                newel.is_con_addr = INTGER;
                newel.value.integer = sf2b(pc+1);
                push_stack(newel);
                pc+=_push2_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _push1: // PUSH 1
            push1_label:
                newel.is_con_addr = INTGER;
                newel.value.integer = sf1b(pc+1);
                push_stack(newel);
                pc+=_push1_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _add: // ADD
            add_label:
                newel = pop_stack();
                newel.value.integer += (pop_stack()).value.integer;
                push_stack(newel);
                pc+=_add_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _sub: // SUB //?????????????????
            sub_label:
                el1 = pop_stack();
                el2 = pop_stack();
                el2.value.integer -= el1.value.integer;
                push_stack(el2);
                pc+=_sub_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _mul: // MUL
            mul_label:
                newel = pop_stack();
                newel.value.integer *= (pop_stack()).value.integer;
                push_stack(newel);
                pc+=_mul_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _div: // DIV
            div_label:
                el1 = pop_stack();
                el2 = pop_stack();
                el2.value.integer /= el1.value.integer;
                push_stack(el2);
                pc+=_div_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _mod: // MOD
            mod_label:
                el1 = pop_stack();
                el2 = pop_stack();
                el2.value.integer %= el1.value.integer;
                push_stack(el2);
                pc+=_mod_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _eq: // EQ
            eq_label:
                newel.is_con_addr = INTGER;
                el1 = pop_stack();
                el2 = pop_stack();
                if (el1.value.integer == el2.value.integer){
                        newel.value.integer = 1;
                }
                else{
                    newel.value.integer = 0;
                }
                push_stack(newel);
                pc+=_eq_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _ne: // NE
            ne_label:
                newel.is_con_addr = INTGER;
                el1 = pop_stack();
                el2 = pop_stack();
                if (el1.value.integer == el2.value.integer){
                    newel.value.integer = 0;
                }
                else{
                    newel.value.integer = 1;
                }
                push_stack(newel);
                pc+=_ne_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _lt: // LT
            lt_label:
                newel.is_con_addr = INTGER;
                el1 = pop_stack();
                el2 = pop_stack();
                if (el2.value.integer < el1.value.integer){
                    newel.value.integer = 1;
                }
                else{
                    newel.value.integer = 0;
                }
                push_stack(newel);
                pc+=_lt_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _gt: // GT
            gt_label:
                newel.is_con_addr = INTGER;
                el1 = pop_stack();
                el2 = pop_stack();
                if (el2.value.integer > el1.value.integer){
                    newel.value.integer = 1;
                }
                else{
                    newel.value.integer = 0;
                }
                push_stack(newel);
                pc+=_gt_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _le: // LE
            le_label:
                newel.is_con_addr = INTGER;
                el1 = pop_stack();
                el2 = pop_stack();
                if (el2.value.integer <= el1.value.integer){
                    newel.value.integer = 1;
                }
                else{
                    newel.value.integer = 0;
                }
                push_stack(newel);
                pc+=_le_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _ge: // GE
            ge_label:
                newel.is_con_addr = INTGER;
                el1 = pop_stack();
                el2 = pop_stack();
                if (el2.value.integer >= el1.value.integer){
                    newel.value.integer = 1;
                }
                else{
                    newel.value.integer = 0;
                }
                push_stack(newel);
                pc+=_ge_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _not: // NOT
            not_label:
                newel = pop_stack();
                newel.is_con_addr = INTGER;
                if (newel.value.integer == 0){
                    newel.value.integer = 1;
                }
                else{
                    newel.value.integer = 0;
                }
                push_stack(newel);
                pc+=_not_s;
                NEXT_INSTRUCTION;


            ///////////////////
            
            case _and: // AND
            and_label:
                el1 = pop_stack();
                el2 = pop_stack();
                el1.is_con_addr = INTGER;
                if ((el1.value.integer!=0) && (el2.value.integer!=0)){
                    el1.value.integer = 1;
                }
                else{
                    el1.value.integer = 0;
                }
                push_stack(el1);
                pc+=_and_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _or: // OR
            or_label:
                el1 = pop_stack();
                el2 = pop_stack();
                el1.is_con_addr = INTGER;
                if ((el1.value.integer!=0) || (el2.value.integer!=0)){
                    el1.value.integer = 1;
                }
                else{
                    el1.value.integer = 0;
                }
                push_stack(el1);
                pc+=_or_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _input: // INPUT
            input_label:
                newel.is_con_addr = INTGER;
                newel.value.integer = getchar();
                push_stack(newel);
                pc+=_input_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _output: // OUTPUT
            output_label:
                el1 = pop_stack();
                putchar((char)el1.value.integer);
                pc+=_output_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _clock: //CLOCK
            clock_label:
                printf("%lf\n",(double)(clock()-start)/CLOCKS_PER_SEC);
                pc+=_clock_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _cons: //CONS
            cons_label:
                
                cc = (concell_t *)malloc(sizeof(concell_t));
                el1 = pop_stack();
                el2 = pop_stack();
                cc->head = el2;
                cc->tail = el1;

                #ifdef MARK_N_SWEEP
                    cc->reachable = UNREACHABLE;
                    malloced++;
                    cc->next = gc_mns_list;
                    gc_mns_list = cc;

                    if (malloced == GC_MALLOC_LIMIT){
                        mark_sweep();
                        malloced = 0;
                    }
                #endif
                #ifdef REF_COUNT
                    cc->ref_num = 1;
                    if (cc->head.is_con_addr){
                        cc->head.value.con_addr->ref_num++;
                    }
                    if (cc->tail.is_con_addr){
                        cc->tail.value.con_addr->ref_num++;
                    }
                #endif
                

                newel.is_con_addr = CON_ADDR;
                newel.value.con_addr = cc;
                push_stack(newel);
                pc+=_cons_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _hd: // HD
            hd_label:
                el1 = pop_stack();
                cc = el1.value.con_addr;
                newel = cc->head;

                #ifdef REF_COUNT
                    if (cc->head.is_con_addr){
                        cc->head.value.con_addr->ref_num++;
                    }

                    ref_butcher(el1);
                #endif

                push_stack(newel);
                pc+=_hd_s;
                NEXT_INSTRUCTION;

            ///////////////////
            
            case _tl: // TL
            tl_label:
                el1 = pop_stack();
                cc = el1.value.con_addr;
                newel = cc->tail;

                #ifdef REF_COUNT
                    if (cc->tail.is_con_addr){
                        cc->tail.value.con_addr->ref_num++;
                    }

                    ref_butcher(el1);
                #endif

                push_stack(newel);

                pc+=_hd_s;
                NEXT_INSTRUCTION;
        }
    }

}

#ifdef MARK_N_SWEEP
    void mark_sweep(){
        mark();
        sweep();
    }

    void sweep(){
        concell_t *curr,*next,*prev;
        curr = gc_mns_list;
        if (curr!=NULL){
            while(curr!=NULL && !curr->reachable){
                next = curr->next;
                free(curr);
                curr = next;
            }
            gc_mns_list = curr;
            if (curr!=NULL){
                prev = curr;
                curr = curr->next;
                while(curr!=NULL){
                    if (!curr->reachable){
                        prev->next = curr->next;
                        free(curr);
                        curr = prev;
                    }
                    curr = curr->next;
                }
            }
        }
    }

    void mark(){
        int i;
        for (i=0;i<=stack.current;i++){
            traverse(stack.stack[i]);
        }
    }

    void traverse(stackelement_t el){
        if (el.is_con_addr && !el.value.con_addr->reachable){
            el.value.con_addr->reachable = 1;
            traverse(el.value.con_addr->head);
            traverse(el.value.con_addr->tail);
        }
    }
#endif

#ifdef REF_COUNT
    void ref_butcher(stackelement_t connie){
        connie.value.con_addr->ref_num--;
        if (connie.value.con_addr->ref_num == 0){
            if (connie.value.con_addr->head.is_con_addr){
                ref_butcher(connie.value.con_addr->head);
            }
            if (connie.value.con_addr->tail.is_con_addr){
                ref_butcher(connie.value.con_addr->tail);
            }

            free(connie.value.con_addr);
            freed++;

        }
    }
#endif

inline uint32_t uf1b(char* mpos){
    return (uint32_t)( *( (uint8_t *)mpos ) );
}

inline uint32_t uf2b(char* mpos){
    return (uint32_t)( *( (uint16_t *)mpos ) );
}
inline int32_t sf1b(char* mpos){
    return (int32_t)( *( (int8_t *)mpos ) );
}
inline int32_t sf2b(char* mpos){
    return (int32_t)( *( (int16_t *)mpos ) );
}
inline int32_t sf4b(char* mpos){
    return *( (int32_t *)mpos );
}

inline void push_stack(stackelement_t newel){
    project_stack_change(1);
    stack.stack[stack.current] = newel;
}

inline stackelement_t pop_stack(){
    stackelement_t result = stack.stack[stack.current];
    project_stack_change(-1);
    return result;
}

inline void project_stack_change(int change_number){

    stack.current += change_number;

    if (stack.current< -1){
        //ERROR CODE
    }
    else if (stack.current >= stack.size){
        stack.size = stack.size+(STACK_GROWTH_NUM);
        stack.stack = realloc(stack.stack,stack.size*sizeof(stackelement_t));
    }
    else if ((stack.size > STACK_GROWTH_NUM) && ((stack.size / (stack.current + 1)) >= 3)){
        stack.size = (stack.size/2) + (STACK_GROWTH_NUM - ((stack.size/2) % STACK_GROWTH_NUM)) % STACK_GROWTH_NUM;
        stack.stack = realloc(stack.stack,stack.size*sizeof(stackelement_t));
    }
}

inline void initialize_stack(){
    stack.size = STACK_GROWTH_NUM;
    stack.current = -1;
    stack.stack = malloc(STACK_GROWTH_NUM*sizeof(stackelement_t));
}
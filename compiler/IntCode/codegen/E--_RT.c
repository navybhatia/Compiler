#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#define R0(x) x=0
#define R1(x) R0(x##0), R0(x##1), R0(x##2), R0(x##3), R0(x##4), R0(x##5), R0(x##6), R0(x##7), R0(x##8), R0(x##9) 
#define R2(x) R1(x##0), R1(x##1), R1(x##2), R1(x##3), R1(x##4), R1(x##5), R1(x##6), R1(x##7), R1(x##8), R1(x##9) 
#define R3(x) R2(x##0), R2(x##1), R2(x##2), R2(x##3), R2(x##4), R2(x##5), R2(x##6), R2(x##7), R2(x##8), R2(x##9) 

int R3(R);
float R3(F);
unsigned *mem;

#define ADD(x, y) x + y
#define SUB(x, y) x - y
#define DIV(x, y) x / y
#define MUL(x, y) x * y
#define MOD(x, y) x % y
#define AND(x, y) x & y
#define  OR(x, y) x | y
#define XOR(x, y) x ^ y

#define FADD(x, y) x + y
#define FSUB(x, y) x - y
#define FDIV(x, y) x / y
#define FMUL(x, y) x * y

#define NEG(x) (-x)
#define FNEG(x) (-x)

#define UGT(x, y) ((unsigned)x > (unsigned)y)
#define UGE(x, y) ((unsigned)x >= (unsigned)y)
#define GT(x, y) (x > y)
#define GE(x, y) (x >= y)
#define EQ(x, y) (x == y)
#define NE(x, y) (x != y)

#define FGT(x, y) (x > y)
#define FGE(x, y) (x >= y)
#define FEQ(x, y) (x == y)
#define FNE(x, y) (x != y)

#define PRTS(x) printf((char *)(x))
#define PRTI(x) printf("%d", (x))
#define PRTF(x) printf("%g", (x))

#define JMP(x) goto x
#define JMPC(y,x) if (y) goto x
#define JMPI(x) goto *(void*)(x)
#define JMPCI(y,x) if (y) goto *(void*)(x)

#define MOVL(l, r) r = (int)(&&l)
#define MOVS(s, r) r = (int)(s)
#define MOVI(i, r) r = i
#define MOVF(f, r) r = f

#define MOVIF(r, f) f = r
#define MIVFI(f, r) r = (int)f

#define STI(r, a) mem[a] = r
#define STF(f, a) mem[a] = *(int*)&f

#define LDI(i, r) r = mem[i]
#define LDF(i, f) f = *(float*)&mem[i]

#define IN(r) r=getc(stdin)
#define INI(r) r=ini();
#define INF(f) f=inf();
   

int regdump, fregdump;
int lowdmem=-1, hidmem=-1;
unsigned msize=0;
const unsigned defmem = (1<<20);
const unsigned maxm = (1<<31);
float protfrac = 0.1;

void usage(const char *cname) {
   fprintf(stderr, "%s [-dr] [-df] [-dm min max] [-m sz]\n", cname);
   fprintf(stderr, "\t-dr: Dump R000 to R009 at the end of execution\n");
   fprintf(stderr, "\t-df: Dump F000 to F009 at the end of execution\n");
   fprintf(stderr, "\t-dm min max: Dump memory locations min through max\n");
   fprintf(stderr, "\t-m sz: Set memory size to sz words (must be multiple of page size)\n");
   exit(1);
}

void rtexit(const char *s) {
   fprintf(stderr, "Runtime error: %s. Aborting execution.\n", s);
   exit(1);
}

void E_RT_init(int argc, char* argv[]) {
   int i;
   unsigned ps = getpagesize(), m1,m2;
   for (i=1; i < argc; i++) {
      if (argv[i][0] == '-') {
         switch (argv[i][1]) {
         case 'd':
            switch (argv[i][2]) {
            case 'r': regdump=1; break;
            case 'f': fregdump=1; break;
            case 'm': 
               if (i+2 < argc) {
                  lowdmem = atoi(argv[i+1]);
                  hidmem  = atoi(argv[i+2]);
                  i += 2;
                  if (lowdmem < 0 || hidmem < 0)
                     usage(argv[0]);
               }
               else usage(argv[0]);
               break;
            default: usage(argv[0]); break;
            }
            break;
         case 'm': 
            if (i+1 < argc) {
               msize = (unsigned)atoi(argv[i+1]); 
               if (msize == 0)
                  usage(argv[0]);
               i++;
            }
            else usage(argv[0]);
            break;

         default:
            usage(argv[0]); break;
         }
      }
      else usage(argv[0]);
   }

   // Allocate requested amount of memory, set up guard area around it
   // to catch (most) memory access violations

   if (msize <= 0)
      msize = defmem;
   if ((hidmem >= (int)msize) || (hidmem < lowdmem)) {
      fprintf(stderr, "Invalid range for memory dumping\n");
      usage(argv[0]);
   }

   m1 = msize*sizeof(int);
   m1 = (m1/ps)*ps;
   if (m1 != msize*sizeof(int)) {
      m1 += ps;
      fprintf(stderr, "Setting memory size to %ld words\n", m1/sizeof(int));
   }
   m2 = (1+(unsigned)((msize*sizeof(int)*protfrac))/ps)*ps;
   if (m1+2*m2 < msize) {
      fprintf(stderr, "Requested too much memory: integer overflow, exiting\n");
      exit(1);
   }
   if ((mem=mmap(0, m1+2*m2, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS,
                 0, 0))==MAP_FAILED){
      fprintf(stderr, "Unable to allocate %d bytes of memory\n", m1+2*m2);
      perror("");
      exit(1);
   }
   if ((mprotect(mem, m2, PROT_NONE) != 0) || 
       (mprotect(((char*)mem)+m1+m2, m2, PROT_NONE) != 0)) {
      fprintf(stderr, "Unable to protect memory, exiting\n");
      exit(1);
   }
   mem = mem+m2;
}

void E_RT_exit() {
   if (regdump) {
      printf("\nDump of Registers R000 to R009:\n");
      printf("R000=%d\tR001=%d\tR002=%d\tR003=%d\tR004=%d\n",
             R000, R001, R002, R003, R004);
      printf("R005=%d\tR006=%d\tR007=%d\tR008=%d\tR009=%d\n",
             R005, R006, R007, R008, R009);
   }
   if (fregdump) {
      printf("\nDump of Registers F000 to F009:\n");
      printf("F000=%f\tF001=%f\tF002=%f\tF003=%f\tF004=%f\n",
             F000, F001, F002, F003, F004);
      printf("F005=%f\tF006=%f\tF007=%f\tF008=%f\tF009=%f\n",
             F005, F006, F007, F008, F009);
   }
   if (lowdmem >= 0) {
      int i, j; int ln=8;
      printf("\nDump of Memory locations %d to %d:", lowdmem, hidmem);
      for (i=lowdmem; i <= hidmem; i += ln) {
         printf("\n%08x:", i);
         for (j=i; (j < i+ln) && (j <= hidmem); j++)
            printf("%11d", mem[j]);
      }
      printf("\n");
   }
}

inline int ini() {
   union {
      char c[4];
      int i;
   } z;
   int r1 = getc(stdin), r2 = getc(stdin), r3 = getc(stdin), r4 = getc(stdin);
   if (r4 < 0) rtexit("Input error");
   z.c[0] = r1; z.c[1] = r2; z.c[2] = r3; z.c[3] = r4;
   return z.i;
}
inline int inf() {
   union {
      char c[4];
      float f;
   } z;
   int r1 = getc(stdin), r2 = getc(stdin), r3 = getc(stdin), r4 = getc(stdin);
   if (r4 < 0) rtexit("Input error");
   z.c[0] = r1; z.c[1] = r2; z.c[2] = r3; z.c[3] = r4;
   return z.f;
}


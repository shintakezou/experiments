#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
 
#define MAXLOOP 10000

enum eltype
{
    TINTEGER = 0x00,
    TLONG = 0x01,
    TDOUBLE = 0x02,
    TARRAY = 0x80
};

struct el {
    enum eltype type;
    union {
        double d;
        int i;
        long l;
        void *p;
    } val;
};


struct el numbered_reg[20];

struct el opstack[1000];
int stackp = -1;



inline void pop(void)
{
    if (stackp >= 0) {
        stackp--;
    }
}

inline struct el *pick(int n)
{
    return &opstack[stackp - n >= 0 ? stackp - n : 0];
}

inline void dpush(double v)
{
    stackp++;
    opstack[stackp].type = TDOUBLE;
    opstack[stackp].val.d = v;
}

inline void sipush(int v)
{
    stackp++;
    opstack[stackp].type = TINTEGER;
    opstack[stackp].val.i = v;
}

inline void g_store(int n)
{
    memcpy(&numbered_reg[n], pick(0), sizeof (numbered_reg[0]));
    pop();
}

inline void g_load(int n)
{
    stackp++;
    memcpy(&opstack[stackp], &numbered_reg[n], sizeof (opstack[0]));
}


inline void astore2(void)
{
    g_store(2);
}

inline void istore1(void)
{
    g_store(1);
}

inline void iload1(void)
{
    g_load(1);
}

inline void iconst0(void)
{
    stackp++;
    opstack[stackp].type = TINTEGER;
    opstack[stackp].val.i = 0;
}

inline void iconst1(void)
{
    stackp++;
    opstack[stackp].type = TINTEGER;
    opstack[stackp].val.i = 1;
}


inline void dconst1(void)
{
    stackp++;
    opstack[stackp].type = TDOUBLE;
    opstack[stackp].val.d = 1.0;
}

inline void dstore(int n)
{
    g_store(n);
}

inline void istore(int n)
{
    g_store(n);
}

inline void iload(int n)
{
    g_load(n);
}

inline void dload(int n)
{
    g_load(n);
}

inline void aload(int n)
{
    g_load(n);
}

inline void aload2(void)
{
    g_load(2);
}


inline void i2d(void)
{
    int v = pick(0)->val.i;
    pick(0)->type = TDOUBLE;
    pick(0)->val.d = (double)v;
}

inline void dmul(void)
{
    double a = pick(1)->val.d;
    double b = pick(0)->val.d;
    pop();
    pick(0)->val.d = a * b;
}

inline void iinc(int n, int iv)
{
    numbered_reg[n].val.i += iv;
}

inline void dastore(void)
{
    double *ap = pick(2)->val.p;
    ap[pick(1)->val.i] = pick(0)->val.d;
    pop(); pop(); pop();
}



inline void newarray(enum eltype t)
{
    struct el *e = pick(0);
    int scale = 1;
    switch (t) {
    case TINTEGER:
        scale = sizeof (int);
        break;
    case TDOUBLE:
        scale = sizeof (double);
        break;
    case TLONG:
        scale = sizeof (long);
        break;
    default:
        break;
    }
    void *p = malloc(e->val.i * scale);
    e->type = TDOUBLE | TARRAY;
    e->val.p = p;
}


inline int if_icmpge(void)
{
    int a = pick(1)->val.i;
    int b = pick(0)->val.i;
    pop(); pop();
    return a >= b;
}

inline int if_icmpgt(void)
{
    int a = pick(1)->val.i;
    int b = pick(0)->val.i;
    pop(); pop();
    return a > b;
}



int main (void)
{
    double *nArray = NULL;

    sipush(MAXLOOP);
    newarray(TDOUBLE);
    astore2();
    
    iconst0();
    istore1();
  L54:
    iload1();
    sipush(MAXLOOP);
    if (if_icmpge()) goto L101;
    dconst1();
    dstore(7);
    iconst1();
    istore(9);
  L67:
    iload(9);
    iload1();
    if (if_icmpgt()) goto L87;
    dload(7);
    iload(9);
    i2d();
    dmul();
    dstore(7);
    iinc(9,1);
    goto L67;
  L87:
    aload2();
    iload1();
    iload1();
    i2d();
    dload(7);
    dmul();
    dastore();
    iinc(1, 1);
    goto L54;
    
  L101:
    nArray = numbered_reg[2].val.p;
    // ordinamento in maniera decrescente
    int i;
    for (i = 0; i < MAXLOOP; i++)
    {
        double dMax = nArray[i];
        int j;
        for (j = i + 1; j < MAXLOOP; j++)     
        {       
            if (nArray[j] > dMax)
            {
                double dTemp = nArray[j];
                nArray[j] = dMax;
                nArray[i] = dTemp;
                dMax = dTemp;
            }
        }
    }

    printf("%lf\n", nArray[0]);
    free(nArray);
    
    return 0;
}

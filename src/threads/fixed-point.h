#ifndef THREADS_FIXED_POINT_H
#define THREADS_FIXED_POINT_H

#include<stdbool.h>
#include <stdint.h>

typedef int fixed;
typedef int64_t fixed64_t;


int fixed_to_int(fixed, bool round);
fixed int_to_fixed(int num);


fixed add_fixed_fixed(fixed, fixed);
fixed sub_fixed_fixed(fixed, fixed);
fixed mul_fixed_fixed(fixed, fixed);
fixed div_fixed_fixed(fixed, fixed);

fixed add_fixed_int(fixed, int);
fixed sub_fixed_int(fixed, int);
fixed mul_fixed_int(fixed, int);
fixed div_fixed_int(fixed, int);



#endif // threads/fixed-point.h

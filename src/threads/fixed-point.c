#include <stdio.h>
#include <stdint.h>
#include "fixed-point.h"

#define P_BITS 17
#define Q_BITS 14
#define FIXED_LOC 1 << Q_BITS


int fixed_to_int(fixed f_num, bool round){
	int num;

	if(round){

		if(f_num >= 0){
			num = ( (f_num + (FIXED_LOC)) / 2) / (FIXED_LOC);
		}else {
			num = ( (f_num - (FIXED_LOC)) / 2) / (FIXED_LOC) ;
		}

	} else {
		num = f_num/(FIXED_LOC);	
	}
	
	return num;
}

/* This function assumes that you have already allocated and declared a
   FIXED struct with all member variables appropriately initialized, 
   thus f_num can't be NULL. Returns n * f where f is 2^q. */
fixed int_to_fixed(int num){
	fixed f_num = num * FIXED_LOC;
	return f_num;
}





fixed add_fixed_fixed(fixed f_num1, fixed f_num2){ return f_num1 + f_num2; }


fixed sub_fixed_fixed(fixed f_num1, fixed f_num2){ return f_num1 - f_num2; }

fixed mul_fixed_fixed(fixed f_num1, fixed f_num2){
	fixed result;
	fixed64_t x =  f_num1;
	result = (x* f_num2)/(FIXED_LOC);
	return result;
}


fixed div_fixed_fixed(fixed f_num1, fixed f_num2){
	fixed result;
	fixed64_t x = f_num1;
	result = (x*(FIXED_LOC))/f_num2;
	return result;
}







fixed add_fixed_int(fixed f_num, int num){
	fixed n = int_to_fixed(num);
	return add_fixed_fixed(f_num, n);
}


fixed sub_fixed_int(fixed f_num, int num){
	fixed n = int_to_fixed(num);
	return sub_fixed_fixed(f_num, n);

}

fixed mul_fixed_int(fixed f_num, int num){ return f_num * num; }

fixed div_fixed_int(fixed f_num, int num){ return f_num / num; }

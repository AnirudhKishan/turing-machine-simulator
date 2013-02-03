#include <stdbool.h>

struct turing_machine
{
	short unsigned int no_Q;		//number of states
	short unsigned int no_T;		//number of tape symbols
	short unsigned int no_F;		//number of final states
	short unsigned int *F;		//an array of final states
};

struct transcition_function
{
	bool valid;
	
	short unsigned int to_state;
	short unsigned int to_symbol;
	char movement_dir;
};

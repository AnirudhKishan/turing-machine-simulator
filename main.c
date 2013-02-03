#include <stdio.h>		//file handeling functions
#include <stdlib.h>		//validating user input
#include <stdbool.h>	//for boolean data type
#include <string.h>		//for string comparision
#include <ctype.h>		//for tolower()
#include "turing_machine.h"

#define MAX_TAPE_LENGTH	500

bool echo = false;

bool read_machine ( struct turing_machine *, struct transcition_function *** );
bool run_machine ( struct turing_machine, struct transcition_function ** );
bool final_state_check ( short unsigned int, short unsigned int *, short unsigned int );

int main ( int argc, char **argv )
{
	bool status;
	
	/*
	setting the echo printing to true, if requested from the command line
	*/
		if ( argc != 1 )	//checking if there is, in efect, an argument
		{
		
			int i, j;
			for ( j = 1; j < argc; j++ ) for ( i = 0; argv[j][i]; i++ ) argv[j][i] = tolower ( argv[j][i] );	//converting command line arguments to lowercase
		
			if ( ! strcmp ( argv[1], "true" ) )
			{
				echo = true;
			}
		}
	
		if ( echo == true ) { printf ( "\n\t$echo : echo printing enabled\n" ); }
	
	struct turing_machine machine;
	struct transcition_function **trans_func;
	
	status = read_machine ( &machine, &trans_func );
	
	status = run_machine ( machine, trans_func );
	
	if ( status == false )
	{
		printf ( "\n\n\t\t!!!THE MACHINE RUN FAILED!!!\n\n" );
		exit ( 1 );
	}
	
	short unsigned int i;
	for ( i = 0; i < machine.no_Q; i++ )
	{
		free ( trans_func[i] );
	}
	free ( trans_func );
	free ( machine.F );
	
	return 0;
}

bool read_machine ( struct turing_machine *machine, struct transcition_function *** trans_func )
{
	/*
	reading the machine from file
	*/	
		if ( echo == true ) { printf ( "\n\t$echo : Reading the Machine\n" ); }
	
		FILE* fin;
		fin = fopen ( "files/machine.tur", "r" );
		
		/*
		reading the number of states
		*/
			fscanf ( fin, "%hu", &(*machine).no_Q );		
			if ( echo == true ) { printf ( "\n\t$echo : Number of States read as : %u\n", (*machine).no_Q ); }
		
		/*
		reading the number of trape symbols
		*/
			fscanf ( fin, "%hu", &(*machine).no_T );
			if ( echo == true ) { printf ( "\n\t$echo : Number of Tape Symbols read as : %u\n", (*machine).no_T ); }
		
		char trash;
		fscanf ( fin, "%c", &trash );	//swallowing the newline
		
		/*
		reading the blank symbol
		*/
			//fscanf ( fin, "%hu", &(*machine).B );
			
			//the blank symbol will be taken as the next unused input symbol
			
			if ( echo == true ) { printf ( "\n\n\t$echo : The Blank Symbol read as : %hu", (*machine).no_T ); }
		
		//fscanf ( fin, "%c", &trash );	//swallowing the newline
		//no need for the above swallowing as the blank symbol os not being read, but is being understood implicitely
		
		/*
		reading the number of final states
		*/
			fscanf ( fin, "%hu", &(*machine).no_F );
			if ( echo == true ) { printf ( "\n\n\t$echo : Number of Final States read as : %hu", (*machine).no_F ); }
			
		/*
		reading the final states
		*/
			(*machine).F = (short unsigned int *) malloc ( (*machine).no_F * sizeof(short unsigned int) );
			
			unsigned int i;
			for ( i = 0; i < (*machine).no_F; i++ )
			{
				fscanf ( fin, "%hu", &(*machine).F[i] );
			}
			
			if ( echo == true )
			{
				printf ( "\n\n\t$echo : The Final States are : " );
				
				for ( i = 0; i < (*machine).no_F; i++ )
				{
					printf ( "%d ", (*machine).F[i] );
				}
			}
			
		if ( echo == true ) { printf ( "\n\n" ); }
		
		/*
		reading the transcition functions
		*/
			
			(*trans_func) = ( struct transcition_function ** ) malloc ( (*machine).no_Q * sizeof (struct transcition_function *) );
			for ( i = 0; i < (*machine).no_Q; i++ )
			{
				(*trans_func)[i] = ( struct transcition_function * ) malloc ( ( (*machine).no_T + 1 ) * sizeof (struct transcition_function) );
				//the "+ 1" is for accomodating the transcition functions on the blank symbol
			}
			
			short unsigned int j;
			for ( i = 0; i < (*machine).no_Q; i++ )
			{
				for ( j = 0; j < (*machine).no_T + 1; j++ )
				{
					(*trans_func)[i][j].valid = false;
				}
			}
			
			if ( echo == true ) { printf ( "The Transcition Functions :-\n\n" ); }
			
			char status;
			short unsigned int state, input, next_state, to_write;
			char movement_dir;
			
			fscanf ( fin, "%hu", &state );
			fscanf ( fin, "%hu", &input );
			
			status = fscanf ( fin, "\t%hu %hu %c", &next_state, &to_write, &movement_dir );
			
			while ( status != EOF )
			{
				
				(*trans_func)[state][input] . valid = true;
				(*trans_func)[state][input] . to_state = next_state;
				(*trans_func)[state][input] . to_symbol = to_write;
				(*trans_func)[state][input] . movement_dir = movement_dir;
				
				if ( echo == true )
				{
					printf ( "%hu,%hu : ", state, input );
					printf ( "%hu %hu %c\n", (*trans_func)[state][input] . to_state, (*trans_func)[state][input] . to_symbol, (*trans_func)[state][input] . movement_dir );
				}
				
				fscanf ( fin, "%hu", &state );
				fscanf ( fin, "%hu", &input );
				
				status = fscanf ( fin, "\t%hu %hu %c", &next_state, &to_write, &movement_dir );
				
			}
		
		fclose ( fin );
		
	return true;
}

bool run_machine ( struct turing_machine machine, struct transcition_function **trans_func )
{
	FILE *fp = fopen ( "files/input_tape.txt", "r" );
	
	short unsigned int tape[MAX_TAPE_LENGTH];
	
	char status;
	unsigned int ct = 0;
	char trash;
	
	status = fscanf ( fp, "%hu", &tape[ct] );
	status = fscanf ( fp, "%c", &trash );
	while ( status != EOF )
	{
		ct++;
		
		status = fscanf ( fp, "%hu", &tape[ct] );
		status = fscanf ( fp, "%c", &trash );
	}
	
	unsigned int i;
	if ( echo == true )
	{
		printf ( "\n\tThe tape has been read as : " );
		
		for ( i = 0; i < ct; i++ )
		{
			printf ( "%hu", tape[i] );
		}
		
		printf ( "\n" );
		
	}
	
	while ( ct < MAX_TAPE_LENGTH )
	{
		tape[ct] = machine.no_T;	//as machine.no_T WOULD be the blank symbol
		
		ct++;
	}	
	
	
	short unsigned int curr_state = 0;
	unsigned int curr_pos = 0;
	
	short unsigned int next_state, next_symbol;
	unsigned int next_pos;
	
	while ( ! final_state_check ( curr_state, machine.F, machine.no_F ) )
	{
		
		if ( trans_func [ curr_state ] [ tape[curr_pos] ] . valid == false )
		{
			if ( echo == true )
			{
				printf ( "\n\n\t\t!!!The machine run will fail because %hu, %hu was not specified!!!\n\n", curr_state, tape[curr_pos] );
			}
			
			return false;
		}
		
		next_state = trans_func [ curr_state ] [ tape[curr_pos] ] . to_state;
		
		next_symbol = trans_func [ curr_state ] [ tape[curr_pos] ] . to_symbol;
		
		if ( trans_func [ curr_state ] [ tape[curr_pos] ] . movement_dir == 'R' )
		{
			next_pos = curr_pos + 1;
		}
		else
		{
			next_pos = curr_pos - 1;
		}
		
		if ( echo == true )
		{
			printf ( "\n\n( %hu, %hu[%hu] ) -> ( %hu, %hu, %hu )\n", curr_state, tape[curr_pos], curr_pos, next_state, next_symbol, next_pos );
			
			printf ( "\n\t" );
			for ( i = 0; tape[i] != machine.no_T; i++ )
			{
				if ( i == curr_pos ) { printf ( "--" ); }
				else if ( i == next_pos )
				{
					if ( next_pos > curr_pos ) { printf ( ">" ); }
					else { printf ( "<" ); }
				}
				else { printf ( "  " ); }
			}
		}
		
		curr_state = next_state;
		tape[curr_pos] = next_symbol;
		curr_pos = next_pos;
		
		if ( echo == true )
		{
			printf ( "\n\t" );
			for ( i = 0; tape[i] != machine.no_T; i++ )
			{
				printf ( "%hu ", tape[i] );
			}
			printf ( "\n" );
		}
	}
	
	fclose ( fp );
	
	fp = fopen ( "files/output_tape", "w" );
	
	curr_pos = 0;
	while ( tape[curr_pos] != machine.no_T )	//here machine.no_T represents the blank symbol
	{
		fprintf ( fp, "%d ", tape[curr_pos] );
		
		curr_pos++;
	}
	
	if ( echo == true )
	{
		printf ( "\nThe final Tape : \n\t" );
		for ( i = 0; tape[i] != machine.no_T; i++ )
		{
			printf ( "%hu ", tape[i] );
		}
		printf ("\n" );
	}

	fclose ( fp );
	
	return true;
}

bool final_state_check ( short unsigned int curr_state, short unsigned int *F, short unsigned int no_F )
{
	short unsigned int i;
	for ( i = 0; i < no_F; i++ )
	{
		if ( curr_state == F[i] )
			return true;
	}
	
	return false;
}

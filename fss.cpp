/*
 * Copyright (c) 2012 University of Stuttgart,
 *                    Sergej Geringer, Karsten Schatz, Florian Frieß
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * this program was written by students in context of the course
 * 'introduction to artificial intelligence' in winter 2012
 * at the University of Stuttgart, 
 * Institute for Visualization and Interactive Systems,
 * given by Prof. Dr.-Ing. Andrés Bruhn.
 * 
 * the goal was write a simple and relatively fast solver for sudokus. 
 * this implementation uses a recursive strategy, calling for the solve() 
 * function and ensuring consistency of the empty and already filled slots 
 * by keeping track of the possible valid entries. 
 * this is done in sudoku.update_slots() in each recursive call of solve().
 *
 * in short: the sudoku Constraint Satisfaction Problem is solved by 
 * determining the next empty field using a 'Minimum Remaining Values' 
 * strategy and backtracking the way throught the search-graph.
 *
 * contact: geringsj@studi.informatik.uni-stuttgart.de
 */

#include <stdio.h>
#include <stdlib.h>
#include <list>
#include "time.h"

/*=====================================*/
/* ========== SUDOKU STRUCTS ========= */
/*=====================================*/

struct slot
{
	/* coordinates in field */
	unsigned int x;
	unsigned int y;

	/* possible values */
	int valid_values_count;
	bool valid_values[10];

	slot() : x(0), y(0), valid_values_count(0)
	{}
	~slot()
	{}
};

struct sudoku 
{
	unsigned int field[9][9];/* well, the field itself ... */
	
	slot* slots;/* all initially empty slots */
	unsigned int* empty_slots;/* referencing unused slots in 'slots' */
	slot* minimum_empty_slot;/* the slot with least amaount of possible values */
	unsigned int minimum_empty_slot_index; /* array-index for the min slot */
	unsigned int empty_slots_count;/* number of remaining empty slots */

	sudoku() : 
		slots(0), 
		empty_slots(0), 
		minimum_empty_slot(0), 
		minimum_empty_slot_index(0),
		empty_slots_count(0)
	{}
	~sudoku()
	{
		delete[] slots; slots = 0;
		delete[] empty_slots; empty_slots = 0;
		minimum_empty_slot = 0;
	}
	void init()
	{ /* prepare slots and stuff */
		slots = new slot[empty_slots_count];
		unsigned int counter = 0;
		for(unsigned int i = 0; i < 9; i++)
		{
			for(unsigned int j = 0; j < 9; j++)
			{
				if( field[i][j] == 0 )
				{
					slots[counter].x = i;
					slots[counter].y = j;
					counter++;
				}
			}
		}
		/* the empty_slots array is for indirect access into the slots array */
		/* for, like, constant acess, without moving around that stuff, y'know */
		empty_slots = new unsigned int[empty_slots_count];
		for(unsigned int i = 0; i < empty_slots_count; i++)
		{
			empty_slots[i] = i;
		}
		minimum_empty_slot = 0;
	}

	/*
	 * this verifies and updates the slots(= variables)
	 * in the sudoku. a few things are done at once:
	 *
	 * 1) for each non-used slot compute the possible (=valid) values
	 *    you can assign to the slot. 
	 *
	 * 2) among all slots in the array empty_slots, 
	 *    move those with exactly 0 (= zero) possible values behind all slots 
	 *    with != 0 possible values. this is done because slots with no (=zero) 
	 *    valid values are not of interest for further recursive calls of 
	 *    solve(), for they are fulfilled constraints, but when we return 
	 *    unsucsessful from solve(), those zero-slots are normal slots to 
	 *    solve()-calls above us in the recursive tree, so we give those 
	 *    constraints back to our forefathers.
	 *
	 * 3) among all slots with != 0 possible valid values, move the slot with 
	 *    least amount of possible values to the end of the array, but put it 
	 *    before all zero-valued slots as in 2). 
	 *    because this minimum-slot has the least amount of possible values,
	 *    we should try it before all other slots. 
	 *    this guarantess that we put minimum effort into trying out values,
	 *    hence we should be faster. 
	 *    
	 *    (==> Minimum Remaining Values for CSP Backtracking)
	 *
	 *    also make this minimum-slot available to the solve() function by the
	 *    minimum_empty_slot pointer
	 *
	 * 4) if a slot runs into a dead end, because there are no valid values for
	 *    this slot left, but there are still zeros in the corresponding 
	 *    row, column and block, set the number of possible valid values to -1.
	 *    this will stop further search in this part of the search tree
	 *    and lead us to a correct solution, if there is one.
	 */
	unsigned int update_slots()
	{
		minimum_empty_slot = 0;
		minimum_empty_slot_index = 0;
		unsigned int current_zeros = 0;
		for(unsigned int s = 0; s < empty_slots_count; s++)
		{
			slot* tmp = slots + empty_slots[s];
			
			/* reset current slot */
			for(unsigned int i = 0; i < 10; i++)
			{
				tmp->valid_values[i] = true;
			}
			/* handle row */
			for(unsigned int i = 0; i < 9; i++)
			{
				tmp->valid_values[ field[i][tmp->y] ] = false;
			}
			/* handle column */
			for(unsigned int i = 0; i < 9; i++)
			{
				tmp->valid_values[ field[tmp->x][i] ] = false;
			}
			/* handle block */
			unsigned int xi = ((tmp->x / 3)*3);
			unsigned int yi = ((tmp->y / 3)*3);
			for(unsigned int i = xi; i < xi+3; i++)
			{
				for(unsigned int j = yi; j < yi+3; j++)
				{
					tmp->valid_values[ field[i][j] ] = false;
				}
			}
			/* count valid values */
			tmp->valid_values_count = 0;
			for(unsigned int i = 1; i < 10; i++)
			{
				if( tmp->valid_values[i] )
					tmp->valid_values_count++;
			}
			/* if all values, including 0, are present in all constraints
			 * we ran into a dead end and need to communicate this */
			if ( 
					!tmp->valid_values[0] &&
					!tmp->valid_values[1] &&
					!tmp->valid_values[2] &&
					!tmp->valid_values[3] &&
					!tmp->valid_values[4] &&
					!tmp->valid_values[5] &&
					!tmp->valid_values[6] &&
					!tmp->valid_values[7] &&
					!tmp->valid_values[8] &&
					!tmp->valid_values[9]
				)
			{
				tmp->valid_values_count = -1;
			}
			/* handle zero possible values */
			if( tmp->valid_values_count == 0)
			{
				current_zeros++;
				empty_slots_count--;
				/* swap last non-zero, not handled entry in array with 
				 * the current zero-valued entry. 
				 * we will take a look at the new entry at position s
				 * and later ignore the zero-valued
				 */
				unsigned int tmpindex = empty_slots[empty_slots_count];
				empty_slots[empty_slots_count] = empty_slots[s];
				empty_slots[s] = tmpindex;
				s--;
			}
			else
			{
				/* if the current slot has at least one
				 * possible value (or -1 for the special case), 
				 * it may be the the one slot with 
				 * minimal valid value count (!= zero) we are looking for
				 */

				/* assign slot with minimal number of possible values */
				if( minimum_empty_slot != 0 )
				{
					if ( 
							minimum_empty_slot->valid_values_count 
							> tmp->valid_values_count 
						)
					{
						minimum_empty_slot = tmp;
						minimum_empty_slot_index = s;
					}
				}
				else
				{
					minimum_empty_slot = tmp;
					minimum_empty_slot_index = s;
				}
			}
		} /* end for(all slots) */

		/* move minimum slot to end of array */
		if( minimum_empty_slot != 0 )
		{
			unsigned int tmpindex = empty_slots[empty_slots_count-1];
			empty_slots[empty_slots_count-1] 
				= empty_slots[minimum_empty_slot_index];
			empty_slots[minimum_empty_slot_index] = tmpindex;
		} 

		/* return zero-offset, so the zeros will not be ignored later */
		return current_zeros;
	} /* end update() */
};

/*=====================================*/
/* ====== SUDOKU SOLVER FUNCTION ===== */
/*=====================================*/

bool solve(sudoku* sudoku_instance, unsigned int r)
{
	unsigned int current_zeros = 
		sudoku_instance->update_slots();

	if( sudoku_instance->minimum_empty_slot == 0 )
	{ /* no more slots to fill */
		sudoku_instance->empty_slots_count += current_zeros;
		return true;
		/* cleaning up the values is not really necessary,
		 * since we already won. but we don't want to be rude */
	}
	/* we have a nice slot available */
	
	/* take away slot from competitors */
	slot* mysloth = sudoku_instance->minimum_empty_slot;
	sudoku_instance->empty_slots_count--;

	/* if we are not stuck, */
	if( mysloth->valid_values_count != -1 )
	/* try all possible values for slot */
	for(unsigned int i = 1; i < 10; i++)
	{
		if ( mysloth->valid_values[i] )
		{
			sudoku_instance->field[mysloth->x][mysloth->y] = i;
			bool recursion_success = solve(sudoku_instance, r+1);

			if ( recursion_success )
			{
				sudoku_instance->empty_slots_count += current_zeros;
				sudoku_instance->empty_slots_count ++;
				mysloth = 0;
				return true;
				/* cleaning up the values is not really necessary,
				 * since we already won. but we don't want to be rude */
			}
		}
	}

	/* we had no luck, although we really tried */
	sudoku_instance->field[mysloth->x][mysloth->y] = 0;
	sudoku_instance->empty_slots_count += current_zeros;
	sudoku_instance->empty_slots_count ++;
	mysloth = 0;
	return false; /* ^ cleanup is indeed needed */
}

/* this is to verify found solutions
 * you know, for debugging, 
 * not for finding solutions
 */
bool verify(sudoku* sudoku_instance)
{
	bool occured[10];

	/* all rows */
	for(int i = 0; i < 9; i++)
	{
		for(int ii = 0; ii < 10; ii++)
		{
			occured[ii] = false;
		}
		for(int j = 0; j < 9; j++)
		{
			occured[ sudoku_instance->field[i][j] ] = true;
		}
		if(occured[0])
			return false;
		for(int ii = 1; ii < 10; ii++)
		{
			if(! occured[ii])
				return false;
		}
	}
	/* all columns */
	for(int i = 0; i < 9; i++)
	{
		for(int ii = 0; ii < 10; ii++)
		{
			occured[ii] = false;
		}
		for(int j = 0; j < 9; j++)
		{
			occured[ sudoku_instance->field[j][i] ] = true;
		}
		if(occured[0])
			return false;
		for(int ii = 1; ii < 10; ii++)
		{
			if(! occured[ii])
				return false;
		}
	}
	/* all blocks */
	for(int i = 0; i < 9; i = i+3)
	{
		for(int ii = 0; ii < 10; ii++)
		{
			occured[ii] = false;
		}
		for(int j = 0; j < 9; j = j+3)
		{
			for(int m = i; m < i+3; m++)
			{
				for(int n = j; n < j+3; n++)
				{
					occured[ sudoku_instance->field[m][n] ] = true;
				}
			}
		}
		if(occured[0])
			return false;
		for(int ii = 1; ii < 10; ii++)
		{
			if(! occured[ii])
				return false;
		}
	}

	return true;
}

/*=====================================*/
/* ========== FILE HANDLING ========== */
/*=====================================*/

/* reads a .txt file with sudokus of the form
 *
 * XXXXXXXXX
 * XXXXXXXXX
 * XXXXXXXXX
 * XXXXXXXXX
 * XXXXXXXXX
 * XXXXXXXXX
 * XXXXXXXXX
 * XXXXXXXXX
 * XXXXXXXXX
 *
 * where X is a number in 0,...,9
 * or the character '-'. '0' and '-' indicate
 * empty fields in the sudoku, which are to be filled
 * with respect to the other entries with values 1,...,9
 *
 * several sudokus may be passed in one file, but they should be
 * separated by empty lines
 */
bool read(std::list<sudoku>* sudoku_list, char const * const filename)
{
	//char const * const filename = "aufgabe.txt";

	/* open file */
	char buffer[12];
	FILE* file = fopen(filename, "rb");

	if( file == NULL )
	{
		printf("-> Error opening file \"%s\" for reading. \n", filename);
		return false;
	}
	else
	{
		printf("-> Opened file \"%s\" for reading. \n", filename);
	}

	/* handle sudokus */
	sudoku* tmp = 0;
	char* state = fgets(buffer, sizeof(buffer), file);/* initial first line */
	while( state != NULL )
	{
		if( buffer[0] != '\n' && buffer[0] != '\r')
		{ /* try to read one sudoku */
			sudoku_list->push_back(sudoku());
			tmp = &(sudoku_list->back());

			for(unsigned int i = 0; i < 9; i++)
			{ /* read 9 lines, for each line 9 characters */
				
				for(unsigned int j = 0; j < 9; j++)
				{
					if( buffer[j] == '-' )
					{
						tmp->field[i][j] = 0;
						tmp->empty_slots_count++;
						/* empy slots is initialized to 0 in the constructor */
					}
					else
					{
						char single = buffer[j];
						unsigned int entry = single - '0'; 
						/* the charackters '0' to '9' have sequential
						 * codes, so (X - '0') is the integer, which hides
						 * behind the character X */
						tmp->field[i][j] = entry;
						if( entry == 0 )
							tmp->empty_slots_count++;
					}
				}

				state = fgets(buffer, sizeof(buffer), file);/* get next line */
			}
			tmp = 0;
		}
		else
		{ /* go on to the next non-empty line */
			state = fgets(buffer, sizeof(buffer), file);/* get next line */
		}
	}
	printf("-> Parsed %li Sudokus. \n", sudoku_list->size());

	fclose(file);
	printf("-> Closed file \"%s\". \n", filename);
	return true;
}

bool write(std::list<sudoku>* sudoku_list, char const * const filename)
{
	//char const * const filename = "loesung.txt";

	/* open file to write into */
	FILE* file = fopen(filename, "wb");

	if( file == NULL )
	{
		printf("-> Error opening file \"%s\" for writing. \n", filename);
		return false;
	}
	else
	{
		printf("-> Opened file \"%s\" for writing. \n", filename);
	}

	/* handle sudokus */ 
	while( ! sudoku_list->empty() )
	{
		sudoku* tmp = &(sudoku_list->front());

		/* write current sudoku in file */
		for(unsigned int i = 0; i < 9; i++)
		{
			for(unsigned int j = 0; j < 9; j++)
			{
				fprintf(file, "%u", tmp->field[i][j]);
			}
			fprintf(file, "\n");
		}
		fprintf(file, "\n");

		sudoku_list->pop_front();
		tmp = 0;
	}

	/* close file */
	printf("-> Closed file \"%s\". \n", filename);
	fclose(file);
	return true;
}

/*=====================================*/
/* =============== MAIN ============== */
/*=====================================*/

int main(int argc, char const ** argv)
{
	clock_t start = clock();

	std::list<sudoku> sudokus;

	/* read sudokus from file */
	if( ! read(&sudokus, "aufgabe.txt") )
	{ printf("-> Exiting with error\n"); return 0; }

	/* solve sudokus */
	int counter = 0;
	printf("-> Solving sudokus\n");
	for(std::list<sudoku>::iterator i = sudokus.begin(); i != sudokus.end(); i++)
	{
		i->init();
		bool result = solve(&*i,0);
		printf("--> Sudoku %i solving result %i \n", ++counter, result );
		if(!result)
			printf("----> there is most probably no solution for this sudoku \n" );
 		//printf("--> Sudoku result verification %i \n", verify(&*i) );
	}

	/* write solutions into solution file */
	if( ! write(&sudokus, "loesung.txt") )
	{ printf("-> Exiting with error\n"); return 0; }

	printf("-> Exiting successfully\n");

	clock_t end = clock();
	float seconds = (float)(((float)end-(float)start)/((float)CLOCKS_PER_SEC));
	printf("-> Execution Time: ~ %fsec \n", seconds);
	return 0;
}

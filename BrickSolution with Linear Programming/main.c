#include <stdio.h> /* needed for input, output, ... */
#include <stdlib.h> /* needed for EXIT_SUCCESS, ... */
#include <ctype.h> /* needed for isspace() */
#include <string.h> /* needed for memset() */
#include <glpk.h> /* the linear programming toolkit */

/* global variables -- YOU SHOULD NOT CHANGE THIS! */
/* (You are allowed to add your own if you want.) */
int numRows; /* number of rows in the puzzle */
int length; /* length of each row in the puzzle */
int *lengths; /* array containing the lengths of each break (0 for no brick) */
int *values; /* array containing the values of each break (undef if no brick) */
int *solution; /* array containing the solution (1 for brick taken) */
int debug; /* flag for debug mode; 1 means debug mode, 0 means debug off */

/* prototypes of functions -- YOU SHOULD NOT CHANGE THIS! */
/* (Feel free to add your own as you like.) */

int readInput(char *filename); /* reads puzzle from file */
/* readInput creates and fills the global variables as needed */
/* it returns 0 if all is okay and 1 otherwise */

void computeSolution(void); /* computes a solution and stores it in solution */

/* It is currently empty and not functional. */
void computeSolution(void) {
    /* Your own implementation needs to go in here. */
    //fprintf(stdout, "About to create a glp_prob instance...\n");
    glp_prob *lp;

    lp = glp_create_prob(); /* the linear programming problem */

    //fprintf(stdout, "Setting Objective...\n");
    glp_set_obj_dir(lp,GLP_MAX);

    //int i, j, k, l; /* loop variables to go over rows and bricks */
    int checkLen, checkLenRowAbove, checkLenRowCurrent, checkLenRowBelow; /* add length in each row to check */
    int brickCount = 0;
    int edgeCount = 0;
    int pointerToEdge = 1;
    int constraintCounter = 1;
    int constraintVariableCounter = 1;
    int edgeFromBrickToSourceOrSink;
    int edgeFromBrickToBrickAbove;
    int edgeFromBrickToBrickBelow;

    //fprintf(stdout, "rows = %d, row length = %d\n", numRows, length);

    /* read rows one after the other to count how many bricks we have e.g num of variables */
    for (int i = 0; i < numRows; i++) {

        checkLen = 0; /* read bricks until length is reached */
        int j = 0; /* start with brick 0 */
        while (checkLen < length) {

            //fprintf(stdout, "Row %d brick %d has length=%d, value=%d\n",
            //i+1, j+1, lengths[i * length + j], values[i * length + j]);

            checkLen += lengths[i * length + j];
            brickCount++;
            edgeCount++;
            j++;
        }
    }

    fprintf(stdout,"Brick count set as: %d\n", edgeCount);

    /* read rows one after the other to count how many edge there are between bricks */
    for (int i = 0; i < numRows-1; i++) {

        checkLenRowAbove = 0; /* read bricks until length is reached */
        checkLenRowCurrent = 0;
        int j = 0; /* points to brick in upper row to add edge */
        int k = 0; /* points to brick in lower row to add edge */

        //fprintf(stdout,"Row: %d\n", i);
        while (checkLenRowAbove < length || checkLenRowCurrent < length) {
            if (checkLenRowAbove == checkLenRowCurrent){
                //fprintf(stdout,"checkLenRowAbove == checkLenRowCurrent\n");
                checkLenRowAbove += lengths[i * length + j];
                //fprintf(stdout,"checkLenRowAbove: %d\n", checkLenRowAbove);
                checkLenRowCurrent += lengths[(i + 1) * length + k];
                //fprintf(stdout,"checkLenRowCurrent: %d\n", checkLenRowCurrent);
                j++;
                k++;
                edgeCount++;

            } else if(checkLenRowAbove > checkLenRowCurrent){
                //fprintf(stdout,"checkLenRowAbove > checkLenRowCurrent\n");
                checkLenRowCurrent += lengths[(i + 1) * length + k];
                //fprintf(stdout,"checkLenRowCurrent: %d\n", checkLenRowCurrent);
                k++;
                edgeCount++;

            } else if(checkLenRowAbove < checkLenRowCurrent){
                //fprintf(stdout,"checkLenRowAbove < checkLenRowCurrent\n");
                checkLenRowAbove += lengths[i * length + j];
                //fprintf(stdout,"checkLenRowAbove: %d\n", checkLenRowAbove);
                j++;
                edgeCount++;

            }
        }
    }

    //fprintf(stdout,"Edge count set as: %d\n", edgeCount);

    glp_add_cols(lp,edgeCount);

    /* Now assign edge with constraints, start with bricks to sink/source */
    for (int i = 0; i < numRows; i++) {

        checkLen = 0; /* read bricks until length is reached */
        int j = 0; /* start with brick 0 */
        while (checkLen < length) {

            //fprintf(stdout, "Row %d brick %d has length=%d, value=%d\n", i, j, lengths[i * length + j], values[i * length + j]);

            if(values[i * length + j] > 0){
                //fprintf(stdout,"Value of brick row %d brick %d positive, adding to objective function...\n", i+1,j+1);
                glp_set_obj_coef(lp,pointerToEdge,1.0);
                //fprintf(stdout,"Capacity from source to brick: %d\n-------------------------------\n",values[i * length + j]);
            } else{
                //printf("Value of brick row %d brick %d negative...\n", i+1, j+1);
            }

            glp_set_col_bnds(lp,pointerToEdge,GLP_DB,0.0,abs(values[i * length + j]));
            //printf("edge x%d constraints set with capacity: %d\n----------------------------\n",pointerToEdge,abs(values[i * length + j]));

            pointerToEdge++;
            checkLen += lengths[i * length + j];
            j++;
        }
    }

    /* Now assign edge with constraints, now setting infinite capacities between bricks */
    for (int i = 0; i < numRows-1; i++) {

        checkLenRowAbove = 0; /* read bricks until length is reached */
        checkLenRowCurrent = 0;
        int j = 0; /* points to brick in upper row to add edge */
        int k = 0; /* points to brick in lower row to add edge */

        while (checkLenRowAbove < length || checkLenRowCurrent < length) {
            if (checkLenRowAbove == checkLenRowCurrent){
                //fprintf(stdout,"checkLenRowAbove == checkLenRowCurrent\n");
                checkLenRowAbove += lengths[i * length + j];
                //fprintf(stdout,"checkLenRowAbove: %d\n", checkLenRowAbove);
                checkLenRowCurrent += lengths[(i + 1) * length + k];
                //fprintf(stdout,"checkLenRowCurrent: %d\n", checkLenRowCurrent);
                j++;
                k++;

            } else if(checkLenRowAbove > checkLenRowCurrent){
                //fprintf(stdout,"checkLenRowAbove > checkLenRowCurrent\n");
                checkLenRowCurrent += lengths[(i + 1) * length + k];
                //fprintf(stdout,"checkLenRowCurrent: %d\n", checkLenRowCurrent);
                k++;

            } else if(checkLenRowAbove < checkLenRowCurrent){
                //fprintf(stdout,"checkLenRowAbove < checkLenRowCurrent\n");
                checkLenRowAbove += lengths[i * length + j];
                //fprintf(stdout,"checkLenRowAbove: %d\n", checkLenRowAbove);
                j++;

            }
            glp_set_col_bnds(lp,pointerToEdge,GLP_LO,0.0,0.0);
            //printf("edge x%d constraints set with infinite capacity\n----------------------------\n",pointerToEdge);
            pointerToEdge++;
        }
    }

    //fprintf(stdout,"BrickCount and therefore constrains count: %d\n", brickCount);

    glp_add_rows(lp,brickCount);

    int index[brickCount+1]; //constraints will be == to brick count. +1 required as constraints start a 1.
    double row[brickCount+1]; //constraints will be == to brick count. +1 required as constraints start a 1.

    //loop for setting constraints
    //printf("Beginning constraint set up...\n");
    for(int i = 1; i<brickCount+1; i++){
        glp_set_row_bnds(lp,i,GLP_FX,0.0,0.0);
        //printf("Creating constraint %d\n------------------\n",i);
    }

    edgeFromBrickToSourceOrSink = 1;
    edgeFromBrickToBrickBelow = brickCount+1;
    edgeFromBrickToBrickAbove = brickCount+1;

    /* loop to finalizing constraints */
    for (int i = 0; i < numRows; i++) {

        checkLenRowAbove = 0; // current length of upper row
        checkLenRowCurrent = 0; // current length of current row
        checkLenRowBelow = 0; // current length of lower row
        int j = 0; /* points to brick in upper row */
        int k = 0; /* points to brick in current row */
        int l = 0; /* points to brick in lower row */

        //printf("Starting row %d...\n-------------\n",i+1);

        //state where numRows is 1, so no rows below or above
        if(numRows == 1){
            while(checkLenRowCurrent < length){
                //first atatch brick so source or sink
                if(values[i * length + k] > 0){
                    index[constraintVariableCounter]=edgeFromBrickToSourceOrSink; row[constraintVariableCounter]=1.0;
                    //printf("Positive value, source to brick direction\n");
                    //printf("constraintCounter: %d\nconstraintVariableCounter: %d\nedge: x%d\n--------------------\n", constraintCounter, constraintVariableCounter, edgeFromBrickToSourceOrSink);
                    constraintVariableCounter++;
                } else{
                    index[constraintVariableCounter]=edgeFromBrickToSourceOrSink; row[constraintVariableCounter]=-1.0;
                    //printf("Negative value, brick to sink direction\n");
                    //printf("constraintCounter: %d\nconstraintVariableCounter: %d\nedge: -x%d\n--------------------\n", constraintCounter, constraintVariableCounter, edgeFromBrickToSourceOrSink);
                    constraintVariableCounter++;
                }

                //add length of first brick
                checkLenRowCurrent += lengths[i * length + k];
                k++;

                //add completed constraint
                glp_set_mat_row(lp,constraintCounter,constraintVariableCounter-1,index,row);

                edgeFromBrickToSourceOrSink++; //increase counter for brick we are looking at
                constraintCounter++; //increase constraint counter
                constraintVariableCounter = 1; //reset constraint variable counter
            }
        } else if(i == 0 && numRows > 1){ //state where we are on first row so only need to check row below

            while(checkLenRowCurrent < length || checkLenRowBelow < length){
                //first atatch brick so source or sink
                if(values[i * length + k] > 0){
                    index[constraintVariableCounter]=edgeFromBrickToSourceOrSink; row[constraintVariableCounter]=1.0;
                    //printf("Positive value, source to brick direction\n");
                    //printf("constraintCounter: %d\nconstraintVariableCounter: %d\nedge: x%d\n--------------------\n", constraintCounter, constraintVariableCounter, edgeFromBrickToSourceOrSink);
                    constraintVariableCounter++;
                } else{
                    index[constraintVariableCounter]=edgeFromBrickToSourceOrSink; row[constraintVariableCounter]=-1.0;
                    //printf("Negative value, brick to sink direction\n");
                    //printf("constraintCounter: %d\nconstraintVariableCounter: %d\nedge: -x%d\n--------------------\n", constraintCounter, constraintVariableCounter, edgeFromBrickToSourceOrSink);
                    constraintVariableCounter++;
                }

                //checks to see if brick lenghths are equal. If current row is not equal to lower row need to move row length
                //back to ensure we are attatching all proper bricks to one another. Not going back means we may miss bricks
                //if equal we know we will not miss any
                if(checkLenRowCurrent != checkLenRowBelow){
                    l--;
                    checkLenRowBelow -= lengths[(i + 1) * length + l];
                }

                //add length of first brick
                checkLenRowCurrent += lengths[i * length + k];
                k++;

                //while current row greater then row below, add bricks to constraint, when lesser we know we are done
                //and can move to next brick
                while (checkLenRowCurrent > checkLenRowBelow){
                    //printf("constaintCounter: %d\nconstraintVariableCounter: %d\nedge: +x%d\n--------------------\n",constraintCounter,constraintVariableCounter,edgeFromBrickToBrickBelow);
                    index[constraintVariableCounter]=edgeFromBrickToBrickBelow; row[constraintVariableCounter]=1.0;
                    constraintVariableCounter++;
                    checkLenRowBelow += lengths[(i + 1) * length + l];
                    l++;
                    edgeFromBrickToBrickBelow++;
                }

                //add completed constraint
                glp_set_mat_row(lp,constraintCounter,constraintVariableCounter-1,index,row);

                edgeFromBrickToSourceOrSink++; //increase counter for brick we are looking at
                constraintCounter++; //increase constraint counter
                constraintVariableCounter = 1; //reset constraint variable counter
            }

        } else if(i == numRows-1){ //state where we are on last row so only have to check row above

            while(checkLenRowAbove < length || checkLenRowCurrent < length) {
                //first atatch brick so source or sink
                if (values[i * length + k] > 0) {
                    index[constraintVariableCounter] = edgeFromBrickToSourceOrSink;
                    row[constraintVariableCounter] = 1.0;
                    //printf("Positive value, source to brick direction\n");
                    //printf("constraintCounter: %d\nconstraintVariableCounter: %d\nedge: x%d\n--------------------\n",
                    //constraintCounter, constraintVariableCounter, edgeFromBrickToSourceOrSink);
                    constraintVariableCounter++;
                } else {
                    index[constraintVariableCounter] = edgeFromBrickToSourceOrSink;
                    row[constraintVariableCounter] = -1.0;
                    //printf("Negative value, brick to sink direction\n");
                    //printf("constraintCounter: %d\nconstraintVariableCounter: %d\nedge: -x%d\n--------------------\n",
                    //constraintCounter, constraintVariableCounter, edgeFromBrickToSourceOrSink);
                    constraintVariableCounter++;
                }

                //checks to see if brick lenghths are equal. If current row is not equal to above row need to move row length
                //back to ensure we are attatching all proper bricks to one another. Not going back means we may miss bricks
                //if equal we know we will not miss any
                if(checkLenRowAbove != checkLenRowCurrent){
                    j--;
                    checkLenRowAbove -= lengths[(i - 1) * length + j];
                }

                //add length of first brick
                checkLenRowCurrent += lengths[i * length + k];
                k++;

                //while current row greater then row above, add bricks to constraint, when lesser we know we are done
                //and can move to next brick
                while (checkLenRowCurrent > checkLenRowAbove){
                    //printf("constaintCounter: %d\nconstraintVariableCounter: %d\nedge: -x%d\n--------------------\n",constraintCounter,constraintVariableCounter,edgeFromBrickToBrickAbove);
                    index[constraintVariableCounter]=edgeFromBrickToBrickAbove; row[constraintVariableCounter]= -1.0;
                    constraintVariableCounter++;
                    checkLenRowAbove += lengths[(i - 1) * length + j];
                    j++;
                    edgeFromBrickToBrickAbove++;
                }

                //add completed constraint
                glp_set_mat_row(lp,constraintCounter,constraintVariableCounter-1,index,row);

                edgeFromBrickToSourceOrSink++; //increase counter for brick we are looking at
                constraintCounter++; //increase constraint counter
                constraintVariableCounter = 1; //reset constraint variable counter

            }

        } else { //middle rows so need to check above and below

            while(checkLenRowCurrent < length || checkLenRowBelow < length){
                //first atatch brick so source or sink
                if(values[i * length + k] > 0){
                    index[constraintVariableCounter]=edgeFromBrickToSourceOrSink; row[constraintVariableCounter]=1.0;
                    //printf("Positive value, source to brick direction\n");
                    //printf("constraintCounter: %d\nconstraintVariableCounter: %d\nedge: x%d\n--------------------\n", constraintCounter, constraintVariableCounter, edgeFromBrickToSourceOrSink);
                    constraintVariableCounter++;
                } else{
                    index[constraintVariableCounter]=edgeFromBrickToSourceOrSink; row[constraintVariableCounter]=-1.0;
                    //printf("Negative value, brick to sink direction\n");
                    //printf("constraintCounter: %d\nconstraintVariableCounter: %d\nedge: -x%d\n--------------------\n", constraintCounter, constraintVariableCounter, edgeFromBrickToSourceOrSink);
                    constraintVariableCounter++;
                }

                //checks to see if brick lenghths are equal. If current row is not equal to above row need to move row length
                //back to ensure we are attatching all proper bricks to one another. Not going back means we may miss bricks
                //if equal we know we will not miss any
                if(checkLenRowAbove != checkLenRowCurrent){
                    j--;
                    checkLenRowAbove -= lengths[(i - 1) * length + j];
                }
                //checks to see if brick lenghths are equal. If current row is not equal to lower row need to move row length
                //back to ensure we are attatching all proper bricks to one another. Not going back means we may miss bricks
                //if equal we know we will not miss any
                if(checkLenRowCurrent != checkLenRowBelow){
                    l--;
                    checkLenRowBelow -= lengths[(i + 1) * length + l];
                }

                //add length of first brick
                checkLenRowCurrent += lengths[i * length + k];
                k++;

                //while current row greater then row above, add bricks to constraint, when lesser we know we are done
                //and can move to next brick
                while (checkLenRowCurrent > checkLenRowAbove){
                    //printf("constaintCounter: %d\nconstraintVariableCounter: %d\nedge: -x%d\n--------------------\n",constraintCounter,constraintVariableCounter,edgeFromBrickToBrickAbove);
                    index[constraintVariableCounter]=edgeFromBrickToBrickAbove; row[constraintVariableCounter]= -1.0;
                    constraintVariableCounter++;
                    checkLenRowAbove += lengths[(i - 1) * length + j];
                    j++;
                    edgeFromBrickToBrickAbove++;
                }
                //while current row greater then row below, add bricks to constraint, when lesser we know we are done
                //and can move to next brick
                while (checkLenRowCurrent > checkLenRowBelow){
                    //printf("constaintCounter: %d\nconstraintVariableCounter: %d\nedge: +x%d\n--------------------\n",constraintCounter,constraintVariableCounter,edgeFromBrickToBrickBelow);
                    index[constraintVariableCounter]=edgeFromBrickToBrickBelow; row[constraintVariableCounter]= 1.0;
                    constraintVariableCounter++;
                    checkLenRowBelow += lengths[(i + 1) * length + l];
                    l++;
                    edgeFromBrickToBrickBelow++;
                    //edgeFromBrickToBrickReduction++;
                }

                //add completed constraint
                glp_set_mat_row(lp,constraintCounter,constraintVariableCounter-1,index,row);

                edgeFromBrickToSourceOrSink++; //increase counter for brick we are looking at
                constraintCounter++; //increase constraint counter
                constraintVariableCounter = 1; //reset constraint variable counter
            }
        }
    }

    glp_term_out(0);

    glp_simplex(lp,NULL);

    fprintf(stdout,"Max Flow: %f\n", glp_get_obj_val(lp));

    brickCount = 0;

    //loop for selecting positive brick in problem
    for (int i = 0; i < numRows; i++) {

        checkLen = 0; /* read bricks until length is reached */
        int j = 0; /* start with brick 0 */
        while (checkLen < length) {

            brickCount++;
            if(values[i * length + j] > 0){
                //printf("Brick x%d is positive.\n",(brickCount));
                if(values[i * length + j]!= glp_get_col_prim(lp,brickCount)){
                    //printf("Take brick x%d!\n", brickCount);
                    solution[i * length + j] = 1;
                }
            }

            checkLen += lengths[i * length + j];
            j++;
        }
    }

    //loop for selecting required bricks so we can legally take our optimal positive bricks
    for (int i = numRows-1; i > 0; i--) { //start at last row to ensure we dont skip any bricks that need to be taken

        checkLenRowAbove = 0; // current length of upper row
        checkLenRowCurrent = 0; // current length of current row
        int j = 0; /* points to brick in upper row */
        int k = 0; /* points to brick in current row */

        while(checkLenRowAbove < length || checkLenRowCurrent < length) {

            //checks to see if brick lenghths are equal. If current row is not equal to lower row need to move row length
            //back to ensure we are attatching all proper bricks to one another. Not going back means we may miss bricks
            //if equal we know we will not miss any
            if(checkLenRowAbove != checkLenRowCurrent){
                j--;
                checkLenRowAbove -= lengths[(i - 1) * length + j];
            }

            //add length of first brick
            //printf("Length brick %d: %d\n",k,lengths[i * length + k]);
            checkLenRowCurrent += lengths[i * length + k];


            //while current row greater then row above, add bricks to constraint, when lesser we know we are done
            //and can move to next brick
            while (checkLenRowCurrent > checkLenRowAbove){
                checkLenRowAbove += lengths[(i - 1) * length + j];
                //printf("Solution of Row:%d Brick:%d is %d\n",(i+1),(k+1),solution[i * length + k]);
                if(solution[i * length + k]==1){
                    solution[(i-1) * length + j] = 1;
                }
                j++;
            }
            k++;
        }
    }

    //fprintf(stdout, "And about to delete it...\n");

    glp_delete_prob(lp); /* release memory used for LP */
}



/***************************************************/
/* YOU SHOULD NOT CHANGE ANYTHING BELOW THIS LINE! */
/***************************************************/

/* int canBeTaken(int row, int brick) checks if the brick at position brick */
/* in row can be taken by checking the bricks immediately above (only those) */
int canBeTaken(int row, int brick) {
    int i; /* loop variables to run over row */
    int left, right; /* beginning and end of brick */
    int pos; /* position that is currently checked; */

    if ( row==0 ) {
        return 1; /* every brick in row 0 can be taken */
    }

    /* run over bricks before brick and add lengths to compute beginning and end */
    for ( i=0, left=0; i<brick; i++ ) {
        left += lengths[row*length+i];
    }
    right = left + lengths[row*length+brick];

    /* run over bricks above and check that all relevant bricks are taken */
    for ( i=0, pos=0; pos<length; i++ ) {
        if ( (pos>=left) && (pos<right) ) {
            /* brick i is above brick; check if taken */
            if ( solution[(row-1)*length+i]==0 ) {
                /* brick not taken => brick cannot be taken */
                return 0;
            }
        }
        /* move to next brick */
        pos += lengths[(row-1)*length+i];
    }
    return 1; /* passed all tests => can be taken */
}

void printSolution(void) {
    int i, j; /* loop variables to go over rows and bricks in each row */
    int value = 0; /* stores the value of the solution */
    int taken; /* flags if at least one brick was taken */

    /* go over each row to print which bricks are taken */
    for ( i=0; i<numRows; i++ ) {
        taken = 0; /* no brick taken yet */
        fprintf(stdout, "Bricks taken from row %d: ", i+1);
        /* go over all bricks to print the ones that are taken */
        for ( j=0; j<length; j++ ) {
            if ( solution[i*length+j]==1 ) {
                taken=1; /* remember that a brick was taken */
                if ( canBeTaken(i, j) && ( lengths[i*length+j]>0 ) ) {
                    /* brick is taken and this is okay */
                    value += values[i*length+j]; /* add up value of taken bricks */
                    fprintf(stdout, "%d (value %d) ", j+1, values[i*length+j]);
                } else {
                    /* brick could not be taken! */
                    fprintf(stdout, "Brick %d (value %d) was taken illegally!\n",
                            j+1, values[i*length+j]);
                    return; /* stop printing solution */
                }
            } else {
                if ( solution[i*length+j]!=0 ) { /* only 0 and 1 are allowed values */
                    fprintf(stdout, "Brick %d (value %d) has an illegal solution value (%d).\n",
                            j+1, values[i*length+j], solution[i*length+j]);
                    return; /* stop printing solution */
                }
            }
        }
        if ( taken ) {
            fprintf(stdout, "\n");
        } else {
            fprintf(stdout, "(none)\n");
        }
    }
    fprintf(stdout, "Value of the solution: %d\n", value);
}

int main(int argc, char **argv) {
    int i; /* used to run over the command line parameters */

    if ( argc<2 ) { /* no command line parameter given */
        fprintf(stderr, "Usage: %s [file1] [file2] [file3] [...]\n"
                        "Where each [file] is the name of a file with a puzzle.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ( argv[1][0]=='-' && argv[1][1]=='d' && argv[1][2]==0 ) {
        /* If the first parameter is -d we activate debug mode. */
        debug=1; /* switch debug mode on */
        fprintf(stdout, "DEBUG: Debug mode activated\n"); /* be explicit about it */
    } else {
        debug=0; /* switch debug mode off */
    }

    for ( i=1+debug; i<argc; i++ ) { /* go over remaining command line parameters */
        if ( readInput(argv[i]) ) { /* try to read file */
            /* returned with error message */
            fprintf(stderr, "%s: Cannot read puzzle with filename %s. Skipping it.\n",
                    argv[0], argv[i]);
        } else { /* input read successfully */
            fprintf(stdout, "Looking at puzzle from %s\n", argv[i]);
            computeSolution(); /* compute a solution */
            printSolution(); /* print the solution including its value */
            /* free memory for next input; avoid a memory leak */
            free(lengths);
            free(values);
            free(solution);
        }
    }
    return EXIT_SUCCESS;
}


/* readInput(*char filename) reads the input and stores it */
/* return value 1 indicates an error; otherwise 0 is returned */
int readInput(char *filename) {
    FILE *fh;
    int i, j; /* loop variables to go over rows and bricks */
    int checkLen; /* add length in each row to check */
    int countSpace; /* two count space */
    char line[4096]; /* one line of text */
    char *inLine; /* pointer to current position in line */

    /* open file for reading */
    if ( ( fh = fopen(filename, "rt") ) == NULL ) {
        return 1;
    }
    /* read line containing number of rows and length */
    if ( fgets(line, 4096, fh)==NULL ) {
        if ( debug ) {
            fprintf(stdout, "DEBUG: Unable to read first line of input file.\n");
        }
        fclose(fh);
        return 1;
    }
    /* extract number of rows and length */
    if ( sscanf(line, "%d %d", &numRows, &length)!=2 ) {
        if ( debug ) {
            fprintf(stdout, "DEBUG: Unable to read number of rows and length.\n");
        }
        fclose(fh);
        return 1;
    }
    /* create arrays to store input and solution */
    if ( (lengths = (int *)malloc(sizeof(int)*numRows*length))==NULL ) {
        if ( debug ) {
            fprintf(stdout, "DEBUG: Unable to allocate memory for lengths.\n");
        }
        fclose(fh);
        return 1;
    }
    if ( (values = (int *)malloc(sizeof(int)*numRows*length))==NULL ) {
        if ( debug ) {
            fprintf(stdout, "DEBUG: Unable to allocate memory for values.\n");
        }
        free(lengths);
        fclose(fh);
        return 1;
    }
    if ( (solution = (int *)malloc(sizeof(int)*numRows*length))==NULL ) {
        if ( debug ) {
            fprintf(stdout, "DEBUG: Unable to allocate memory for solution.\n");
        }
        free(lengths);
        free(values);
        fclose(fh);
        return 1;
    }
    if ( debug ) {
        fprintf(stdout, "DEBUG: rows=%d, length=%d\n", numRows, length);
    }
    /* initialise solution with all 0s */
    memset(solution, 0, sizeof(int)*numRows*length);
    /* read rows one after the other */
    for ( i=0; i<numRows; i++ ) {
        /* read next line */
        if ( fgets(line, 4096, fh)==NULL ) {
            if ( debug ) {
                fprintf(stdout, "DEBUG: Unable to read row %d of input file.\n", i+1);
            }
            free(lengths);
            free(values);
            free(solution);
            fclose(fh);
            return 1;
        }
        inLine=line;
        checkLen=0; /* read bricks until length is reached */
        j=0; /* start with brick 8 */
        while ( checkLen<length ) {
            if ( sscanf(inLine, "%d %d", &lengths[i*length+j], &values[i*length+j])!=2 ) {
                if ( debug ) {
                    fprintf(stdout, "DEBUG: Unable to read next two values.\n");
                }
                free(lengths);
                free(values);
                free(solution);
                fclose(fh);
                return 1;
            }
            if ( debug ) {
                fprintf(stdout, "DEBUG: brick %d in row %d has length=%d, value=%d\n",
                        i, j, lengths[i*length+j], values[i*length+j]);
            }
            if ( lengths[i*length+j]<=0 ) {
                if ( debug ) {
                    fprintf(stdout, "DEBUG: length needs to be positive\n");
                }
                free(lengths);
                free(values);
                free(solution);
                fclose(fh);
                return 1;
            }
            checkLen += lengths[i*length+j];
            /* remove two integers from line and continue */
            for ( countSpace=0; countSpace<2; countSpace++ ) {
                /* skip all whitespace */
                while ( isspace(*inLine) ) {
                    inLine++;
                }
                /* skip one integer */
                while ( !isspace(*inLine ) ) {
                    inLine++;
                }
            }
            /* move to next brick */
            j++;
        }
        if ( checkLen>length ) {
            if ( debug ) {
                fprintf(stdout, "DEBUG: total length %d too long (%d)\n", checkLen, length);
            }
            free(lengths);
            free(values);
            free(solution);
            fclose(fh);
            return 1;
        }
    }
    fclose(fh); /* close file after reading the input */
    return 0; /* signal all went well */
}

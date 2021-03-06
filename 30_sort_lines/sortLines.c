#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//Testcase1 ?
//Testcase2 normal input
//Testcase3 ?
//Testcase4 ?

//This function is used to figure out the ordering
//of the strings in qsort.  You do not need
//to modify it.
int stringOrder(const void * vp1, const void * vp2) {
  const char * const * p1 = vp1;
  const char * const * p2 = vp2;
  return strcmp(*p1, *p2);
}
//This function will sort and print data (whose length is count).
void sortData(char ** data, size_t count) {
  qsort(data, count, sizeof(char *), stringOrder);
}
/*
--takes FILE*, int* to no of lines
--return pointer to array of strings
 */
char** readFile(FILE * f, size_t* n_lines){
  char * line = NULL;//initialize to NULL to avoid garbage value
  size_t sz = 0;
  int c_read = 0;
  size_t n = 0;
  char **array = NULL;
  int check = 0;
  if(f == stdin){
    check = 1;
  }
  //to include newline check = 0
  //newline gets stdin terminated
  while ((c_read = getline(&line,&sz, f)) > check) {
    n++;
    array = realloc(array, n * sizeof(*array));
    array[n-1] = malloc((c_read + 1) * sizeof(**array));
    strncpy(array[n-1],line, c_read + 1);//line size larger tnan array[n-1]
    //also to copy null terminator from line have c_read + 1
    //otherwise it will not be null terminated; cause overflow
  }
  free(line);
  *n_lines = n;
  return array;
}

/*
--takes char** array and size_t no of lines
--prints them and returns nothing
*/
void printArray(char **array, size_t n_lines){
  for(size_t i=0; i < n_lines; i++){
    printf("%s", array[i]);
  }
}

void freeArray(char **array, size_t n_lines){
  for(size_t i=0; i < n_lines; i++){
    free(array[i]);
  }
  free(array);
}

int main(int argc, char ** argv) {
  FILE *f = NULL;
  char **array = NULL;
  size_t n_lines = 0;
  if(argc == 1){
    f = stdin;
    array = readFile(f, &n_lines);
    sortData(array, n_lines);
    printArray(array, n_lines);
    freeArray(array, n_lines);
  }
  else{
    for(int i=1; i < argc; i++){
      f = fopen(argv[i], "r");
      if(f == NULL){
	fprintf(stderr, "Cannot open: %s\n",argv[i]);
	return EXIT_FAILURE;
      }
     array =  readFile(f, &n_lines);
     fclose(f);
     sortData(array, n_lines);
     printArray(array, n_lines);
     freeArray(array, n_lines);
    }
  }
  return EXIT_SUCCESS;
}

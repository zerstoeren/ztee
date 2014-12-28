#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "queue.h"

char *output_filename = NULL;
FILE *output_file = NULL;

//one thread reads in
//one thread writes out and parses
void print_error(){

    printf("You messed it up\n");
    exit(0);
}

int read_in(queue* my_queue){
//read in from stdin and add to back of linked list
    char* input;
    input = malloc(sizeof(char) *1000);
    
    fprintf(stderr, "in read in\n");
    while(fgets(input, 1000, stdin) != NULL){
        fprintf(stderr, "successfully reading in line\n");
        fprintf(stderr, "OUTPUT: %s\n", input);
        push_back(input, my_queue); 
        fprintf(stderr, "finished pushing back\n");
    }
    return 1;
}

void write_out(queue *my_queue){
//take whatever is in the front of the linked list and parse it out to the
//outputfile
    node* temp = pop_front(my_queue);

}

int check_format(){
    //check that the output file is either in a csv form or json form
    int length = strlen(output_filename);

}
int main(int argc, char *argv[]){
   

    int ch;
   char* test1 = "hello this is my data\n"; 
   char* test2 = "hasldkjs\n";
   if (argc > 2){
        print_error();
   }
   while((ch = getopt(argc, argv, "o:")) != -1){
        switch(ch){
            case 'o':
                output_filename = optarg;
                output_file = (FILE *)fopen(output_filename, "w");
                if(output_file == NULL){
                    perror("can't open file");
                    exit(0);
                }
                break;
            default: print_error();
        }
   }

   if(output_file == NULL) output_file = stdout;

    queue* my_queue;
    my_queue = queue_init(&my_queue);
    /*fprintf(stderr, "init\n");
    my_queue = queue_init(&my_queue);
    check_queue(&my_queue);

    fprintf(stderr, "pushback1\n");
    push_back(test1, my_queue);
    check_queue(my_queue);

    fprintf(stderr, "pushback2\n");
    push_back(test2, my_queue);
    check_queue(my_queue);

    fprintf(stderr, "popback1\n");
    pop_front(my_queue);
    check_queue(my_queue);

    fprintf(stderr, "popback2\n");
    pop_front(my_queue);
    check_queue(my_queue);

    fprintf(stderr, "pushback1\n");
    push_back(test2, my_queue);
    check_queue(my_queue);

    fprintf(stderr, "popback1\n");
    pop_front(my_queue);
    check_queue(my_queue);

    fprintf(stderr, "popback2\n");
    pop_front(my_queue);
    check_queue(my_queue);


    fprintf(stderr, "delete\n");
    delete_queue(my_queue);
    check_queue(my_queue);*/
    read_in(my_queue);
    fprintf(stderr, "Finished with reading in\n");
    check_queue(my_queue);
    /*fprintf(stderr, "free\n");
    free(my_queue);
    check_queue(my_queue);*/

    
}

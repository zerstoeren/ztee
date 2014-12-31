#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "queue.h"

char *output_filename = NULL;
FILE *output_file = NULL;
//keeps track if input file is csv
int input_csv = 0;
//keeps track if output file is csv
int output_csv = 0;
//keeps track of the number of fields present in csv (minus 1 because 0 index)
int number_of_fields = 0;
int ip_field = 0;
int ip_field_found = 0;

//one thread reads in
//one thread writes out and parses


void print_error();
//uses fgets to read from stdin and add it to the queue
int read_in(queue* my_queue);

//finds IP in the string of csv and sends it to stdout for zgrab
//you need to know what position is the csv string the ip field is in
//zero indexed
void find_IP(char* my_string);

void write_stdout(char* ip_address);

//writes a csv string out to csv file
void write_out_to_file(queue *my_queue);

//figure out how many fields are present if it is a csv
void figure_out_fields(char* data);

//checks if input is csv or just ip
//only needs to be run one time and on the first node
//because the first node will have the different fields or
//just the ip address`
void input_is_csv(queue *my_queue);

//check that the output file is either in a csv form or json form
int output_file_is_csv();

int main(int argc, char *argv[]){
   

    int ch;
   char* test1 = "hello this is my data\n"; 
   char* test2 = "hasldkjs\n";
   if (argc > 3){
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

   if(output_file == NULL) print_error();

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
    //figure out if input is csv
    //
    fprintf(stderr, "Finished with reading in\n");
    check_queue(my_queue);
    /*fprintf(stderr, "free\n");
    free(my_queue);
    check_queue(my_queue);*/

    
}
void print_error(){
    //includes incorrect output file format
    //
    printf("You messed it up\n");
    exit(0);
}

int read_in(queue* my_queue){
//read in from stdin and add to back of linked list
    char* input;
    input = malloc(sizeof(char) *1000);
    
    fprintf(stderr, "in read in\n");
    while(fgets(input, 1000, stdin) != NULL){
        fprintf(stderr, "before OUTPUT: %s\n", input);
        check_queue(my_queue);
        push_back(input, my_queue); 
        fprintf(stderr, "pushed back\n");
        check_queue(my_queue);
        fprintf(stderr, "finished pushing back\n");
    }
    return 1;
}

void find_IP(char* my_string){
   //finds IP in the string of csv and sends it to stdout for zgrab
   //you need to know what position is the csv string the ip field is in
   //zero indexed
    char *found; 
    char *new_found;
    char *temp;
    int length;
    int i = 0;
    int j = 0;
    if(ip_field ==0){
        found=strchr(my_string, ',');
        length = strlen(my_string) - strlen(found); 
        for(i; i < length; i++){
            temp += my_string[i];
        }
        fprintf(stdout, "%s\n", temp);
    }
    new_found = found;
    for(i; i <= ip_field; i++){
       found = new_found; 
       new_found= strchr(found, ',');
    } 
    length = strlen(found) - strlen(new_found); 
    for(i; i < length; i++){
        temp += my_string[i];
    }
    fprintf(stdout, "%s\n", temp);

}

void write_stdout(char* ip_address){
    fprintf(fprintf, "%s\n", ip_address);
}

void write_out_to_file(queue *my_queue){
//take whatever is in the front of the linked list and parse it out to the
//outputfile
    if(is_empty(my_queue)){
        fprintf(stderr, "Nothing to print out to file because nothing in queue\n");
    }
    node* temp = pop_front(my_queue);
    char *found;
    char *new_found;
    char *temp_string;
    int length;
    int i =0; 
    int j =0;
    if(output_csv){
        if(!input_csv){
            fprintf(stdout, "%s\n", temp->data);
        }else{
           //need to parse out by comma 
            found=strchr(temp->data, ',');
            length = strlen(temp->data) - strlen(found); 
            for(i; i < length; i++){
                temp_string += temp->data[i];
            }
            fprintf(output_file, "%s\n", temp_string);
            i = 0;
            for(j; j < ip_field; j++){
                new_found =strchr(found, ',');
                length = strlen(found) - strlen(new_found);
                for(i; i < length+i; i++){
                   temp_string +=found[i]; 
                }
                fprintf(output_file, "%s\n", temp_string);
           }
        }
    }else{
       //output as json 
    }
    
}

void figure_out_fields(char* data){
   //number_of_fields = number of commas + 1 
   //check each substring if it is the same as saddr
   //set ip_field
   char *temp;
   char* saddr = "saddr";
   char* found;
   char *new_found;
   int i =0;
   int count = 0;
   int length;
   found = strchr(data, ',');
   length = strlen(data) - strlen(found); 
   for(i; i < length; i++){
       temp += data[i];
   }

   if(!strcmp(temp, saddr)){    
       ip_field = 0;
       ip_field_found = 1;
   }
   while(found){ 
       new_found = strchr(found, ',');
       if(!ip_field_found){
           length = strlen(found) - strlen(new_found); 
           temp = NULL;
           //what to set i to
           for(i; i < length; i++){
               temp += found[i];
           }

           if(!strcmp(temp, saddr)){    
               ip_field = count;
               ip_field_found = 1;
           }
       }
       found = new_found;
       new_found = NULL;
       count++;
   }
   number_of_fields = count++;
}

void input_is_csv(queue *my_queue){
    //checks if input is csv or just ip
    //only needs to be run one time and on the first node
    //because the first node will have the different fields or
    //just the ip address`
    node *temp = malloc(sizeof(node));  
    temp = get_front(my_queue);
    
    char *found;
    found = strchr(temp->data, ',');
    if(!found){
        input_csv = 0;     
    }else{
        //if the input is a csv write out the first line to the outputfile
        input_csv = 1;
        pop_front(my_queue);
        figure_out_fields(temp->data);
        fprintf(output_file, "%s\n", temp->data);
    }
}

int output_file_is_csv(){
    //check that the output file is either in a csv form or json form
    int length = strlen(output_filename);
    char *end_of_file = output_filename[length-3];
    char *csv = "csv";
    char *json = "json";
    if(!strcmp(end_of_file, csv) || !strcmp(end_of_file, json)){
        print_error();
    }
    if(!strcmp(end_of_file, csv)) output_csv = 1;
    if(!strcmp(end_of_file, json)) output_csv = 0;
}

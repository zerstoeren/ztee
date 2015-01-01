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

//writes a csv string out to csv file
void write_out_to_file(char* data);

//figure out how many fields are present if it is a csv
void figure_out_fields(char* data);

//checks if input is csv or just ip
//only needs to be run one time and on the first node
//because the first node will have the different fields or
//just the ip address`
void input_is_csv(queue *my_queue);

//check that the output file is either in a csv form or json form
void output_file_is_csv();

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
                fprintf(stderr, "found filename\n");
                fprintf(stderr, "%s\n", output_filename);
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
    //BEFORE READING IN REST OF QUEUE
    //figure out if input is csv
    //find where ip field is 
    //AFTER
    //take ip and print to stdout
    //write to output file is there is one
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
    fprintf(stderr, "out of while loop\n");
    input_is_csv(my_queue);
    output_file_is_csv();
    fprintf(stderr, "input is csv?: %i\n", input_csv);
    fprintf(stderr, "what is ip_field? %i\n", ip_field);
    while(!is_empty(my_queue)){
        node* temp = malloc(sizeof(node));
        temp = pop_front(my_queue);
        fprintf(stderr, "about to find IP\n");
       find_IP(temp->data);
        fprintf(stderr, "found IP\n");
       write_out_to_file(temp->data);
       //clean up node 
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
    if(!input_csv){
       fprintf(stdout, "%s\n", my_string);
       fprintf(stderr, "found ip, exiting first if\n");
       return;
    }
    found = strdup(my_string);
    new_found = strchr(found, ',');
    fprintf(stderr, "beofre first for statement\n");
    for(i; i <= number_of_fields; i++){
        if(i == ip_field && new_found){
            length = strlen(found) - strlen(new_found); 
            //what to set i to
            temp = malloc(sizeof(char*)*(length+1));
            strncpy(temp, found, length);
            temp[length] = '\0';
            fprintf(stdout, "%s", temp);
            return;
        }else if(i == ip_field){
            fprintf(stdout, "%s", found); 
            return;
        }
        found = new_found+1;
        new_found = strchr(found, ',');

    }
    fprintf(stdout, "%s\n", temp);

}
void write_out_to_file(char* data){
//take whatever is in the front of the linked list and parse it out to the
//outputfile
    char *found;
    char *new_found;
    char *temp_string;
    int length;
    int i =0; 
    int j =0;
    if(output_csv){
        /*if(!input_csv){
            fprintf(stdout, "%s\n", data);
        }else{
           //need to parse out by comma 
            found=strchr(data, ',');
            length = strlen(data) - strlen(found); 
            for(i; i < length; i++){
                temp_string += data[i];
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
        }*/
        fprintf(output_file, "%s", data);
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
   fprintf(stderr, "in figure_out_fields\n");
   /*found = strchr(data, ',');
   length = strlen(data) - strlen(found); 
   fprintf(stderr, "found: %s\n", found);
   temp = malloc(sizeof(char*)*(length+1));
   strncpy(temp, data, length);
   temp[length+1] = '\0';
    
   fprintf(stderr, "temp: %s\n", temp);
   fprintf(stderr, "saddr: %s\n", saddr);
   if(!strncmp(temp, saddr, 5)){    
       ip_field = 0;
       fprintf(stderr, "set ip_field to %i\n", ip_field);
       ip_field_found = 1;
   }*/
   found = data;
   found = found+1;
   fprintf(stderr, "found right before while loop %s\n", found);
   new_found = strchr(found, ',');
   fprintf(stderr, "new_found %s\n", new_found);
   while(new_found){ 
       fprintf(stderr, "found right at startwhile loop %s\n", found);
       fprintf(stderr, "new_found %s\n", new_found);
       if(!ip_field_found){
           length = strlen(found) - strlen(new_found); 
           temp = NULL;
           //what to set i to
           temp = malloc(sizeof(char*)*(length+1));
           strncpy(temp, found, length);

           fprintf(stderr, "temp: %s\n", temp);
           if(!strncmp(temp, saddr, 5)){    
               ip_field = count;
               fprintf(stderr, "set ip_field to %i\n", ip_field);
               ip_field_found = 1;
           }
       }
       found = new_found+1;
       new_found = NULL;
       count++;
       new_found = strchr(found, ',');
   }
   number_of_fields = count++;
   fprintf(stderr, "number of fields %i\n", number_of_fields);
   if(!ip_field_found){
       fprintf(stderr, "found: %s\n", found);
       if(!strncmp(found, saddr, 5)){    
           ip_field = number_of_fields;
           fprintf(stderr, "set ip_field to %i\n", ip_field);
           ip_field_found = 1;
       }
   }
}

void input_is_csv(queue *my_queue){
    //checks if input is csv or just ip
    //only needs to be run one time and on the first node
    //because the first node will have the different fields or
    //just the ip address`
    fprintf(stderr, "in input_is_csv\n");
    node *temp = malloc(sizeof(node));  
    temp = get_front(my_queue);
    
    char *found;
    found = strchr(temp->data, ',');
    fprintf(stderr, "found comman\n");
    fprintf(stderr, "%s\n", found);
    if(!found){
        input_csv = 0;     
    }else{
        //if the input is a csv write out the first line to the outputfile
        input_csv = 1;
        pop_front(my_queue);
        figure_out_fields(temp->data);
        fprintf(output_file, "%s", temp->data);
    }
    fprintf(stderr, "out of input_is_csv\n");
}

void output_file_is_csv(){
    //check that the output file is either in a csv form or json form
    fprintf(stderr, "outputfile is csv\n");
    int length = strlen(output_filename);
    char *end_of_file = malloc(sizeof(char*) *4);
    strncpy(end_of_file, output_filename+(length - 3), 3);
    end_of_file[4] = "\0"; 
    fprintf(stderr, "%s\n", end_of_file);
    char *csv = "csv\n";
    char *json = "jso\n";
    if(!end_of_file) fprintf(stderr, "WTF\n");
    if(!strncmp(end_of_file, csv, 3) && !strncmp(end_of_file, json, 3)){
        print_error();
    }
    if(!strncmp(end_of_file, csv, 3)) output_csv = 1;
    if(!strncmp(end_of_file, json, 3)) output_csv = 0;
    fprintf(stderr, "output_csv: %i\n", output_csv);
}

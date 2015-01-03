#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include "queue.h"

char *output_filename = NULL;
char *monitor_filename = NULL;
FILE *output_file = NULL;
FILE *monitor_output_file = NULL;
//keeps track if input file is csv
pthread_mutex_t queue_size_lock = PTHREAD_MUTEX_INITIALIZER;
int input_csv = 0;
//keeps track if output file is csv
int output_csv = 0;
//set to 1 if this flag is set
int find_success_only = 0;
//keep track of what field the is_successful field is
int success_field = 0;
//tells whether the is_successful field has been found or not
int success_found = 0;
//keeps track of the number of fields present in csv (minus 1 because 0 index)
int number_of_fields = 0;
int ip_field = 0;
int ip_field_found = 0;
int monitor = 0;
int done = 0;
int process_done = 0;
int total_read_in = 0;
int read_in_last_sec =0;

pthread_t threads[3];

//one thread reads in
//one thread writes out and parses


void process_queue(queue* my_queue);

void print_error();
//uses fgets to read from stdin and add it to the queue
int read_in(queue* my_queue);
//does the same as find UP but finds only successful IPs, determined by the 
//is_successful field and flag
void find_successful_IP(char* my_string);
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
//throws error is it is not either
//NOTE: JSON OUTPUT NOT IMPLEMENTED
void output_file_is_csv();

void print_thread_error();

void monitor_ztee(queue *my_queue);
int main(int argc, char *argv[]){
   

    int ch = 0;
    int index;

   /*if (argc < 2){
        print_error();
   }*/
   const struct option longOpts[] = {
        {"find_success_only", no_argument, 0, 'f'},
        {"output-file", required_argument, 0, 'o'},
        {"monitor", required_argument, 0, 'm'},
        {0,0,0,0}
   };
   while(ch != -1){
       ch=getopt_long(argc, argv, "fo:m:", longOpts, &index);
       switch(ch){
            case 'f':
                find_success_only = 1;
                break;
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
            case 'm':
                monitor = 1;
                monitor_filename = optarg;
                fprintf(stderr, "found filename\n");
                monitor_output_file = (FILE *)fopen(output_filename, "w");
                if(monitor_output_file==NULL){
                    perror("can't open file");
                    exit(0);
                }
                break;
            case -1:
                break;
        }
   }

   if(output_file == NULL) print_error();

    queue* my_queue;
    my_queue = queue_init(&my_queue);
    //BEFORE PROCESSING REST OF QUEUE
    //figure out if input is csv
    //find where ip field is 
    //THEN
    //take ip and print to stdout
    //write to output file if there is one
    /*int x = pthread_create(&read_in_thread, NULL, read_in, my_queue);
    if(x){
        print_thread_error();
        exit(0);
    }*/
    
    pthread_t *read_thread, process_thread, monitor_thread;
    //begins reading in and adding to queue
    int y = pthread_create(&threads[0], NULL, read_in, my_queue);
    char* read = "read thread\n";
    if(y){
        print_thread_error(read);
        exit(1);
    }

    input_is_csv(my_queue);
    output_file_is_csv();


    int a = pthread_create(&threads[1], NULL, process_queue, my_queue);
    char* process = "process thread\n";
    if(a){
        print_thread_error(process);
        exit(1);
    }
    if(monitor){
        int z = pthread_create(&threads[2], NULL, monitor, my_queue);
        char* monitor_thread = "monitor thread\n";
        if(z){
            print_thread_error(monitor_thread);
            exit(1);
        }
        fprintf(stderr, "created monitor thread\n");
    }
    int g;
    for(g=0; g < 3; g++){
       pthread_join(threads[g], NULL); 
    }
    fprintf(stderr, "about to exit main\n"); 
}

void process_queue(queue* my_queue){
    fprintf(stderr, "processing queue\n");
    fprintf(stderr, "is empty\n", is_empty(my_queue));

    while(!is_empty(my_queue) || !done){
        node* temp = malloc(sizeof(node));
        temp = pop_front(my_queue);
        fprintf(stderr, "about to find IP\n");
        if(!input_csv){
            fprintf(stdout, "%s\n", temp->data);
            fprintf(stderr, "found ip, exiting first if\n");
        }else if(find_success_only){
            find_successful_IP(temp->data);
        }else{
            find_IP(temp->data);
        }
        fprintf(stderr, "found IP\n");
       write_out_to_file(temp->data);
       //clean up node 
       free(temp);
    }
    fprintf(stderr, "processing done\n");
    if(monitor){
        //pthread_cancel(&threads[2]);
    }
    process_done = 1;
    free(my_queue);
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
        pthread_mutex_lock(&queue_size_lock);
        total_read_in++;
        read_in_last_sec++;
        pthread_mutex_unlock(&queue_size_lock);
    }
    fprintf(stderr, "out of while loop\n");
    done = 1;

    return 1;
}

void find_successful_IP(char* my_string){

    char *found; 
    char *new_found;
    char *this_IP;
    int length;
    int i = 0;
    int j = 0;
    char* is_this_IP_successful = 0;
    int is_successful = 0;
    found = strdup(my_string);
    new_found = strchr(found, ',');
    fprintf(stderr, "beofre first for statement in success\n");
    for(i; i <= number_of_fields; i++){
            fprintf(stderr, "beginning of while found %s\n", found);
        if(i == success_field && new_found){
            fprintf(stderr, "first if statement\n");
            length = strlen(found) - strlen(new_found); 
            //what to set i to
            is_this_IP_successful = malloc(sizeof(char*)*(length+1));
            strncpy(is_this_IP_successful, found, length);
            is_successful = atoi(is_this_IP_successful);
            fprintf(stderr, "found %s\n", found);
            fprintf(stderr, "is ip successfule? %i\n", is_successful);
            if(!is_successful) return;
        }else if(i == success_field){
            fprintf(stderr, "second if statement\n");
           is_successful = atoi( found );
            fprintf(stderr, "found %s\n", found);
           fprintf(stderr, "is ip successfule? %i\n", is_successful);
           if(!is_successful) return;
           //is_this_IP_successful = atoi(is_this_IP_successful);
        }
        if(i == ip_field && new_found){
            fprintf(stderr, "third if statement\n");
            length = strlen(found) - strlen(new_found); 
            //what to set i to
            this_IP = malloc(sizeof(char*)*(length+1));
            strncpy(this_IP, found, length);
            this_IP[length] = '\0';
        }else if(i == ip_field){
            fprintf(stderr, "fourth if statement\n");
            this_IP = found;
        }
        fprintf(stderr, "about to increment\n");
        fprintf(stderr, "found %s\n", found);
        fprintf(stderr, "new_found %s\n", new_found);
        if(new_found) found = new_found+1;
        new_found = strchr(found, ',');
    }
    fprintf(stdout, "out of while \n");
    fprintf(stdout, "THIS IP: %s\n", this_IP);
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
    found = strdup(my_string);
    new_found = strchr(found, ',');
    fprintf(stderr, "beofre first for statement\n");
    for(i; i <= number_of_fields; i++){
        if(i == ip_field && new_found){
            temp = NULL;
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
        if(new_found) found = new_found+1;
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
    fprintf(output_file, "%s", data);
    /*if(output_csv){
        if(!input_csv){
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
        }
        fprintf(output_file, "%s", data);
    }else{
       //output as json 
    }*/
    
}

void figure_out_fields(char* data){
   //number_of_fields = number of commas + 1 
   //check each substring if it is the same as saddr
   //set ip_field
   char *temp;
   char* saddr = "saddr";
   char* found;
   char* is_successful = "success\0";
   char *new_found;
   int i =0;
   int count = 0;
   int length;
   fprintf(stderr, "in figure_out_fields\n");
   found = data;
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
           //will find the success_field regardless
       }
       
       if(!success_found){
           if(!strncmp(temp, is_successful, 7)){
               success_found = 1;
               success_field = count;
               fprintf(stderr, "set success_field to %i\n", success_field);
           }
       }
       found = new_found+2;
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
   if(!success_found){
       if(!strncmp(found, is_successful, 7)){
           success_found = 1;
           success_field = number_of_fields;
           fprintf(stderr, "set success_field to %i\n", success_field);
       }
   }
}

void input_is_csv(queue *my_queue){
    //checks if input is csv or just ip
    //only needs to be run one time and on the first node
    //because the first node will have the different fields or
    //just the ip address`
    fprintf(stderr, "in input_is_csv\n");
    while(is_empty(my_queue));
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
        node *to_delete = malloc(sizeof(node*));
        input_csv = 1;
        to_delete = pop_front(my_queue);
        figure_out_fields(temp->data);
        fprintf(output_file, "%s", temp->data);
        free(to_delete);
    }
    fprintf(stderr, "out of input_is_csv\n");
    output_file_is_csv();
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

void print_thread_error(char* string){
    fprintf(stderr, "Could not create thread %s\n", string);
    return;
}

void monitor_ztee(queue* my_queue){
    fprintf(stderr, "in monitor code\n");
    fprintf(monitor_output_file,"Total_read_in, read_in_last_sec, read_per_sec_avg, buffer_current_size, buffer_avg_size\n");
    int read_per_sec_avg = 0, buffer_current_size = 0, buffer_avg_size = 0;
    total_read_in = 0;
    double count_seconds = 1;
    while(!process_done){
        read_in_last_sec = 0;
        buffer_current_size = 0;
        //sleep(60);
        get_size(my_queue, &buffer_current_size); 
        buffer_avg_size = (buffer_current_size + buffer_avg_size)/count_seconds;
        
        pthread_mutex_lock(&queue_size_lock);
        read_per_sec_avg = (read_per_sec_avg + read_in_last_sec)/count_seconds;
        fprintf(monitor_output_file, "%i,%i,%i,%i,%i\n", total_read_in, read_in_last_sec,
                read_per_sec_avg, buffer_current_size, buffer_avg_size);
        pthread_mutex_unlock(&queue_size_lock);
        //unlock
        count_seconds++;

    }
    pthread_exit(NULL);
}

//
//
// Chicago crime look up program. //
// <<Srinivas Lingutla>>
// << Mac OS X 10.11.5 , XCODE>> // U. of Illinois, Chicago
// CS251, Fall 2016
// HW #9
//
//  main.c
//  Homework 9 - 251
//
//  Created by Srinivas Lingutla on 9/15/16.
//  Copyright © 2016 Srinivas Lingutla. All rights reserved.
//

#include "avl.h"


//****************************************************************************************
//hashing function
int hashing_Index(char *case_id, int size){
    
    int number = atoi(&case_id[2])+ case_id[0]*31;;
    int hash = number + ( 1000000 * (case_id[1]%65)) ;
    hash = abs(hash);
    
    return hash % size;
}

//****************************************************************************************

//creating a new node for the linked list to go in the hash table
NODE * Create_new_Node(char case_id[15], char date[40],   char IUCR[6], char arrest[6],  char domestic[6], int community) {
    
    struct node *new_Node;
    new_Node = (struct node *) mymalloc(sizeof(struct node));
    
    //assign the new values from the file
    strcpy(new_Node->case_id, case_id);
    strcpy(new_Node->date, date);
    strcpy(new_Node->IUCR, IUCR);
    strcpy(new_Node->arrest, arrest);
    strcpy(new_Node->domestic, domestic);
    new_Node->community = community;
    
    //equate the next node to null
    new_Node->next = NULL;
    return new_Node;
}

//****************************************************************************************

//Function to input the new nodes from  the file
HASH *Insert_in_Hash( char case_id[15], char date[40],   char IUCR[6], char arrest[6],  char domestic[6], int community , HASH *hashtable , int *collisions, int size_hashTable) {
    
    //create a new linked list with the desired values
    struct node *newnode = Create_new_Node(case_id , date , IUCR , arrest , domestic , community);
    
    //get the new hashing index
    int hashIndex = hashing_Index(case_id, size_hashTable);
    
    //check if empty hashing index is empty and point it to the linked list
    if (hashtable[hashIndex].head == NULL) {
        hashtable[hashIndex].head = newnode;
        return hashtable;
    }
    
    //else if not empty, place the new linked list before the others
    newnode->next = (hashtable[hashIndex].head);
    hashtable[hashIndex].head = newnode;
    (*collisions)++; // increment the number of collisions
    
    
    
    return hashtable;
}



//****************************************************************************************

//Function to iterate through the hash table and insert unknown IUCR codes into the avl tree
AVLNode *CRIME_IN_AREA(AVLNode *root, char *IUCR, int area_code, AVLElementType *unknown_node) {
    //obtain a pointer to the avl tree
    AVLNode *node = root;

    while (node != NULL){  //a while loop to traverse the list
        
        if (strcmp(IUCR , node->value.IUCR) == 0)
            break;
        else if (strcmp(IUCR , node->value.IUCR) < 0)
            node = node->left;
        else if(strcmp(IUCR , node->value.IUCR) > 0)
            node = node->right;
    }
    //copy over the new node to avl tree as unknown
    if(node == NULL){
        strcpy(unknown_node->IUCR, IUCR);
        strcpy(unknown_node->primary, "UNKNOWN");
        strcpy(unknown_node->secondary, "");
        Insert(root, *unknown_node);
        CRIME_IN_AREA(root, IUCR, area_code, unknown_node);
        return root;
    }
    else{
        //if the node is already present, increment the crimes in that particular area code
        node->crimes[area_code]++;
        node->count++;
    }
    
    return root;
}

//****************************************************************************************

//Function to build the hastable and get the lines from the crimes filename
HASH *Build_hashtable(char *filename , int *size_of_hash, int *total_crimes, AVLNode *binary_tree, int *crimes_per_area, AVLElementType *value)
{
    
    //initialise the root
    HASH *hashtable = NULL;
    FILE *input;
    char str[400];  //for the input line
    
    input = fopen(filename, "r"); // open the file:
    if (input == NULL) //if unable to open
    {
        printf("**Error: unable to open '%s'\n", filename);
        exit(-1);
    }
    
    //get the first line for the years
    fgets(str, 350, input);
    
    int first_year = atoi(&str[0]);
    int second_year = atoi(&str[5]);
    //calculate the memory needed to allocate
    int memory_needed = (second_year - first_year +1)*1500000 ;
    *size_of_hash = memory_needed;
    hashtable = (struct hash *) mymalloc(sizeof(struct hash) * memory_needed);
    
    printf(">> Years:\t %d..%d\n", first_year, second_year);
    
    if (hashtable == NULL) {
        printf("** Error: malloc failed to allocate hash table (%d).\n", memory_needed);
        exit(-1);
    }
    //get the second line of input to disregard the data
    fgets(str, 350, input);
    
    //Variables to store the values from the files
    char case_id[15];
    char date[40];
    char IUCR[6];
    char arrest[6];
    char domestic[6];
    int community;
    int collisions = 0;
    
    //while loop to run till the file runs out
    while (fgets(str, 350, input))
    {
        //get line by line and break it into pieces
        char *token;
        token = strtok(str, ",");
        strcpy(case_id, token);
        
        token = strtok(NULL, ",");
        strcpy(date, token);
        
        
        token = strtok(NULL, ",");
        strcpy(IUCR, token);
        
        token = strtok(NULL, ",");
        strcpy(arrest, token);
        
        token = strtok(NULL, ",");
        strcpy(domestic, token);
        
        token = strtok(NULL, ",");
        token = strtok(NULL, ",");
        token = strtok(NULL, ",");
        
        token = strtok(NULL, ",");
        community = atoi(token);
        crimes_per_area[community]++;
        
        token = strtok(NULL, ",");
        (*total_crimes)++;
        
        //insert the new data into the hash table
        hashtable = Insert_in_Hash(case_id, date , IUCR , arrest , domestic , community, hashtable, &collisions , memory_needed);
        
        //insert the crime in th avl if the node is not already present
        CRIME_IN_AREA(binary_tree, IUCR, community, value);
        
    }
    
    //printing data
    printf(">> # crimes:\t %d\n", *total_crimes);
    printf(">> ht size:\t %d\n", memory_needed);
    printf(">> COLLISIONS:\t %d\n\n", collisions);
    
    
    fclose(input);
    return hashtable;
    
}



//****************************************************************************************

//Function to get and store the reviews
void ParseAreas (FILE *file , char code_area[][100], AVLNode *BinaryTree){
    
    //variables to store the values from the file
    char str[100];
    int id;
    
    //get the first line
    fgets(str, 350, file);
    
    //while loop to run till the file runs out
    while (fgets(str, 350, file)){
        
        
        //Getting the value
        char *token;
        token = strtok(str, ",");
        id = atoi(str);
        
        token = strtok(NULL, ",");
        
        char *pos;
        if ((pos=strchr(token, '\n')) != NULL)
            *pos = '\0';
        
        strcpy(code_area[id], token);
        
        
    }
    //return the number of reviews
    fclose(file);
}


//****************************************************************************************

//Function to traverse the AVL tree
AVLNode *BT_Search(AVLNode *root, char *IUCR) {
    AVLNode *node = root;
    
    while (node != NULL){  //a while loop to traverse the list
        if (strcmp(IUCR , node->value.IUCR) == 0)
            break;
        else if (strcmp(IUCR , node->value.IUCR) < 0)
            node = node->left;
        else if(strcmp(IUCR , node->value.IUCR) > 0)
            node = node->right;
    }
    return node;
}

//****************************************************************************************
//Function to search a hashindex in the hashtable and print out the required details
void searchInHash(char *case_id , HASH *hashTable , char areaCode[][100], int size_of_hash, AVLNode *root, int *Number_of_Crimes, int total_crimes) {
    //get the hash index
    int hashIndex = hashing_Index(case_id, size_of_hash);
    printf(">> hash index:  %d <<\n", hashIndex);
    int found = 0;
    struct node *data;
    //if incorrect hash index, then give error
    if (hashTable[hashIndex].head == NULL) {
        printf("Case-ID invalid\n");
        return;
    }
    else
        data = hashTable[hashIndex].head; //else get the head of the linked list

    while (data != NULL) {
        if (strcmp(data->case_id, case_id)==0) {
            //Get the IUCR Node from the AVL tree
            AVLNode *IUCR_NODE = BT_Search(root, data->IUCR);
                //print out the required details
            printf("%s: \n", data->case_id);
            printf("   date/time:  %s\n", data->date);
            printf("   city area: %d =>  %s\n", data->community, areaCode[data->community]);
            printf("   IUCR code:  %s => %s %s\n", IUCR_NODE->value.IUCR, IUCR_NODE->value.primary , IUCR_NODE->value.secondary);
            printf("   arrested:  %s\n", data->arrest);
            printf("   domestic violence:  %s\n", data->domestic);
            
            printf("\nAREA:  %d => %s\n", data->community, areaCode[data->community]);
            printf("   # of crimes in area:  %d\n", Number_of_Crimes[data->community]);
            printf("   %% of Chicago crime:\t%lf%%\n", 100 * (float)Number_of_Crimes[data->community] / total_crimes);
            
           
            printf("\nCRIME: %s => %s: %s\n", IUCR_NODE->value.IUCR, IUCR_NODE->value.primary , IUCR_NODE->value.secondary);
            printf("   # of THIS crime in area:\t%d\n", IUCR_NODE->crimes[data->community]);
            printf("   %% of THIS crime in area:\t%lf%%\n", 100 * (float)IUCR_NODE->crimes[data->community]/Number_of_Crimes[data->community]);
            printf("   %% of THIS crime in Chicago:\t%lf%%\n",100 * (float)IUCR_NODE->count/total_crimes);
            printf("-------------------------------------------------------------------------\n");
            
            //if found
            found = 1;
            break;
        }
        //else go to the next linked list node
        data = data->next;
    }
    //if not found
    if (!found)
        printf("Case-ID invalid\n");
    return;
}


//****************************************************************************************

//interactive search of stations for the user.
int search_crimes ( HASH *hashtable, char code_area[][100] , int size_of_hash, AVLNode *root, int *Number_of_Crimes, int total_crimes){
    
    while(1){
        
        char name[255];
        
        
        printf("\n>> Please enter case number:  ");
        //getting the user input
        fgets(name, sizeof(name), stdin);
        
        //removing the tailing newline character from fgets
        size_t ln = strlen(name) - 1;
        if (name[ln] == '\n')
            name[ln] = '\0';
        
        //checking if user pressed enter
        if(name[0] == '\0')
            break;
        
        if(strlen(name) < 8){
            printf("Please enter a case number in correct format, try again\n");
            continue;
        }
        //search the case id in hash table
        searchInHash(name, hashtable, code_area , size_of_hash, root, Number_of_Crimes, total_crimes);
    }
    return 0;
}


//****************************************************************************************

//Function to build and get the lines from the codes filename and store the lines into a AVL tree
AVLNode *BUILD_AVL_TREE(char *filename, AVLElementType *value)
{
    //initialise the root
    AVLNode *root = NULL;
    FILE *input;
    char str[300];  //for the input line
    
    input = fopen(filename, "r"); // open the file:
    if (input == NULL) //if unable to open
    {
        printf("**Error: unable to open '%s'\n", filename);
        exit(-1);
    }
    
    //get the first line for disregarding
    fgets(str, 350, input);
    
    //while loop to run till the file runs out
    while (fgets(str, 350, input))
    {
        //get line by line and break it into pieces
        char *token;
        token = strtok(str, ",");
        strcpy(value->IUCR, str);
        
        token = strtok(NULL, ",");
        strcpy(value->primary, token);
        
        token = strtok(NULL, ",");
        
        char *pos;
        if ((pos=strchr(token, '\n')) != NULL)
            *pos = '\0';
        
        strcpy(value->secondary, token);
        
        root = Insert(root, *value);
        
    }
    fclose(input);
    return root;
    
}

//****************************************************************************************
//function to free the AVL tree through recursive
void freeTree(AVLNode * node)
{
    if (node == NULL) return;

    freeTree(node->left);
    freeTree(node->right);
    
    myfree(node);
}

//****************************************************************************************


int main() {
    
    HASH *hashtable = NULL;
    
    AVLNode *binary_tree = NULL;
    
    AVLElementType *value = (struct AVLElementType*) mymalloc(sizeof(struct AVLElementType));
    
    binary_tree = BUILD_AVL_TREE("Codes.csv", value);
    
    FILE* file = fopen("Areas.csv", "r");
    if (!file)
        return -1; //error if the file didnt open
    int size_of_hash = 0;
    
    //displaying the details
    printf("** Chicago Crime Lookup Program **\n\n\n");
    printf(">> Reading:\t 'Areas.csv'\n");
    printf(">> Reading:\t 'Crimes.csv'\n");
    
    
    char code_area[100][100];
    int Number_of_crimes[100];
    int l = 0;
    for(l = 0; l<100; l++){
        Number_of_crimes[l] = 0;
    }
    
    ParseAreas(file, code_area, binary_tree);
    
    int Total_crimes = 0;
    hashtable = Build_hashtable("Crimes.csv" , &size_of_hash, &Total_crimes, binary_tree, Number_of_crimes, value);
    
    search_crimes(hashtable, code_area, size_of_hash, binary_tree, Number_of_crimes, Total_crimes);
    
    printf("\n**Done**\n\n");
    
    mymem_stats();
    printf("** Freeing memory...\n");
    
    //free the array and the linked lists
    int i = 0;
    struct node *curr = NULL;
    while (i <= size_of_hash) {
        
        if(hashtable[i].head == NULL){
            i++;
        }
        else{
            while(hashtable[i].head != NULL){
                curr = hashtable[i].head;
                hashtable[i].head = hashtable[i].head->next;          // advance head to next element.
                myfree (curr);
            }
            i++;
        }
    }
    
    myfree(hashtable);
    
    freeTree(binary_tree);

    myfree(value);
   
    mymem_stats();
    printf("\n\n");
    
    return 0;
}
//****************************************************************************************
//THE END...

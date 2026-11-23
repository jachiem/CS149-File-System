#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_OF_SECTORS 100 
#define SECTOR_SIZE 512
#define ENTRIES_PER_DIR 25
#define DATA_SIZE 504
#define ROOT_DIR_SECTOR 0
#define MAX_NAME_LENGTH 8


int curDir = 0;

struct {
    int isOpen;
    int dirSec, dirIdx;   // where the file's row is
    char mode;            // I, O, or U
    int pos;              // cursor
} openInfo;

//below is the given code for disk sectors (?)
/*The first structure represents a directory sector. 
The second represents a sector containing the contents 
of a regular file. The sector array must be declared 
after the union definition.*/
union block { 
    struct { 
        int back; // link to the previous sector
        int frwd; // the link to the next sector
        int free; // unused 
        struct { 
            char type; 
            char name[9]; 
            int link; 
            short size; 
        } 
        dir[25]; 
    } 
    directory; 
    struct { 
        int backf; 
        int frwdf; 
        char userdata[DATA_SIZE]; 
    } 
    data; 
};

/* acts as main disk */
union block disk[NUM_OF_SECTORS];


struct nodeType {
    int info; //free box
    struct nodeType *link; //next item 
};

//pointer for top of sector
struct nodeType *freeList = NULL; 

//make a list of 99 EMPTY blocks.
void buildList(void){
    for (int s=99; s>=1; s--){
        struct nodeType *n = malloc(sizeof *n); 
        n->info = s;
        n->link = freeList;
        freeList = n;
    }
}

//remove a blocks from EMPTY when it's being used
int Remove(void){
    if(freeList ==NULL) return -1;
    struct nodeType *n = freeList;
    int s = n->info; 
    freeList = n->link; 
    free(n);
    return s; 
}


//insert puts empty blocks back after delete 
void Insert(int s){
    struct nodeType *n = malloc(sizeof *n);
    n->info = s;
    n->link = freeList; 
    freeList = n; 
}

//blanket folder in box s 
void initDir(int s)          
{
    memset(&disk[s], 0, sizeof disk[s]);
    disk[s].directory.back = -1;
    disk[s].directory.frwd = 0;
    disk[s].directory.free = 25;  //25 free slots
    for (int i = 0; i < 25; i++) {
        disk[s].directory.dir[i].type = 'F';
        disk[s].directory.dir[i].link = 0;
    }
}



static void initializeDisk(void)
{
    initDir(0);                       
    disk[0].directory.back = -1;
    disk[0].directory.frwd = -1;
    buildList();                     
}

static int findFreeDirectoryEntry(void)
{
    for (int i = 0; i < ENTRIES_PER_DIR; i++) {
        if (disk[ROOT_DIR_SECTOR].directory.dir[i].type == 'F') {
            return i;
        }
    }
    return -1;
}

static int findDirectoryEntry(const char name[])
{
    for (int i = 0; i < ENTRIES_PER_DIR; i++) {
        if (disk[ROOT_DIR_SECTOR].directory.dir[i].type != 'F' &&
            strcmp(disk[ROOT_DIR_SECTOR].directory.dir[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}




/* --------------------------------------- FILE FUNCTIONS ---------------------------------------*/
void destroyList(void){
    while (freeList != NULL) {
        struct nodeType *n = freeList;
        freeList = freeList->link;
        free(n);
    }
}


void createFile(char type, const char name[])
{
    if (name == NULL || name[0] == '\0') {
        fprintf(stderr, "Error: file name cannot be empty.\n");
        return;
    }
    if (strlen(name) > MAX_NAME_LENGTH) {
        fprintf(stderr, "Error: file name must be at most %d characters.\n",
                MAX_NAME_LENGTH);
        return;
    }
    if (findDirectoryEntry(name) != -1) {
        fprintf(stderr, "Error: file '%s' already exists.\n", name);
        return;
    }

    int dir_index = findFreeDirectoryEntry();
    if (dir_index == -1) {
        fprintf(stderr, "Error: root directory is full.\n");
        return;
    }

    int data_sector = Remove();
    if (data_sector == -1) {
        fprintf(stderr, "Error: disk is full.\n");
        return;
    }

    disk[0].directory.dir[dir_index].type = type;
    strcpy(disk[0].directory.dir[dir_index].name, name);
    disk[0].directory.dir[dir_index].link = data_sector;
    disk[0].directory.dir[dir_index].size = 0;


    disk[data_sector].data.backf = 0;
    disk[data_sector].data.frwdf = 0;
    memset(disk[data_sector].data.userdata, 0, DATA_SIZE);
    disk[ROOT_DIR_SECTOR].directory.free--;

    printf("Created file '%s' in data sector %d.\n", name, data_sector);
}



void deleteFile(const char name[])
{
    int dir_index = findDirectoryEntry(name);
    if (dir_index == -1) {
        fprintf(stderr, "Error: file '%s' was not found.\n", name);
        return;
    }
int sector = disk[ROOT_DIR_SECTOR].directory.dir[dir_index].link;
    while (sector != 0) {
        if (sector <= ROOT_DIR_SECTOR || sector >= NUM_OF_SECTORS) {
            fprintf(stderr, "Error: invalid data-sector link for '%s'.\n", name);
            break;
        }

        int next_sector = disk[sector].data.frwdf;
        Insert(sector);
        memset(&disk[sector], 0, sizeof(disk[sector]));
        sector = next_sector;
    }

    memset(&disk[ROOT_DIR_SECTOR].directory.dir[dir_index], 0,
           sizeof(disk[ROOT_DIR_SECTOR].directory.dir[dir_index]));
    disk[ROOT_DIR_SECTOR].directory.dir[dir_index].type = 'F';
    disk[ROOT_DIR_SECTOR].directory.dir[dir_index].link = 0;
    disk[ROOT_DIR_SECTOR].directory.free++;

    printf("Deleted file '%s'.\n", name);
}



void listFiles(void)
{
    int found = 0;

    printf("Type  Name      Size  First sector\n");
    printf("----  --------  ----  ------------\n");

    for (int i = 0; i < ENTRIES_PER_DIR; i++) {

        if (disk[0].directory.dir[i].type != 'F') {
            printf("%-4c  %-8s  %-4d  %d\n",
                   disk[0].directory.dir[i].type, disk[0].directory.dir[i].name,
                   disk[0].directory.dir[i].size, disk[0].directory.dir[i].link);
            found = 1;
        }
    }

    if (!found) {
        printf("(directory is empty)\n");
    
    }
}


void Root(){

}

void openFile(){

}

void closeFile(){

}

void readFile(){
    
}

void writeFile(){

}

void seekFile(){

}

void changeDir(){

}



//TODO: Main command loop (should be at the bottom)
int main(int argc, char *argv[]) {
    if (sizeof(disk[0].directory.dir[0]) != 20 || sizeof(union block) != 512) {
        printf("Error: structure sizes are wrong\n");
        return 1;
    }

    initializeDisk();

    printf("Enter an input to use our file system! It is case sensitive.\nFor info on commands, type INFO and enter. To exit, type EXIT and enter.\n");
    
    char userInput[510]; // to store input that is not the commands. since data max is 504... 510 should be enough?
    char userCommand[20];

    while(1){
        printf("CS149FS> ");

        if (fgets(userInput, sizeof(userInput), stdin) == NULL) {
            destroyList();
            break;
        }
        userInput[strcspn(userInput, "\n")] = '\0';
        sscanf(userInput, "%19s", userCommand);

        if (strcmp(userCommand, "INFO") == 0){
            printf("Commands are:\n");
            printf("  CREATE U \'filename\' - creates an empty regular file and adds its entry to the current directory.\n");
            printf("  CREATE D \'filename\' - allocates and initializes a directory sector and adds an entry of type D to the current directory.\n");
            // will add more stuff later
        } else if (strcmp(userCommand, "EXIT") == 0){
            destroyList();
            break;
        } else if (strcmp(userCommand, "CREATE") == 0){
            printf("Creating file...\n");
            //createFile();
        } else if (strcmp(userCommand, "DELETE") == 0){
            printf("Deleting file...\n");
            //deleteFile();
        } else if (strcmp(userCommand, "ROOT") == 0){
            printf("Moving to root.\n");
            //Root()
        } else if (strcmp(userCommand, "OPEN") == 0){
            printf("Opening file...\n");
            //openFile()
        } else if (strcmp(userCommand, "CLOSE") == 0){
            printf("Closing file...\n");
            //closeFile()
        } else if (strcmp(userCommand, "READ") == 0){
            printf("Reading file...\n");
            //readFile()
        } else if (strcmp(userCommand, "WRITE") == 0){
            printf("Writing to file...\n");
            //writeFile()
        } else if (strcmp(userCommand, "SEEK") == 0){
            printf("Looking for file...\n");
            //seekFile()
        } else if (strcmp(userCommand, "DIR") == 0){
            printf("this does something yeah\n");
            //listFiles()
        } else if (strcmp(userCommand, "CD") == 0){
            printf("Moving directories.\n");
            //changeDir()
        }
         else {
            printf("Please enter a valid command. (Case sensitive)\n");
        }
        
    }
}
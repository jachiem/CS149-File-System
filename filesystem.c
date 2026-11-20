#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define NUM_OF_SECTORS 100 
#define SECTOR_SIZE 512
#define ENTRIES_PER_DIR 25
#define DATA_SIZE 504

//below is the given code for disk sectors (?)
/*The first structure represents a directory sector. 
The second represents a sector containing the contents 
of a regular file. The sector array must be declared 
after the union definition.*/
union block { 
    struct { 
        int back; 
        int frwd; 
        int free; 
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
        char userdata[504]; 
    } 
    data; 
};


//TODO: disk organization  




//File stuff
void createFile(char type, const char name[]) {
    
}

void deleteFile(const char name[]) {
    
}

//TODO: Main command loop (should be at the bottom)
int main(int argc, char *argv[]) {

}
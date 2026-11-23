#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define NUM_OF_SECTORS 100 
#define SECTOR_SIZE 512
#define ENTRIES_PER_DIR 25
#define DATA_SIZE 504
#define ROOT_DIR_SECTOR 0
#define MAX_NAME_LENGTH 8

/* A directory entry points to the first data sector of a file. */
struct dir_entry {
    char type;
    char name[9];
    int link;
    short size;
};

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

/* The simulated disk and a small allocation table for all sectors. */
static union block disk[NUM_OF_SECTORS];
static int sector_used[NUM_OF_SECTORS];

static void initializeDisk(void)
{
    memset(disk, 0, sizeof(disk));
    memset(sector_used, 0, sizeof(sector_used));

    /* Sector 0 is always the root directory. */
    sector_used[ROOT_DIR_SECTOR] = 1;
    disk[ROOT_DIR_SECTOR].directory.back = -1;
    disk[ROOT_DIR_SECTOR].directory.frwd = -1;
    disk[ROOT_DIR_SECTOR].directory.free = ENTRIES_PER_DIR;

    for (int i = 0; i < ENTRIES_PER_DIR; i++) {
        disk[ROOT_DIR_SECTOR].directory.dir[i].type = '\0';
        disk[ROOT_DIR_SECTOR].directory.dir[i].link = -1;
        disk[ROOT_DIR_SECTOR].directory.dir[i].size = 0;
    }
}

static int findFreeDirectoryEntry(void)
{
    for (int i = 0; i < ENTRIES_PER_DIR; i++) {
        if (disk[ROOT_DIR_SECTOR].directory.dir[i].type == '\0') {
            return i;
        }
    }
    return -1;
}

static int findDirectoryEntry(const char name[])
{
    for (int i = 0; i < ENTRIES_PER_DIR; i++) {
        if (disk[ROOT_DIR_SECTOR].directory.dir[i].type != '\0' &&
            strcmp(disk[ROOT_DIR_SECTOR].directory.dir[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static int findFreeSector(void)
{
    for (int i = ROOT_DIR_SECTOR + 1; i < NUM_OF_SECTORS; i++) {
        if (!sector_used[i]) {
            return i;
        }
    }
    return -1;
}

/*
 * Create an empty file.  The supplied type is stored in the directory entry;
 * use 'f' for an ordinary file.  This version allocates its first data sector
 * immediately, which makes the entry ready for later write operations.
 */
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

    int data_sector = findFreeSector();
    if (data_sector == -1) {
        fprintf(stderr, "Error: disk is full.\n");
        return;
    }

    struct dir_entry *entry = &disk[ROOT_DIR_SECTOR].directory.dir[dir_index];

    entry->type = type;
  strcpy(entry->name, name);
    entry->link = data_sector;
    entry->size = 0;

    sector_used[data_sector] = 1;
    disk[data_sector].data.backf = -1;
    disk[data_sector].data.frwdf = -1;
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
    while (sector != -1) {
        if (sector <= ROOT_DIR_SECTOR || sector >= NUM_OF_SECTORS) {
            fprintf(stderr, "Error: invalid data-sector link for '%s'.\n", name);
            break;
        }

        int next_sector = disk[sector].data.frwdf;
        sector_used[sector] = 0;
        memset(&disk[sector], 0, sizeof(disk[sector]));
        sector = next_sector;
    }

    memset(&disk[ROOT_DIR_SECTOR].directory.dir[dir_index], 0,
           sizeof(disk[ROOT_DIR_SECTOR].directory.dir[dir_index]));
    disk[ROOT_DIR_SECTOR].directory.dir[dir_index].link = -1;
    disk[ROOT_DIR_SECTOR].directory.free++;

    printf("Deleted file '%s'.\n", name);
}
void listFiles(void)
{
    int found = 0;

    printf("Type  Name      Size  First sector\n");
    printf("----  --------  ----  ------------\n");

    for (int i = 0; i < ENTRIES_PER_DIR; i++) {
        const struct dir_entry *entry =
            &disk[ROOT_DIR_SECTOR].directory.dir[i];

        if (entry->type != '\0') {
            printf("%-4c  %-8s  %-4d  %d\n",
                   entry->type, entry->name, entry->size, entry->link);
            found = 1;
        }
    }

    if (!found) {
        printf("(directory is empty)\n");
    }
}






//File stuff
void createFile(char type, const char name[]) {
    
}

void deleteFile(const char name[]) {
    
}

//TODO: Main command loop (should be at the bottom)
int main(int argc, char *argv[]) {

}

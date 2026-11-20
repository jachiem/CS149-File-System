# CS149-File-System

We are a team of three collaborating to create a simple file management system where users can create, delete, edit, read, and list files with contiguous or linked allocation, all simulated on the computer without actually modifying the computer’s actual file system. When the user prompts to create or delete a file, the system stores or destroys the metadata in a simulated disk with an array of 100 blocks. For deletion, it will find the correct file with the starting block address before deletion. It will then return confirmation and the remaining available disk blocks. When prompted to read a file, the system will find and open the specific file and send the information to the user. When listing files, the system will list every single file in its disk of 100 blocks to the user. We might use linked allocation for the project due to the limited space of 100 blocks. Linked allocation lets a file use any free block so no blocks are wasted. 


The work will be divided amongst the three of us by the following, which is tentative: 


Gigi: codes the simulated disk, the bitmap, and the linked allocation logic (finding free blocks and linking them together). Researches and writes the section on Mac and Linux file systems (APFS, ext4).


Wenjie: codes Create File, Delete File (including releasing blocks and reporting the free count), and List Files. Researches and writes the section on Windows and flash drive file systems (NTFS, FAT32, exFAT)		


Janet: codes Write File, Read File, and the user menu that ties everything together. Researches and writes the section on what happens when you copy a file between file systems.


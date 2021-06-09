// Data Recovery Tool

// This program reads memory locations on a memory card to retrieve lost JPEG images and write copies 
// of these images to new JPEG files, based on hexidecimal values and 512 byte blocks.

// CS50 library imports.
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char *argv[])
{
    // Check for invalid input.
    // Make sure the user inputs only two arguments.
    if (argc != 2)
    {
        printf("Please input a single file.\n");
        return 1;
    }
    // Check to see if the file has anything in it.
    if (argv[1] == NULL)
    {
        printf("The image could not be opened.\n");
        return 1;
    }
    // Open the file (memory card) to read.
    FILE *open_file = fopen(argv[1], "r");
    // Create input variables that will be used in reading and writing our files.
    // Create a BYTE type to be used in storing bytes to our buffer array.
    typedef uint8_t BYTE;
    // Pointer (via uint8_t) where we will store data, temporarily, in memory we are reading
    // from the file in 512 byte (B) chunks.
    BYTE buffer[512];
    // Pointer to memory location to which we will write our new JPEG files.
    char *new_image_name;
    // A tracker that will give us the correct number to name each individual filename with.
    int new_image_name_count = 0;
    // The file that will contain a copy of an original JPEG image.
    FILE *new_image;

    // Start reading the card file until the end.
    // fread() is a file I/O stream.
    // We read the 512 B into a temporary buffer, 1, 512 B block at a time on each loop.
    // This while-loop loops through all blocks in the card file.
    while ((fread(buffer, 512, 1, open_file)))
    {
        // This if-block is used to address the first 512 B block of any JPEG file on our memory card.

        // Every JPEG on the card begins with a distinct header. If the first 4 bytes of our current 
        // block have certain hexidecimal values in their header, then we know we're at the beginning 
        // of a new JPEG.
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            // Since we are at the start of an original JPEG we need to copy all of the 512 B blocks of
            // that JPEG to a new JPEG file.

            // First, dynamically allocate memory for our new JPEG file name.
            // Second, set up a name for the image we are about to create.
            // Then create that image.

            // If new_image_name_count is 0, that means we haven't read any JPEGs and are on our first 
            // JPEG and don't have to close any previous JPEG files. Here, we write our first 512 B to
            // a new JPEG image.
            if (new_image_name_count == 0)
            {
                // Allocate memory for our file name on the heap, which consists of 3 characters (char),
                // with each character equating to 1 byte.
                new_image_name = malloc(3);
                // If malloc() can't give us memory, that will crash our program, so we shut down the
                // program if it can't.
                if (new_image_name == NULL)
                {
                    printf("Could not allocate new dynamic memory for new_image_name, so the program was aborted.");
                    return 1;
                }
                // If we are allocated memory, we create a name for our new image by printing it to a 
                // formatted string.
                sprintf(new_image_name, "%03i.jpg", new_image_name_count);
                // Then we create that new image (an empty file via "w") so we can write the information
                // from our original 512 B block.
                new_image = fopen(new_image_name, "w");
                // We write the information that was sent to "buffer" in our fread condition, to our new
                // image block in order to copy it.
                fwrite(buffer, 512, 1, new_image);
            }
            // If new_image_name_count is greater than zero, that means the 512 B block we are reading
            // contains the hexidecimal header, so we're no longer on the first JPEG anymore and have reached
            // another JPEG. We just wrote to a different JPEG file, and we need to close that file
            // in order to create and write this 512 B block to a new file here.
            else if (new_image_name_count > 0)
            {
                // Close the file we were just writing to. Example: If we were just writing to new_image
                // 002.jpeg, but we've encountered a new JPEG based on the hexidecimal header, we need to
                // close 002.jpeg, and create, open, and write to 003.jpeg in 512 B blocks.
                fclose(new_image);
                // Allocate memory for our new file name on the heap.
                // new_image_name is a pointer to that (malloc(3)) block of memory.
                new_image_name = malloc(3);
                // If malloc() can't give us memory, we shut down the program.
                if (new_image_name == NULL)
                {
                    printf("Could not allocate new dynamic memory for new_image_name, so the program was aborted.");
                    return 1;
                }
                // If we are allocated memory, we create a name for our new image.
                sprintf(new_image_name, "%03i.jpg", new_image_name_count);
                // Then we create that new image (an empty file via "w") so we can write the information from
                // our original 512 B block.
                new_image = fopen(new_image_name, "w");
                // Write to the new file.
                fwrite(buffer, 512, 1, new_image);
            }
            // Every time we create a new image (which will happen every time we encounter the hexidecimal header),
            // we add 1 to new_image_name_count so the next JPEG file that is created will get that value in
            // their name.
            new_image_name_count++;
        }
        // If we're not at the start of a new JPEG (i.e. the 512 B block we're reading doesn't start with the
        // hexidecimal header), we still have to write information to the rest of the blocks that are part of
        // the current JPEG file, so we do that below, copying non-starting 512 B blocks from our current JPEG 
        // to new_image.
        else
        {
            // Continue writing to the new file.
            fwrite(buffer, 512, 1, new_image);
        }
    }
    // Free up our dynamically allocated memory to prevent memory leaks.
    free(new_image_name);
}

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include "vedparse.h"

bool running = true;
static vedframe_t frame;

static void intHandler(int dummy) 
{
    running = false;
}

int32_t total = 0;
int32_t bad_frame = 0;


int main()
{
    signal(SIGINT, intHandler);
    printf("START\n");

    VEDPARSE_init();

    int byte;

    // Read byte by byte until EOF is reached
    while ((byte = getchar()) != EOF) 
    {
        //printf("%c", (char)byte); // Print

        bool frameReady = VEDPARSE_process(byte);

        if(frameReady)
        {
            uint32_t result = VEDPARSE_get_frame(&frame);
            total++;

            if(frame.checksum_valid)
            {
                printf("---------------------------\n");
                for(uint8_t i = 0; i < frame.property_count; i++)
                {
                    printf("%s: %s\n", frame.properties[i].key, frame.properties[i].value);
                }
                printf("---------------------------\n");
            }
            else
            {
                bad_frame++;
                printf("Frame completed with invalid checksum: %02X\n", frame.checksum);
            }

            printf("[Frame stats --> Bad: %d, Total %d]\n", bad_frame, total);

            VEDPARSE_init();
        }
    }

    printf("EXIT\n");
    return 0;
}

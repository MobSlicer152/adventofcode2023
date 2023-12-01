#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int32_t
main(
    int32_t argc,
    char* argv[]
    )
{
    FILE* File;
    char* FileBuffer;
    size_t FileSize;
    char* Line;
    char* End;
    bool Done;
    size_t i;
    size_t LineCount;
    uint32_t Left;
    uint32_t Right;
    uint32_t Total;

    File = NULL;
    FileBuffer = NULL;

    if ( argc < 2 )
    {
        fprintf(stderr, "Need name of input file\n");
        errno = EINVAL;
        goto End;
    }

    File = fopen(
        argv[1],
        "rb"
        );
    if ( !File )
    {
        fprintf(stderr, "Failed to open %s: %s\n", argv[1], strerror(errno));
        goto End;
    }

    fseek(
        File,
        0,
        SEEK_END
        );
    FileSize = ftell(File);
    if ( FileSize <= 0 )
    {
        fprintf(stderr, "Failed to get file size: %s\n", strerror(errno));
        goto End;
    }
    fseek(
        File,
        0,
        SEEK_SET
        );

    FileBuffer = calloc(
        1,
        FileSize
        );
    if ( !FileBuffer )
    {
        fprintf(stderr, "Failed to allocate %zu byte(s): %s\n", FileSize, strerror(errno));
        goto End;
    }

    if ( !fread(
             FileBuffer,
             1,
             FileSize,
             File
             ) )
    {
        fprintf(stderr, "Failed to read file: %s\n", strerror(errno));
        goto End;
    }

    fclose(File);
    File = NULL;

    Total = 0;
    Line = FileBuffer;
    strtok(
        FileBuffer,
        "\n"
        );
    Done = false;
    LineCount = 0;
    while ( !Done )
    {
        End = strtok(
            NULL,
            "\n"
            );
        Done = !End;
        if ( Done )
        {
            End = Line + strlen(Line) + 1;
        }

        Left = UINT32_MAX;
        Right = UINT32_MAX;
        printf("Line %zu: %.*s\n", LineCount + 1, End - Line, Line);
        for ( i = 0; i < End - Line && (Left == UINT32_MAX || Right == UINT32_MAX); i++ )
        {
            if ( isdigit(Line[i]) && Left == UINT32_MAX )
            {
                Left = Line[i] - '0';
                printf("Left = %u\n", Left);
            }

            // -2 to skip over NUL, means same index can be used
            if ( isdigit(End[-i - 2]) && Right == UINT32_MAX )
            {
                Right = End[-i - 2] - '0';
                printf("Right = %u\n", Right);
            }
        }

        if ( Left != UINT32_MAX && Right != UINT32_MAX )
        {
            Total += Left * 10 + Right;
            printf("Left = %u, Right = %u, Value = %u, Total = %u\n", Left, Right, Left * 10 + Right, Total);
        }

        Line = End;
        LineCount++;
    }

    printf("Processed %zu lines, total is %u\n", LineCount, Total);

    errno = 0;
End:
    if ( File )
    {
        fclose(File);
        File = NULL;
    }

    if ( FileBuffer )
    {
        free(FileBuffer);
        FileBuffer = NULL;
    }

    return errno;
}


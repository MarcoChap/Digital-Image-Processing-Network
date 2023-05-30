/*********************************************************************/
/* PhotoLab_server.c: Assignment 8 for CENG231, Spring 2023          */
/*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Image.h"

/*** global definitions ***/
#define SLEN 80       /* maximum length of file names */
#define MAX_PIXEL 255 /* maximum pixel intesity value */
#define BUF_SIZE 256

/*** function declarations ***/
/* Read image from a file 						*/
/* The size of the image needs to be pre-set				*/
/* The memory spaces of the image will be allocated in this function	*/
/* Return values:							*/
/* NULL: fail to load or create an image				*/
/* image: load or create an image successfully				*/
IMAGE *LoadImage(const char *fname);

/* Save a processed image 		*/
/* Return values:			*/
/* 0: successfully saved the image	*/
/* 1: Cannot open the file for writing	*/
/* 2: File error while writing to file	*/
int SaveImage(const char *fname, const IMAGE *image);

/* Age the image to make it look like old picture */
IMAGE *Aging(IMAGE *image);

/* Black and White */
IMAGE *BlackNWhite(IMAGE *image);

/* Reverse image color */
IMAGE *Negative(IMAGE *image);

/* flip image vertically */
IMAGE *VFlip(IMAGE *image);

/* mirror image horizontally */
IMAGE *HMirror(IMAGE *image);

/* sharpen the image */
IMAGE *Sharpen(IMAGE *image);

/* exchange R and B color channels */
IMAGE *ExRB(IMAGE *image);

/* test all functions */
int AutoTest(void);

/* Displays an error message to the command line */
void FatalError(const char *Program, const char *ErrorMsg);

/* Prints out the available command flags */
void PrintUsage();

int main(int argc, char *argv[])
{
    /* this is stolen from client {*/
    int x = 1;
    int port_no;
    while (x < argc)
    {
        /* the server ip address option */
        if (strcmp(argv[x], "-p") == 0)
        {
            if (x < argc - 1)
            {
                port_no = atoi(argv[x + 1]);
                x += 2;
                continue;
            }
            else
            {
                FatalError(argv[0], "Port must be greater than 2000");
            }
        }
        else if (strcmp(argv[x], "-h") == 0)
        {
            PrintUsage();
            return 0;
        }
        else
        {
            printf("\nCOMMAND IS %s.\n", argv[x]);
            FatalError(argv[0], "Unknown command detected. Do -h for help");
        }
        x++;
    }

    int n;
    int server_socket_fd, data_socket_fd;
    socklen_t client_len;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUF_SIZE];

    if (argc < 2)
    {
        FatalError(argv[0], "Usage: <Program> port");
    }

    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd < 0)
    {
        FatalError(argv[0], "Service socket creation failed");
    }

    printf("%s: preparing the server address...\n", argv[0]);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_no);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    printf("%s: assigning the server name to the socket...\n", argv[0]);
    if (bind(server_socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        FatalError(argv[0], "Binding the server to a socket failed");
    }

    printf("%s: listening on port %d...\n", argv[0], port_no);
    if (listen(server_socket_fd, 5) < 0)
    {
        FatalError(argv[0], "Listening on socket failed");
    }
    do
    {
        client_len = sizeof(client_addr);
        data_socket_fd = accept(server_socket_fd, (struct sockaddr *)&client_addr, &client_len);
        if (data_socket_fd < 0)
        {
            FatalError(argv[0], "Data socket creation (accept) failed");
        }

        do
        {
            n = read(data_socket_fd, buffer, sizeof(buffer) - 1);
            if (n < 0)
            {
                FatalError(argv[0], "Reading from data socket failed");
            }
            buffer[n] = '\0';

            printf("%s: Received message: %s\n", argv[0], buffer);
            if (strcmp(buffer, "AUTOTEST") == 0)
            {
                AutoTest();
                printf("%s: Sending response: OK.\n", argv[0]);
                n = write(data_socket_fd, "OK", 2);
            }
            else if (strcmp(buffer, "PRINT") == 0)
            {
                char *info = "\n\nWelcome! Here are the DIP operations:\n"
                             "1. Aging\n"
                             "2. Black and White\n"
                             "3. Negative\n"
                             "4. Vertical Flip\n"
                             "5. Horizontal Mirror\n"
                             "6. Sharpen\n"
                             "7. Exchange Red and Blue channels\n";
                n = write(data_socket_fd, info, strlen(info));
            }
            else if (strcmp(buffer, "CLOSE") == 0)
            {
                printf("%s: Sending response: OK.\n", argv[0]);
                n = write(data_socket_fd, "OK", 2);
                close(data_socket_fd);
                break;
            }
            if (n < 0)
            {
                FatalError(argv[0], "Writing to socket failed");
            }
        } while (1);
    } while (1);

    return 0;
}

IMAGE *LoadImage(const char *fname)
{
    FILE *File;
    char Type[SLEN];
    int W, H, MaxValue;
    unsigned int x, y;
    char ftype[] = ".ppm";
    char fname_tmp[SLEN];
    IMAGE *image;
    strcpy(fname_tmp, fname);
    strcat(fname_tmp, ftype);
    File = fopen(fname_tmp, "r");
    if (!File)
    {
        printf("\nCan't open file \"%s\" for reading!\n", fname);
        return NULL;
    }

    fscanf(File, "%79s", Type);
    if (Type[0] != 'P' || Type[1] != '6' || Type[2] != 0)
    {
        printf("\nUnsupported file format!\n");
        fclose(File);
        return NULL;
    }

    fscanf(File, "%d", &W);
    if (W <= 0)
    {
        printf("\nUnsupported image width %d!\n", W);
        fclose(File);
        return NULL;
    }

    fscanf(File, "%d", &H);
    if (H <= 0)
    {
        printf("\nUnsupported image height %d!\n", H);
        fclose(File);
        return NULL;
    }

    fscanf(File, "%d", &MaxValue);
    if (MaxValue != 255)
    {
        printf("\nUnsupported image maximum value %d!\n", MaxValue);
        fclose(File);
        return NULL;
    }
    if ('\n' != fgetc(File))
    {
        printf("\nCarriage return expected at the end of the file!\n");
        fclose(File);
        return NULL;
    }

    image = CreateImage(W, H);

    if (!image)
    {
        printf("\nError creating image from %s!\n", fname_tmp);
        fclose(File);
        return NULL;
    }
    else
    {
        for (y = 0; y < ImageHeight(image); y++)
            for (x = 0; x < ImageWidth(image); x++)
            {
                SetPixelR(image, x, y, fgetc(File));
                SetPixelG(image, x, y, fgetc(File));
                SetPixelB(image, x, y, fgetc(File));
            }

        if (ferror(File))
        {
            printf("\nFile error while reading from file!\n");
            DeleteImage(image);
            return NULL;
        }

        printf("%s was read successfully!\n", fname_tmp);
        fclose(File);
        return image;
    }
}

int SaveImage(const char *fname, const IMAGE *image)
{
    assert(image != NULL && "No image to save!\n");
    FILE *File;
    int x, y;
    char SysCmd[SLEN * 5];
    char ftype[] = ".ppm";
    char fname_tmp[SLEN];
    char fname_tmp2[SLEN];
    unsigned int WIDTH = ImageWidth(image);
    unsigned int HEIGHT = ImageHeight(image);
    strcpy(fname_tmp, fname);
    strcpy(fname_tmp2, fname);
    strcat(fname_tmp2, ftype);

    File = fopen(fname_tmp2, "w");
    if (!File)
    {
        printf("\nCan't open file \"%s\" for writing!\n", fname);
        return 1;
    }
    fprintf(File, "P6\n");
    fprintf(File, "%d %d\n", WIDTH, HEIGHT);
    fprintf(File, "255\n");

    for (y = 0; y < HEIGHT; y++)
        for (x = 0; x < WIDTH; x++)
        {
            fputc(GetPixelR(image, x, y), File);
            fputc(GetPixelG(image, x, y), File);
            fputc(GetPixelB(image, x, y), File);
        }

    if (ferror(File))
    {
        printf("\nError while writing to file!\n");
        return 2;
    }
    fclose(File);
    printf("%s was saved successfully. \n", fname_tmp2);
    /*
     * Rename file to image.ppm, convert it to ~/public_html/<fname>.jpg
     * and make it world readable
     */
    sprintf(SysCmd, "~ceng231/bin/pnmtojpeg.sh %s",
            fname_tmp2);
    if (system(SysCmd) != 0)
    {
        printf("\nError while converting to JPG:\nCommand \"%s\" failed!\n", SysCmd);
        return 3;
    }
    printf("%s.jpg was stored for viewing. \n", fname_tmp);
    return 0;
}

/* age the image to make it look like old picture */
IMAGE *Aging(IMAGE *image)
{
    int x, y;

    for (y = 0; y < ImageHeight(image); y++)
        for (x = 0; x < ImageWidth(image); x++)
        {
            SetPixelB(image, x, y, (GetPixelR(image, x, y) + GetPixelG(image, x, y) + GetPixelB(image, x, y)) / 5);
            SetPixelR(image, x, y, (unsigned char)GetPixelB(image, x, y) * 1.6);
            SetPixelG(image, x, y, (unsigned char)GetPixelB(image, x, y) * 1.6);
        }
    return image;
}

/* remove image color */
IMAGE *BlackNWhite(IMAGE *image)
{
    assert(image);
    int x, y, tmp;

    for (y = 0; y < ImageHeight(image); y++)
    {
        for (x = 0; x < ImageWidth(image); x++)
        {
            tmp = (GetPixelR(image, x, y) + GetPixelG(image, x, y) + GetPixelB(image, x, y)) / 3;
            SetPixelR(image, x, y, tmp);
            SetPixelG(image, x, y, tmp);
            SetPixelB(image, x, y, tmp);
        }
    }
    return image;
}

/* reverse image color */
IMAGE *Negative(IMAGE *image)
{
    assert(image);
    int x, y;
    for (y = 0; y < ImageHeight(image); y++)
    {
        for (x = 0; x < ImageWidth(image); x++)
        {
            SetPixelR(image, x, y, MAX_PIXEL - GetPixelR(image, x, y));
            SetPixelG(image, x, y, MAX_PIXEL - GetPixelG(image, x, y));
            SetPixelB(image, x, y, MAX_PIXEL - GetPixelB(image, x, y));
        }
    }
    return image;
}

/* flip image vertically */
IMAGE *VFlip(IMAGE *image)
{
    assert(image);
    int x, y;
    unsigned char r, g, b;
    for (y = 0; y < ImageHeight(image) / 2; y++)
    {
        for (x = 0; x < ImageWidth(image); x++)
        {
            r = GetPixelR(image, x, ImageHeight(image) - 1 - y);
            g = GetPixelG(image, x, ImageHeight(image) - 1 - y);
            b = GetPixelB(image, x, ImageHeight(image) - 1 - y);
            SetPixelR(image, x, ImageHeight(image) - 1 - y, GetPixelR(image, x, y));
            SetPixelG(image, x, ImageHeight(image) - 1 - y, GetPixelG(image, x, y));
            SetPixelB(image, x, ImageHeight(image) - 1 - y, GetPixelB(image, x, y));
            SetPixelR(image, x, y, r);
            SetPixelG(image, x, y, g);
            SetPixelB(image, x, y, b);
        }
    }
    return image;
}

/* mirror image horizontally */
IMAGE *HMirror(IMAGE *image)
{
    assert(image);
    int x, y;
    for (y = 0; y < ImageHeight(image); y++)
    {
        for (x = 0; x < ImageWidth(image) / 2; x++)
        {
            SetPixelR(image, ImageWidth(image) - 1 - x, y, GetPixelR(image, x, y));
            SetPixelG(image, ImageWidth(image) - 1 - x, y, GetPixelG(image, x, y));
            SetPixelB(image, ImageWidth(image) - 1 - x, y, GetPixelB(image, x, y));
        }
    }
    return image;
}

/* sharpen the image */
IMAGE *Sharpen(IMAGE *image)
{
    assert(image);
    IMAGE *tmpImage = CreateImage(ImageWidth(image), ImageHeight(image));
    assert(tmpImage);

    int x, y, m, n;
    memcpy(tmpImage->R, image->R, ImageWidth(image) * ImageHeight(image) * sizeof(unsigned char));
    memcpy(tmpImage->G, image->G, ImageWidth(image) * ImageHeight(image) * sizeof(unsigned char));
    memcpy(tmpImage->B, image->B, ImageWidth(image) * ImageHeight(image) * sizeof(unsigned char));
    for (y = 1; y < ImageHeight(image) - 1; y++)
    {
        for (x = 1; x < ImageWidth(image) - 1; x++)
        {
            int tmpR = GetPixelR(tmpImage, x, y);
            int tmpG = GetPixelG(tmpImage, x, y);
            int tmpB = GetPixelB(tmpImage, x, y);
            for (m = -1; m <= 1; m++)
            {
                for (n = -1; n <= 1; n++)
                {
                    tmpR += (GetPixelR(tmpImage, x, y) - GetPixelR(tmpImage, x + n, y + m));
                    tmpG += (GetPixelG(tmpImage, x, y) - GetPixelG(tmpImage, x + n, y + m));
                    tmpB += (GetPixelB(tmpImage, x, y) - GetPixelB(tmpImage, x + n, y + m));
                }
            }
            SetPixelR(image, x, y, (tmpR > MAX_PIXEL) ? MAX_PIXEL : (tmpR < 0) ? 0
                                                                               : tmpR);
            SetPixelG(image, x, y, (tmpG > MAX_PIXEL) ? MAX_PIXEL : (tmpG < 0) ? 0
                                                                               : tmpG);
            SetPixelB(image, x, y, (tmpB > MAX_PIXEL) ? MAX_PIXEL : (tmpB < 0) ? 0
                                                                               : tmpB);
        }
    }

    DeleteImage(tmpImage);
    tmpImage = NULL;
    return image;
}

/* exchange R and B color channels */
IMAGE *ExRB(IMAGE *image)
{
    assert(image);
    int x, y;
    unsigned char tmp;

    for (y = 0; y < ImageHeight(image); y++)
    {
        for (x = 0; x < ImageWidth(image); x++)
        {
            tmp = GetPixelR(image, x, y);
            SetPixelR(image, x, y, GetPixelB(image, x, y));
            SetPixelB(image, x, y, tmp);
        }
    }

    return image;
}

/* auto test */
int AutoTest(void)
{
    int result;
    const char fname[SLEN] = "wilson";

    IMAGE *image = NULL;
    image = LoadImage(fname);
    if (!image)
        return 11;
    image = Aging(image);
    if (!image)
        return 12;
    result = SaveImage("aging", image);
    if (result)
        return result;
    printf("Aging tested!\n\n");
    DeleteImage(image);
    image = NULL;

    image = LoadImage(fname);
    if (!image)
        return 13;
    image = BlackNWhite(image);
    if (!image)
        return 14;
    result = SaveImage("bw", image);
    if (result)
        return result;
    printf("Black and White tested!\n\n");
    DeleteImage(image);
    image = NULL;

    image = LoadImage(fname);
    if (!image)
        return 15;
    image = Negative(image);
    if (!image)
        return 16;
    result = SaveImage("negative", image);
    if (result)
        return result;
    printf("Negative tested!\n\n");
    DeleteImage(image);
    image = NULL;

    image = LoadImage(fname);
    if (!image)
        return 17;
    image = VFlip(image);
    if (!image)
        return 18;
    result = SaveImage("vflip", image);
    if (result)
        return result;
    printf("VFlip tested!\n\n");
    DeleteImage(image);
    image = NULL;

    image = LoadImage(fname);
    if (!image)
        return 19;
    image = HMirror(image);
    if (!image)
        return 20;
    result = SaveImage("hmirror", image);
    if (result)
        return result;
    printf("HMirror tested!\n\n");
    DeleteImage(image);
    image = NULL;

    image = LoadImage(fname);
    if (!image)
        return 21;
    image = Sharpen(image);
    if (!image)
        return 22;
    result = SaveImage("sharpen", image);
    if (result)
        return result;
    printf("Sharpen tested!\n\n");
    DeleteImage(image);
    image = NULL;

    image = LoadImage(fname);
    if (!image)
        return 23;
    image = ExRB(image);
    if (!image)
        return 24;
    result = SaveImage("xRB", image);
    if (result)
        return result;
    printf("ExRB tested!\n\n");
    DeleteImage(image);
    image = NULL;

    return 0; /* success! */
}

void PrintUsage()
{
    printf("Usage: PhotoLab_server -p <port_no>\n");
    printf("Options:\n");
    printf("-p      \t Specify the server port number\n");
    printf("-h      \t Display this usage information\n");
}

void FatalError(const char *Program, const char *ErrorMsg)
{
    fprintf(stderr, "%s: %s\n", Program, ErrorMsg);
    exit(EXIT_FAILURE);
}

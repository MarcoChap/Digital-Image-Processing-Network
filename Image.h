/* Image.h: header file for basic image manipulations		     */
/* Do not modify it */
#ifndef IMAGE_H
#define IMAGE_H

typedef struct {
	unsigned int W;	    /* image width */
	unsigned int H;     /* image height */
	unsigned char *R;	/* pointer to the memory storing all the R intensity values */
	unsigned char *G;	/* pointer to the memory storing all the G intensity values */
	unsigned char *B;	/* pointer to the memory storing all the B intensity values */
} IMAGE;

/* Get the R intensity of pixel (x, y) in image */
unsigned char GetPixelR(const IMAGE *image, unsigned int x,  unsigned int y);

/* Get the G intensity of pixel (x, y) in image */
unsigned char GetPixelG(const IMAGE *image, unsigned int x,  unsigned int y);

/* Get the B intensity of pixel (x, y) in image */
unsigned char GetPixelB(const IMAGE *image, unsigned int x,  unsigned int y);

/* Set the R intensity of pixel (x, y) in image to r */
void SetPixelR(IMAGE *image, unsigned int x,  unsigned int y, unsigned char r);

/* Set the G intensity of pixel (x, y) in image to g */
void SetPixelG(IMAGE *image, unsigned int x,  unsigned int y, unsigned char g);

/* Set the B intensity of pixel (x, y) in image to b */
void SetPixelB(IMAGE *image, unsigned int x,  unsigned int y, unsigned char b);

/* Allocate dynamic memory for the image structure and its R/G/B values */
/* Return the pointer to the image, or NULL in case of error */
IMAGE *CreateImage(unsigned int Width, unsigned int Height);

/* Free the memory for the R/G/B values and IMAGE structure */
void DeleteImage(IMAGE *image);

/* Return the image's width in pixels */
unsigned int ImageWidth(const IMAGE *image);

/* Return the image's height in pixels */
unsigned int ImageHeight(const IMAGE *image);

#endif

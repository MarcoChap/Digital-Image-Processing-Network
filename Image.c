/* Image.c: hw8, for CENG 231, Spring 2023 */

#include "Image.h"
#include <stdlib.h>
#include <assert.h>

unsigned char GetPixelR(const IMAGE *image, unsigned int x,  unsigned int y) {
    assert(image);
    assert(image->R);
    assert(x < ImageWidth(image));
    assert(y < ImageHeight(image));
    return image->R[x + y * ImageWidth(image)];
}

unsigned char GetPixelG(const IMAGE *image, unsigned int x,  unsigned int y) {
    assert(image);
    assert(image->G);
    assert(x < ImageWidth(image));
    assert(y < ImageHeight(image));
    return image->G[x + y * ImageWidth(image)];
}

unsigned char GetPixelB(const IMAGE *image, unsigned int x,  unsigned int y) {
    assert(image);
    assert(image->B);
    assert(x < ImageWidth(image));
    assert(y < ImageHeight(image));
    return image->B[x + y * ImageWidth(image)];
}

void SetPixelR(IMAGE *image, unsigned int x,  unsigned int y, unsigned char r) {
    assert(image);
    assert(image->R);
    assert(x < ImageWidth(image));
    assert(y < ImageHeight(image));
    image->R[x + y * ImageWidth(image)] = r;
}

void SetPixelG(IMAGE *image, unsigned int x,  unsigned int y, unsigned char g) {
    assert(image);
    assert(image->G);
    assert(x < ImageWidth(image));
    assert(y < ImageHeight(image));
    image->G[x + y * ImageWidth(image)] = g;
}

void SetPixelB(IMAGE *image, unsigned int x,  unsigned int y, unsigned char b) {	
    assert(image);
    assert(image->B);
    assert(x < ImageWidth(image));
    assert(y < ImageHeight(image));
    image->B[x + y * ImageWidth(image)] = b;
}

IMAGE *CreateImage(unsigned int Width, unsigned int Height) {
    IMAGE *image = malloc(sizeof(IMAGE));
	if (!image) return NULL;
    image->W = Width;
    image->H = Height;
    const int CHANNEL_SIZE = Width * Height * sizeof(unsigned char);
    image->R = malloc(CHANNEL_SIZE);
    if (!image->R) {
        free(image);
        return NULL;
    }
    image->G = malloc(CHANNEL_SIZE);
    if (!image->G) {
        free(image);
        free(image->R);
        return NULL;
    }
    image->B = malloc(CHANNEL_SIZE);
    if (!image->B) {
        free(image);
        free(image->R);
        free(image->G);
        return NULL;
    }
    return image;
}

/* Release the memory spaces for the pixel color intensity values */
/* Release the memory spaces for the image                        */
/* Set R/G/B pointers to NULL                                     */
void DeleteImage(IMAGE *image) {
    assert(image);
    assert(image->R);
    assert(image->G);
    assert(image->B);
    free(image->R);
    free(image->G);
    free(image->B);
    image->R = NULL;
    image->G = NULL;
    image->B = NULL;
    free(image);
}

unsigned int ImageWidth(const IMAGE *image){
	assert(image);
	
	return image->W;
}

unsigned int ImageHeight(const IMAGE *image){
	assert(image);
	
	return image->H;
}

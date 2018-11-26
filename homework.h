#ifndef HOMEWORK_H
#define HOMEWORK_H

typedef struct {
    unsigned char R, G, B;
} pixel_t;

typedef struct {
    unsigned char pic_type;
    unsigned int width, height;
    unsigned char maxval;
    pixel_t **color_picture;
    unsigned char **grayscale_picture;

} image;

typedef struct {
    image *in, *out;  
    unsigned int start, end;

} thread_data_t;

void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image * out);

#endif /* HOMEWORK_H */


#ifndef HOMEWORK_H1
#define HOMEWORK_H1

typedef struct {
    unsigned char pic_type;
    unsigned int width, height;
    unsigned char maxval;
    unsigned char **picture;

} image;

typedef struct {
    image *img;  
    unsigned int start, end;

} thread_data_t;

void initialize(image *im);
void render(image *im);
void writeData(const char * fileName, image *img);

#endif /* HOMEWORK_H1 */
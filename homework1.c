#include "homework1.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define LOGIC_LINE_WIDTH    3
#define LOGIC_IMAGE_WIDTH   100
#define BLACK               0x00
#define WHITE               0xFF

int num_threads;
int resolution;

void initialize(image *img) 
{    
    //Header
    img->pic_type = 5;
    img->width = resolution;
    img->height = resolution;
    img->maxval = 0xFF;

    //Image array
    img->picture = (unsigned char **) malloc(img->height * \
        sizeof(unsigned char *));
    for (int i = 0; i < img->height; i++) 
        img->picture[i] = (unsigned char *) malloc(img->width * \
            sizeof(unsigned char));

}

void* threadFunction(void *var)
{
    thread_data_t thread_data = *(thread_data_t *) var;
    int i, j;
    float y_coord, x_coord;

    //Pixels per logic space centimeter
    float scale = ((float)(thread_data.img)->width) / LOGIC_IMAGE_WIDTH;
    
    float sqrt5 = sqrt(5);
    
    //Half of a pixel's side length in centimeters (in the logic space)
    float half_pixel = 1 / (2 * scale);

    for (i = 0; i < (thread_data.img)->height; i++) {
        
	//Pixel's y_coord in the 100x100 logic space
        y_coord = i / scale + half_pixel;
        
        for (j = thread_data.start; j < thread_data.end; j++) {
        
	    //Pixel's x_coord in the 100x100 logic space
            x_coord = j / scale + half_pixel;
        
            //Compute distance from pixel's center to the line
	        if ((abs( - x_coord + 2 * y_coord) / sqrt5) < LOGIC_LINE_WIDTH) {
                (thread_data.img)->picture[(thread_data.img)->height - i - 1][j] = BLACK;
            } else {
                (thread_data.img)->picture[(thread_data.img)->height - i - 1][j] = WHITE;        
            }
        }
    }

    return NULL;

}

void render(image *img) 
{
    int i;
    pthread_t tid[num_threads];
    thread_data_t thread_data[num_threads];

    //Initialize thread_data structure 
    int chunks = img->width / num_threads;
    int rest = img->width % num_threads;
    
    thread_data[0].img = img;
    thread_data[0].start = 0;
    thread_data[0].end = chunks;
    if (rest != 0) thread_data[0].end++;

    //Set each thread's bounds to parallelize computing of the new pixels 
    for (i = 1; i < num_threads; i++) {

        thread_data[i].img = img;
        thread_data[i].start = thread_data[i - 1].end;
        
        //Spread the surplus to the first (chunks % num_threads) threads
        if (i < rest) {
            thread_data[i].end = thread_data[i].start + chunks + 1;
        } else {
            thread_data[i].end = thread_data[i].start + chunks;
        }

    }

    //Create threads
    for (i = 0; i < num_threads; i++) {
        pthread_create(&(tid[i]), NULL, threadFunction, &(thread_data[i]));
    }

    //Join threads
    for (i = 0; i < num_threads; i++) {
        pthread_join(tid[i], NULL);
    }
}

void writeData(const char * fileName, image *img) 
{    
    //Open output file
    FILE *output_file = fopen(fileName, "wb");

    //Write header
    fprintf(output_file, "P%hhu\n%u %u\n%hhu\n", img->pic_type, img->width,
            img->height, img->maxval);

    //Write image
    for (int i = 0; i < img->height; i++) 
            fwrite(img->picture[i], sizeof(unsigned char),
                img->width, output_file);
    
    //Close output file
    fclose(output_file);
}


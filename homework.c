#include "homework.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define isColored(img)  (img->pic_type == 6)

int num_threads;
int resize_factor;

void readInput(const char * fileName, image *img) 
{
    //Open input file
    FILE *input_file = fopen(fileName, "rb");
    
    //Read header
    fscanf(input_file, "P%hhu\n%u %u\n%hhu\n", &(img->pic_type), &(img->width),
            &(img->height), &(img->maxval));

    //Check picture type
    int is_colored = 0, is_grayscale = 0;
    if (img->pic_type == 6) {
        is_colored = 1;
    } else {
        is_grayscale = 1;
    }

    //Alloc color_picture matrix
    img->color_picture = (pixel_t**) malloc(img->height * \
            is_colored * sizeof(pixel_t*));
    for (int i = 0; i < img->height * is_colored; i++) {
        img->color_picture[i] = (pixel_t*) malloc(img->width * \
            sizeof(pixel_t));
    }

    //Alloc grayscale_picture matrix
    img->grayscale_picture = (unsigned char**) malloc(img->height * \
        is_grayscale * sizeof(unsigned char*));
    for (int i = 0; i < img->height * is_grayscale; i++) {
        img->grayscale_picture[i] = (unsigned char *) malloc(img->width * \
        sizeof(unsigned char));
    }

    //Read picture into according matrix
    if (is_colored) {
        for (int i = 0; i < img->height; i++)
            fread(img->color_picture[i], sizeof(pixel_t),
                img->width, input_file);
    } else {
        for (int i = 0; i < img->height; i++) 
            fread(img->grayscale_picture[i], sizeof(unsigned char),
                img->width, input_file);
    }

    //Close input file
    fclose(input_file);   
    
}

void writeData(const char * fileName, image *img) 
{
    //Open output file
    FILE *output_file = fopen(fileName, "wb");

    //Write header
    fprintf(output_file, "P%hhu\n%u %u\n%hhu\n", img->pic_type, img->width,
            img->height, img->maxval);

    //Check picture type
    int is_colored = 0;
    if (img->pic_type == 6) {
        is_colored = 1;
    }

    //Write picture into according matrix
    if (is_colored) {
        for (int i = 0; i < img->height; i++)
            fwrite(img->color_picture[i], sizeof(pixel_t), 
                img->width, output_file);
    } else {
        for (int i = 0; i < img->height; i++) 
            fwrite(img->grayscale_picture[i], sizeof(unsigned char),
                img->width, output_file);
    }

    //Close output file
    fclose(output_file);

}

void* threadFunction(void *var)
{
	thread_data_t thread_data = *(thread_data_t*) var;
    int sum, sumR, sumG, sumB, k_end, l_end, i, j, k, l, gauss_k, gauss_l;

    int gKernel[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};

	if (resize_factor != 3) {
        if (!isColored(thread_data.in)) {
            
            //First case (BW && RF != 3)

            //For each pixel in the output image
            for (i = 0; i < (thread_data.out)->height; i++) {

                k_end = (i + 1) * resize_factor; 
                for (j = thread_data.start; j < thread_data.end; j++) {
                    
                    sum = 0;
                    l_end = (j + 1) * resize_factor;
                    
                    //Sum its corresponding pixels from the input image
                    for (k = i * resize_factor; k < k_end; k++) {
                        for (l = j * resize_factor; l < l_end; l++) {
                    
                            sum += (thread_data.in)->grayscale_picture[k][l];
                    
                        }
                    }
                    
                    //Make the sum the arithmetic mean of them
                    sum /= resize_factor * resize_factor;
                    
                    //Store it into new pixel of output image
                    (thread_data.out)->grayscale_picture[i][j] = sum;
                
                }
            }
        } else {
            //Second case (Colored && RF != 3)

            pixel_t **in_picture = (thread_data.in)->color_picture;
            pixel_t **out_picture = (thread_data.out)->color_picture;
            //For each pixel in the output image
            for (i = 0; i < (thread_data.out)->height; i++) {
                
                k_end = (i + 1) * resize_factor; 
                for (j = thread_data.start; j < thread_data.end; j++) {
                    
                    sumR = sumG = sumB = 0;
                    l_end = (j + 1) * resize_factor;
                    
                    //Sum its corresponding pixels from the input image
                    for (k = i * resize_factor; k < k_end; k++) {
                        for (l = j * resize_factor; l < l_end; l++) {

                            sumR += (in_picture[k][l]).R;
                            sumG += (in_picture[k][l]).G;
                            sumB += (in_picture[k][l]).B;
                        
                        }
                    }
                    
                    //Make the sum the arithmetic mean of them
                    sumR /= resize_factor * resize_factor;
                    sumG /= resize_factor * resize_factor;
                    sumB /= resize_factor * resize_factor;
                    
                    //Store it into new pixel of output image
                    (out_picture[i][j]).R = sumR;
                    (out_picture[i][j]).G = sumG;
                    (out_picture[i][j]).B = sumB;

                }
            }
        }
    } else {
        if (!isColored(thread_data.in)) {
            //Third case (BW && RF == 3)

            //For each pixel in the output image
            for (i = 0; i < (thread_data.out)->height; i++) {

                k_end = (i + 1) * resize_factor; 
                for (j = thread_data.start; j < thread_data.end; j++) {
                    
                    sum = 0;
                    l_end = (j + 1) * resize_factor;
 
                    //Sum its corresponding pixels from the input image
                    //Multiplied by its coef from Gaussian Kernel                    
                    for (k = i * resize_factor; k < k_end; k++) {
                        for (l = j * resize_factor; l < l_end; l++) {
                    
                            sum += ((thread_data.in)->grayscale_picture[k][l] * \
                                gKernel[k % resize_factor][l % resize_factor]);
                        }
                    }

                    //Store it into new pixel of output image (divided by 16)
                    (thread_data.out)->grayscale_picture[i][j] = sum / 16;
                
                }
            }
        } else {
            //Fourth case (Colored && RF == 3)
            
            pixel_t **in_picture = (thread_data.in)->color_picture;
            pixel_t **out_picture = (thread_data.out)->color_picture;
            //For each pixel in the output image
            for (i = 0; i < (thread_data.out)->height; i++) {
                
                k_end = (i + 1) * resize_factor; 
                for (j = thread_data.start; j < thread_data.end; j++) {
                    
                    sumR = sumG = sumB = 0;
                    l_end = (j + 1) * resize_factor;

                    //Sum its corresponding pixels from the input image
                    //Multiplied by its coef from Gaussian Kernel                    
                    for (k = i * resize_factor; k < k_end; k++) {
                        
                        gauss_k = k % resize_factor;
                        for (l = j * resize_factor; l < l_end; l++) {
                            
                            gauss_l = l % resize_factor;
                            
                            sumR += ((in_picture[k][l]).R) * \
                                    gKernel[gauss_k][gauss_l];
                            sumG += ((in_picture[k][l]).G) * \
                                    gKernel[gauss_k][gauss_l];
                            sumB += ((in_picture[k][l]).B) * \
                                    gKernel[gauss_k][gauss_l];
                        }
                    }
                    
                    //Store it into new pixel of output image (divided by 16)
                    (out_picture[i][j]).R = sumR / 16;
                    (out_picture[i][j]).G = sumG / 16;
                    (out_picture[i][j]).B = sumB / 16;

                }
            }
        }
    }

    return NULL;
}

void resize(image *in, image * out) 
{     
    int i;
    
    //Write header
    out->pic_type = in->pic_type;
    out->width = in->width / resize_factor;
    out->height = in->height / resize_factor;
    out->maxval = in->maxval;

    pthread_t tid[num_threads];
    thread_data_t thread_data[num_threads];

    //Check picture type
    int is_colored = 0, is_grayscale = 0;
    if (isColored(in)) {
        is_colored = 1;
    } else {
        is_grayscale = 1;
    }

    //Alloc out color_picture matrix
    out->color_picture = (pixel_t**) malloc (out->height * \
        is_colored * sizeof(pixel_t*));
    for (i = 0; i < out->height * is_colored; i++) {
        out->color_picture[i] = (pixel_t*) malloc(out->width * \
            sizeof(pixel_t));
    }

    //Alloc out grayscale_picture matrix
    out->grayscale_picture = (unsigned char**) malloc (out->height * \
        is_grayscale * sizeof(unsigned char*));
    for (i = 0; i < out->height * is_grayscale; i++) {
        out->grayscale_picture[i] = (unsigned char *) malloc(out->width * \
            sizeof(unsigned char));
    }

    //Initialize thread_data structure 
    int chunks = out->width / num_threads;
    int rest = out->width % num_threads;
    
    thread_data[0].in = in;
    thread_data[0].out = out;
    thread_data[0].start = 0;
    thread_data[0].end = chunks;
    if (rest != 0) thread_data[0].end++;

    //Set each thread's bounds to parallelize computing of the new pixels 
    for (i = 1; i < num_threads; i++) {

        thread_data[i].in = in;
        thread_data[i].out = out;
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


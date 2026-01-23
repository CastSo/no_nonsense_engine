#include "tga_image.h"

int fget_little_short(FILE *f) {
    uint8_t b1, b2;

    b1 = fgetc(f);
    b2 = fgetc(f);

    return (short)(b1 + b2*256);
}

struct color4ub* load_tga(char *file_name, TGAHeader *tga_header) {
    //Assumes bottom left origin
    FILE *fptr;
    fptr = fopen(file_name, "r");
    if (fptr == NULL) {
        perror("Unable to open file");
        exit(1);
    }
    
    
    int width, height, number_pixels;
    int row, column;
    uint8_t *tga_rgba;

    uint8_t bpp = 0;

    tga_header->id_length = fgetc(fptr);
    tga_header->colormap_type = fgetc(fptr);
    tga_header->data_type_code = fgetc(fptr);

    tga_header->colormap_origin = fget_little_short(fptr);
    tga_header->colormap_length = fget_little_short(fptr);
    tga_header->colormap_depth = fgetc(fptr);
    tga_header->x_origin = fget_little_short(fptr);
    tga_header->y_origin = fget_little_short(fptr);
    tga_header->width = fget_little_short(fptr);
    tga_header->height = fget_little_short(fptr);
    tga_header->bits_per_pixel = fgetc(fptr);
    tga_header->image_descriptor = fgetc(fptr);

    if (tga_header->data_type_code != 2 && tga_header->data_type_code != 10) {
        perror("LoadTGA: Only type 2 and 10 targa RGB images supported\n");
        return NULL;
    }

    if (tga_header->colormap_type !=0 || (tga_header->bits_per_pixel !=32 && tga_header->bits_per_pixel!=24))
	{	
        perror("Texture_LoadTGA: Only 32 or 24 bit images supported (no colormaps)\n");
        return NULL;
    }
    
    width = tga_header->width; //columns
    height = tga_header->height; //rows
    number_pixels = width * height;
    //uint8_t *pixbuf ;
    color4ub *pixbuf = malloc(sizeof(color4ub) * (width*height));
    tga_rgba = malloc(number_pixels * 4);

    //printf("%d", tga_header->data_type_code);
    if (tga_header->id_length != 0)
        fseek(fptr, tga_header->id_length, SEEK_CUR); //Skip TARGA image comment

    //Uncompressed type
    int pixbuf_i = 0;
    if (tga_header->data_type_code == 2) {
        for(int row = height-1; row >= 0; row--) {
            //pixbuf = tga_rgba + row * width * 4;
            for(int column = 0; column < width; column++) {
                pixbuf_i =  row*width+column;
                unsigned char red, green, blue, alphabyte;
                switch (tga_header->bits_per_pixel) {
                    case 24:
                        blue = getc(fptr);
                        green = getc(fptr);
                        red = getc(fptr);
                        // *pixbuf++ = red;
                        // *pixbuf++ = green;
                        // *pixbuf++ = blue;
                        // *pixbuf++ = 255;
                        pixbuf[pixbuf_i].r = red;
                        pixbuf[pixbuf_i].g = green;
                        pixbuf[pixbuf_i].b = blue;
                        pixbuf[pixbuf_i].a = 255;
                        break;
                    case 32:
                        blue = getc(fptr);
                        green = getc(fptr);
                        red = getc(fptr);
                        alphabyte = getc(fptr);
                        // *pixbuf++ = red;
                        // *pixbuf++ = green;
                        // *pixbuf++ = blue;
                        // *pixbuf++ = alphabyte;
                        pixbuf[pixbuf_i].r = red;
                        pixbuf[pixbuf_i].g = green;
                        pixbuf[pixbuf_i].b = blue;
                        pixbuf[pixbuf_i].a = alphabyte;
                        break;

                }
            }
        }
    } else if (tga_header->data_type_code == 10) {
        unsigned char red, green, blue, alphabyte, packet_header, packet_size, j;
        for (int row = height-1; row >= 0; row--) {
            //pixbuf = tga_rgba + row * width * 4;
            for (int column = 0; column < width; ) {
                pixbuf_i =  row*width+column;
                packet_header = getc(fptr);
                packet_size = 1 + (packet_header & 0x7f);
                if (packet_header & 0x80) { //run-length packet
                    switch (tga_header->bits_per_pixel) {
                        case 24:
                            blue = getc(fptr);
                            green = getc(fptr);
                            red = getc(fptr);
                            alphabyte = 255;
                            break;
                        case 32:
                            blue = getc(fptr);
                            green = getc(fptr);
                            red = getc(fptr);
                            alphabyte = getc(fptr);
                            break;
                    }
                    for (int j = 0; j < packet_size; j++) {
                        pixbuf_i =  row*width+column;
                        // *pixbuf++ = red;
                        // *pixbuf++ = green;
                        // *pixbuf++ = blue;
                        // *pixbuf++ = alphabyte;
                        pixbuf[pixbuf_i].r = red;
                        pixbuf[pixbuf_i].g = green;
                        pixbuf[pixbuf_i].b = blue;
                        pixbuf[pixbuf_i].a = alphabyte;
                        column++;
                        if (column == width) {
                            column = 0;
                            if (row > 0){    
                                row--;
                            }
                            else
                                goto breakOut;
                            //pixbuf = tga_rgba + row * width * 4;
                            
                        }
                    }
                } else {    //non run-length packet
                    for (int j = 0; j < packet_size; j++) {
                        pixbuf_i =  row*width+column;
                        switch (tga_header->bits_per_pixel) {
                            case 24:
                                blue = getc(fptr);
                                green = getc(fptr);
                                red = getc(fptr);
                                // *pixbuf++ = red;
                                // *pixbuf++ = green;
                                // *pixbuf++ = blue;
                                // *pixbuf++ = 255;
                                pixbuf[pixbuf_i].r = red;
                                pixbuf[pixbuf_i].g = green;
                                pixbuf[pixbuf_i].b = blue;
                                pixbuf[pixbuf_i].a = 255;
                                break;
                            case 32:
                                blue = getc(fptr);
                                green = getc(fptr);
                                red = getc(fptr);
                                alphabyte = getc(fptr);
                                // *pixbuf++ = red;
                                // *pixbuf++ = green;
                                // *pixbuf++ = blue;
                                // *pixbuf++ = alphabyte;
                                pixbuf[pixbuf_i].r = red;
                                pixbuf[pixbuf_i].g = green;
                                pixbuf[pixbuf_i].b = blue;
                                pixbuf[pixbuf_i].a = alphabyte;
                                break;
                        }
                        column++;
                        if (column == width) {
                            column = 0;
                            if (row > 0)
                                row--;
                            else
                                goto breakOut;
                            //pixbuf = tga_rgba + row * width * 4;
                            
                        }
                    }
                }
               // printf("%d: %d, %d, %d\n", pixbuf_i, pixbuf[pixbuf_i].r, pixbuf[pixbuf_i].g, pixbuf[pixbuf_i].b);
            }
            breakOut:;
        }
    }


    free(tga_rgba);

    fclose(fptr);
    return pixbuf;
}

void flip_horizontally(color4ub *pixbuf, int width, int height) {
    for (int i = 0; i < width/2; i++) {
        for (int j = 0; j < height; j++) {
            int a = i+j*width;
            int b = width-1-i+j*width;
            color4ub tmp = pixbuf[a];
            pixbuf[a] = pixbuf[b];
            pixbuf[b] = tmp;
        }
    }
}


void flip_vertically(color4ub *pixbuf, int width, int height) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height/2; j++) {
            int a = i+j*width;
            int b = i+(height-1-j)*width;
            color4ub tmp = pixbuf[a];
            pixbuf[a] = pixbuf[b];
            pixbuf[b] = tmp;
        }
    }
}
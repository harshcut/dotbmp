#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

typedef unsigned int color_t;

struct Bitmap
{
    int width;
    int height;
    color_t *pixels;
};

struct Bitmap
init_bitmap(int width, int height)
{
    color_t *pixels = malloc(width * height * sizeof(color_t));
    struct Bitmap bitmap = {width, height, pixels};
    return bitmap;
}

void set_pixel(struct Bitmap bitmap, int x, int y, color_t color)
{
    if (x < 0 || x >= bitmap.width || y < 0 || y >= bitmap.height)
    {
        return;
    }

    bitmap.pixels[y * bitmap.width + x] = color;
}

void fill(struct Bitmap bitmap, int x1, int y1, int x2, int y2, color_t color)
{
    for (int y = y1; y <= y2; y++)
    {
        for (int x = x1; x <= x2; x++)
        {
            set_pixel(bitmap, x, y, color);
        }
    }
}

int calc_row_size(int width)
{
    int row_size = width * 3;
    row_size += 4 - (row_size % 4);
    assert(row_size % 4 == 0);
    return row_size;
}

int calc_image_size(int width, int height)
{
    return calc_row_size(width) * height;
}

void write_fileheader(int width, int height, FILE *file)
{
    int file_size = 14 + 40 + calc_image_size(width, height);
    int reserved = 0;
    int offset = 14 + 40;

    fputs("BM", file);
    fwrite(&file_size, 4, 1, file);
    fwrite(&reserved, 4, 1, file);
    fwrite(&offset, 4, 1, file);
}

void write_infoheader(int width, int height, FILE *file)
{
    int header_size = 40;
    int planes = 1;
    int bits_per_pixel = 24;
    int compression = 0;
    int image_size = calc_image_size(width, height);
    int x_pixels_per_meter = 0;
    int y_pixels_per_meter = 0;
    int colors_used = 0;
    int important_colors_used = 0;

    fwrite(&header_size, 4, 1, file);
    fwrite(&width, 4, 1, file);
    fwrite(&height, 4, 1, file);
    fwrite(&planes, 2, 1, file);
    fwrite(&bits_per_pixel, 2, 1, file);
    fwrite(&compression, 4, 1, file);
    fwrite(&image_size, 4, 1, file);
    fwrite(&x_pixels_per_meter, 4, 1, file);
    fwrite(&y_pixels_per_meter, 4, 1, file);
    fwrite(&colors_used, 4, 1, file);
    fwrite(&important_colors_used, 4, 1, file);
}

void write_pixeldata(struct Bitmap bitmap, FILE *file)
{
    int row_size = calc_row_size(bitmap.width);
    for (int y = bitmap.height - 1; y >= 0; y--)
    {
        for (int x = 0; x < bitmap.width; x++)
        {
            color_t color = bitmap.pixels[y * bitmap.width + x];
            unsigned char red = (color & 0xFF0000) >> 16;
            unsigned char green = (color & 0x00FF00) >> 8;
            unsigned char blue = color & 0x0000FF;

            fwrite(&blue, 1, 1, file);
            fwrite(&green, 1, 1, file);
            fwrite(&red, 1, 1, file);
        }
        for (int i = bitmap.width * 3; i < row_size; i++)
        {
            fputc(0, file);
        }
    }
}

void write_bitmap(struct Bitmap bitmap, const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        return;
    }

    write_fileheader(bitmap.width, bitmap.height, file);
    write_infoheader(bitmap.width, bitmap.height, file);
    write_pixeldata(bitmap, file);
    fclose(file);
}

void free_bitmap(struct Bitmap bitmap)
{
    free(bitmap.pixels);
    bitmap.pixels = NULL;
}

void cprint(int red, int green, int blue)
{
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "\033[48;2;%d;%d;%dm \033[0m", red, green, blue);
    printf("%s%s", buffer, buffer);
}

void print_pixeldata(int width, int height, unsigned char *pixeldata, int pixeldata_size)
{
    int alignment_padding = (4 - (width * 3) % 4) % 4;
    int bytes_per_row = width * 3 + alignment_padding;
    int start_index = pixeldata_size - bytes_per_row;

    for (int i = 1; i <= height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int red = pixeldata[start_index + 2];
            int green = pixeldata[start_index + 1];
            int blue = pixeldata[start_index];
            cprint(red, green, blue);
            start_index += 3;
        }
        start_index = pixeldata_size - (bytes_per_row * (i + 1));
        printf("\033[0;m\n");
    }
}

void free_pixeldata(unsigned char *pixeldata)
{
    free(pixeldata);
    pixeldata = NULL;
}

unsigned int parse_little_endian_int(char *buffer, int start, int length)
{
    unsigned int value = 0;
    for (int i = 0; i < length; i++)
    {
        value |= buffer[start + i] << (8 * i);
    }
    return value;
}

void display_bitmap(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        return;
    }

    char header[54];
    fread(header, sizeof(header), 1, file);
    unsigned int file_size = parse_little_endian_int(header, 2, 4);
    unsigned int pixeldata_offset = parse_little_endian_int(header, 10, 4);
    unsigned int width = parse_little_endian_int(header, 18, 4);
    unsigned int height = parse_little_endian_int(header, 22, 4);

    unsigned int pixeldata_size = file_size - pixeldata_offset;
    unsigned char *pixeldata = malloc(pixeldata_size);
    fread(pixeldata, pixeldata_size, 1, file);
    fclose(file);
    print_pixeldata(width, height, pixeldata, pixeldata_size);
    free_pixeldata(pixeldata);
}

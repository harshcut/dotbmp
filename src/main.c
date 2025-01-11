#include "lib.c"

int main()
{
    struct Bitmap bitmap = init_bitmap(30, 20);

    fill(bitmap, 0, 0, 9, 19, 0x0055A4);
    fill(bitmap, 10, 0, 19, 19, 0xFFFFFF);
    fill(bitmap, 20, 0, 29, 19, 0xF04135);

    write_bitmap(bitmap, "out.bmp");
    free_bitmap(bitmap);
    return 0;
}

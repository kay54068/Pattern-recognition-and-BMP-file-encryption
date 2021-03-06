#include "script_functions.h"

#define HEADER_LENGTH 54
#define TEMPLATE_NUMBER 10
#define FILE_NAME_LENGTH 100
#define WIDTH_POSITION_IN_HEADER 18
#define HEIGHT_POSITION_IN_HEADER 22
#define CORRELATION_THRESHOLD 0.5
#define MAXIMUM_OVERLAY 0.2

void removeTrailingTerminator(unsigned char *string)
{
    // find the first, if any, \n and end the string before it
    char *p = strchr(string, '\n');
    if (p != NULL)
        *p = '\0';

    p = strchr(string, '\r');
    if (p != NULL)
        *p = '\0';
}

void allocateMemoryForUnsignedCharArrays(unsigned char **array, unsigned int length)
{
    (*array) = (unsigned char *)malloc(length * sizeof(unsigned char));
    if ((*array) == NULL)
        printf("\nIn function -allocateMemoryForUnsignedCharArrays-, memory could not be allocated for the array!\n");
}

void allocateMemoryForPixelArrays(Pixel **array, unsigned int length)
{
    (*array) = (Pixel *)malloc(length * sizeof(Pixel));
    if ((*array) == NULL)
        printf("\nIn function -allocateMemoryForPixelArrays-, memory could not be allocated for the array!\n");
}

void allocateMemoryForUnsignedIntArrays(unsigned int **array, unsigned int length)
{
    (*array) = (unsigned int *)malloc(length * sizeof(unsigned int));
    if ((*array) == NULL)
        printf("\nIn function -allocateMemoryForUnsignedIntArrays-, memory could not be allocated for the array!\n");
}

void allocateMemoryForDetectionArrays(Detection **vector, unsigned int dimensiune)
{
    (*vector) = (Detection *)malloc(dimensiune * sizeof(Detection));
    if ((*vector) == NULL)
        printf("\nIn function -allocateMemoryForDetectionArrays-, memory could not be allocated for the array!\n");
}

void readWidthAndHeight(unsigned char *source_file_name, int *width, int *height)
{
    FILE *f;

    f = fopen(source_file_name, "rb");
    if (f == NULL)
    {
        printf("\nIn function -readWidthAndHeight- file could not be opened %s!\n", source_file_name);
        return;
    }

    fseek(f, WIDTH_POSITION_IN_HEADER, SEEK_SET);
    fread(width, sizeof(unsigned int), 1, f);
    fseek(f, HEIGHT_POSITION_IN_HEADER, SEEK_SET);
    fread(height, sizeof(unsigned int), 1, f);
    if (fclose(f) != 0)
        printf("\nIn function -readWidthAndHeight- file could not be closed %s!\n", source_file_name);
}

void readR0andSV(unsigned char *key_file_name, unsigned int *R0, unsigned int *SV)
{
    FILE *f;

    f = fopen(key_file_name, "r");
    if (f == NULL)
    {
        printf("\nIn function -readR0andSV- file could not be opened %s\n!", key_file_name);
        return;
    }
    fscanf(f, "%u %u", R0, SV);
    if (fclose(f) != 0)
        printf("\nIn function -readR0andSV- file could not be closed %s\n!", key_file_name);
}

void loadLinearizedHeaderInInternalMemory(unsigned char *source_file_name, unsigned char **vector)
{
    FILE *f;
    unsigned int i;

    f = fopen(source_file_name, "rb");
    if (f == NULL)
    {
        printf("\nIn function -loadLinearizedHeaderInInternalMemory- file could not be opened %s\n!", source_file_name);
        return;
    }

    fseek(f, 0, SEEK_SET);
    for (i = 0; i < HEADER_LENGTH; i++)
        fread(&((*vector)[i]), sizeof(unsigned char), 1, f);

    if (fclose(f) != 0)
        printf("\nIn function -loadLinearizedHeaderInInternalMemory- file could not be closed %s\n!", source_file_name);
}

void loadLinearizedBMPImageInInternalMemory(unsigned char *souce_file_name, Pixel **image_array, unsigned int W, unsigned int H)
{
    FILE *f;
    unsigned int i, j;
    unsigned int padding;

    if (W % 4 != 0)
        padding = 4 - (3 * W) % 4;
    else
        padding = 0;

    f = fopen(souce_file_name, "rb");
    if (f == NULL)
    {
        printf("\nIn function -loadLinearizedBMPImageInInternalMemory- file could not be opened %s\n!", souce_file_name);
        return;
    }

    for (i = 0; i < H; i++)
    {
        fseek(f, HEADER_LENGTH + (W * 3 + padding) * (H - 1 - i), SEEK_SET);
        for (j = 0; j < W; j++)
        {
            fread(&((*image_array)[i * W + j].B), sizeof(unsigned char), 1, f);
            fread(&((*image_array)[i * W + j].G), sizeof(unsigned char), 1, f);
            fread(&((*image_array)[i * W + j].R), sizeof(unsigned char), 1, f);
        }
    }
    if (fclose(f) != 0)
        printf("\nIn function -loadLinearizedBMPImageInInternalMemory- file could not be closed %s\n!", souce_file_name);
}

void saveLinearizedBMPImageInExternalMemory(unsigned char *destination_file_name, unsigned char *header_array, Pixel *BMP_image_array, unsigned int W, unsigned int H)
{
    unsigned int i, j, padding;
    unsigned char c = '0';
    FILE *f;

    if (W % 4 != 0)
        padding = 4 - (3 * W) % 4;
    else
        padding = 0;

    f = fopen(destination_file_name, "wb");
    if (f == NULL)
    {
        printf("\nIn function -salvareBMPInMeomoriaExterna- file could not be opened %s\n!", destination_file_name);
        return;
    }

    for (i = 0; i < HEADER_LENGTH; i++)
        fwrite(&header_array[i], sizeof(unsigned char), 1, f);

    for (i = 0; i <= H - 1; i++)
    {
        for (j = 0; j < W; j++)
            fwrite(&BMP_image_array[(H - 1 - i) * W + j], 3 * sizeof(unsigned char), 1, f);
        for (j = 1; j <= padding; j++)
            fwrite(&c, sizeof(unsigned char), 1, f);
    }

    if (fclose(f) != 0)
        printf("\nIn function -salvareBMPInMeomoriaExterna- file could not be closed %s\n!", destination_file_name);
}

void XORSHIFT32(unsigned int length, unsigned int seed, unsigned int **array)
{
    unsigned int r, k;
    r = seed;
    for (k = 1; k < length; k++)
    {
        r = r ^ r << 13;
        r = r ^ r >> 17;
        r = r ^ r << 5;
        (*array)[k] = r;
    }
}

void generateRandomPermutationDurstenfeld(unsigned int permutation_length, unsigned int *random_numbers_array, unsigned int **permutation)
{
    unsigned int r, k, aux;

    for (k = 0; k < permutation_length; k++)
        (*permutation)[k] = k;

    for (k = permutation_length - 1; k >= 1; k--)
    {
        r = random_numbers_array[permutation_length - k] % (k + 1);
        if (r != k)
        {
            aux = (*permutation)[k];
            (*permutation)[k] = (*permutation)[r];
            (*permutation)[r] = aux;
        }
    }
}

void permuteThePixelsOfLinearizedImage(Pixel **BMP_array, unsigned int *permutation, unsigned int BMP_array_length)
{
    Pixel *aux;
    unsigned int k;

    allocateMemoryForPixelArrays(&aux, BMP_array_length);

    for (k = 0; k < BMP_array_length; k++)
        aux[permutation[k]] = (*BMP_array)[k];

    for (k = 0; k < BMP_array_length; k++)
        (*BMP_array)[k] = aux[k];

    free(aux);
}

void XORintegerXpixel(unsigned int integer, Pixel **pixel, unsigned int k)
{
    unsigned char *c;

    c = (unsigned char *)&integer;
    (*pixel)[k].B = ((*pixel)[k].B) ^ (*c);
    c++;
    (*pixel)[k].G = ((*pixel)[k].G) ^ (*c);
    c++;
    (*pixel)[k].R = ((*pixel)[k].R) ^ (*c);
}

void XORpixelXpixel(Pixel **pixel1, Pixel **pixel2, unsigned int k1, unsigned int k2)
{
    (*pixel1)[k1].B = ((*pixel1)[k1].B) ^ ((*pixel2)[k2].B);
    (*pixel1)[k1].G = ((*pixel1)[k1].G) ^ ((*pixel2)[k2].G);
    (*pixel1)[k1].R = ((*pixel1)[k1].R) ^ ((*pixel2)[k2].R);
}

void encryptPixelsOfLinearizedImage(Pixel **BMP_array, unsigned int *random_numbers_array, unsigned int SV, unsigned int W, unsigned int H)
{
    unsigned int k;

    k = 0;
    XORintegerXpixel(SV, BMP_array, k);
    XORintegerXpixel(random_numbers_array[W * H], BMP_array, k);

    for (k = 1; k < W * H; k++)
    {
        XORpixelXpixel(BMP_array, BMP_array, k, k - 1);
        XORintegerXpixel(random_numbers_array[W * H + k], BMP_array, k);
    }
}

void encryptBMPimage(unsigned char *source_file_name, unsigned char *destination_file_name, unsigned char *key_file_name)
{
    unsigned int H, W, R0, SV, i;
    unsigned int *R, *P;
    unsigned char *header;
    Pixel *LinearizedBMPimage;

    readWidthAndHeight(source_file_name, &W, &H);
    allocateMemoryForUnsignedCharArrays(&header, HEADER_LENGTH);
    allocateMemoryForPixelArrays(&LinearizedBMPimage, W * H);
    loadLinearizedHeaderInInternalMemory(source_file_name, &header);
    loadLinearizedBMPImageInInternalMemory(source_file_name, &LinearizedBMPimage, W, H);

    readR0andSV(key_file_name, &R0, &SV);

    allocateMemoryForUnsignedIntArrays(&R, 2 * W * H);
    R[0] = R0;
    XORSHIFT32(2 * W * H, R0, &R);

    allocateMemoryForUnsignedIntArrays(&P, W * H);
    generateRandomPermutationDurstenfeld(W * H, R, &P);

    permuteThePixelsOfLinearizedImage(&LinearizedBMPimage, P, W * H);
    encryptPixelsOfLinearizedImage(&LinearizedBMPimage, R, SV, W, H);
    saveLinearizedBMPImageInExternalMemory(destination_file_name, header, LinearizedBMPimage, W, H);

    free(P);
    free(LinearizedBMPimage);
    free(R);
    free(header);
}

void generateReversePermutation(unsigned int **permutation, unsigned int permutation_length)
{
    unsigned int *aux;
    unsigned int k;

    allocateMemoryForUnsignedIntArrays(&aux, permutation_length);

    for (k = 0; k < permutation_length; k++)
        aux[(*permutation)[k]] = k;

    for (k = 0; k < permutation_length; k++)
        (*permutation)[k] = aux[k];

    free(aux);
}

void decryptPixelsOfLinearizedImage(Pixel **BMP_array, unsigned int *random_numbers_array, unsigned int SV, unsigned int W, unsigned int H)
{
    unsigned int k;
    Pixel *aux;

    allocateMemoryForPixelArrays(&aux, W * H);

    for (k = 0; k < W * H; k++)
    {
        aux[k].B = (*BMP_array)[k].B;
        aux[k].G = (*BMP_array)[k].G;
        aux[k].R = (*BMP_array)[k].R;
    }

    k = 0;
    XORintegerXpixel(SV, BMP_array, k);
    XORintegerXpixel(random_numbers_array[W * H], BMP_array, k);

    for (k = 1; k < W * H; k++)
    {
        XORpixelXpixel(BMP_array, &aux, k, k - 1);
        XORintegerXpixel(random_numbers_array[W * H + k], BMP_array, k);
    }

    free(aux);
}

void dencryptBMPimage(unsigned char *source_file_name, unsigned char *destination_file_name, unsigned char *key_file_name)
{
    unsigned int H, W, R0, SV, i;
    unsigned int *R, *P;
    unsigned char *header;
    Pixel *LinearizedBMPimage;

    readWidthAndHeight(source_file_name, &W, &H);
    allocateMemoryForUnsignedCharArrays(&header, HEADER_LENGTH);
    allocateMemoryForPixelArrays(&LinearizedBMPimage, W * H);
    loadLinearizedHeaderInInternalMemory(source_file_name, &header);
    loadLinearizedBMPImageInInternalMemory(source_file_name, &LinearizedBMPimage, W, H);

    readR0andSV(key_file_name, &R0, &SV);

    allocateMemoryForUnsignedIntArrays(&R, 2 * W * H);
    R[0] = R0;
    XORSHIFT32(2 * W * H, R0, &R);

    allocateMemoryForUnsignedIntArrays(&P, W * H);
    generateRandomPermutationDurstenfeld(W * H, R, &P);
    generateReversePermutation(&P, W * H);

    decryptPixelsOfLinearizedImage(&LinearizedBMPimage, R, SV, W, H);
    permuteThePixelsOfLinearizedImage(&LinearizedBMPimage, P, W * H);
    saveLinearizedBMPImageInExternalMemory(destination_file_name, header, LinearizedBMPimage, W, H);

    free(P);
    free(LinearizedBMPimage);
    free(R);
    free(header);
}

void frequencyOfAValueOnAColorChannel(Pixel *BMP_array, unsigned int BMP_array_length, unsigned int value, float *frequency, unsigned char channel)
{
    unsigned int i;

    (*frequency) = 0;
    if (channel == 'R')
        for (i = 0; i < BMP_array_length; i++)
        {
            if (value == BMP_array[i].R)
                (*frequency) = (*frequency) + 1;
        }
    else if (channel == 'G')
        for (i = 0; i < BMP_array_length; i++)
        {
            if (value == BMP_array[i].G)
                (*frequency) = (*frequency) + 1;
        }
    else
        for (i = 0; i < BMP_array_length; i++)
            if (value == BMP_array[i].B)
                (*frequency) = (*frequency) + 1;
}

void chiSquaredTestOnAColorChannel(Pixel *BMP_array, unsigned int H, unsigned int W, unsigned char channel)
{
    unsigned int i;
    float sum = 0;
    float theoretical_frequency, frequency;

    theoretical_frequency = (float)(H * W) / 256;

    for (i = 0; i <= 255; i++)
    {
        frequencyOfAValueOnAColorChannel(BMP_array, W * H, i, &frequency, channel);
        sum = sum + (float)((frequency - theoretical_frequency) * (frequency - theoretical_frequency)) / theoretical_frequency;
    }
    printf("%c: %f\n", channel, sum);
}

void chiSquaredTest(unsigned char *sorce_file_name)
{
    unsigned int H, W, i;
    Pixel *LinearizedBMPimage;

    readWidthAndHeight(sorce_file_name, &W, &H);
    allocateMemoryForPixelArrays(&LinearizedBMPimage, W * H);
    loadLinearizedBMPImageInInternalMemory(sorce_file_name, &LinearizedBMPimage, W, H);

    printf("Chi-squared test on RGB channels for %s:\n", sorce_file_name);
    chiSquaredTestOnAColorChannel(LinearizedBMPimage, H, W, 'R');
    chiSquaredTestOnAColorChannel(LinearizedBMPimage, H, W, 'G');
    chiSquaredTestOnAColorChannel(LinearizedBMPimage, H, W, 'B');

    free(LinearizedBMPimage);
}

void grayScaleLinearizedImage(Pixel **BMP_array, unsigned int H, unsigned int W)
{
    unsigned int i, j;
    unsigned char aux;

    for (i = 0; i <= H - 1; i++)
        for (j = 0; j <= W - 1; j++)
        {
            aux = 0.299 * ((*BMP_array)[i * W + j].R) + 0.587 * ((*BMP_array)[i * W + j].G) + 0.114 * ((*BMP_array)[i * W + j].B);
            (*BMP_array)[i * W + j].R = aux;
            (*BMP_array)[i * W + j].G = aux;
            (*BMP_array)[i * W + j].B = aux;
        }
}

void addBlackBorder(Pixel **BMP_array, unsigned int h, unsigned int w, unsigned int H, unsigned int W)
{
    Pixel *aux;
    int i, j;

    allocateMemoryForPixelArrays(&aux, (H + 2 * h - 2) * (W + 2 * w - 2));

    for (i = 0; i < h - 1; i++)
        for (j = 0; j < W + 2 * w - 2; j++)
        {
            aux[i * (W + 2 * w - 2) + j].R = 0;
            aux[i * (W + 2 * w - 2) + j].G = 0;
            aux[i * (W + 2 * w - 2) + j].B = 0;
        }

    for (i = h - 1; i < h + H - 1; i++)
    {
        for (j = 0; j < w - 1; j++)
        {
            aux[i * (W + 2 * w - 2) + j].R = 0;
            aux[i * (W + 2 * w - 2) + j].G = 0;
            aux[i * (W + 2 * w - 2) + j].B = 0;
        }

        for (j = w - 1; j < W + w - 1; j++)
        {
            aux[i * (W + 2 * w - 2) + j].R = (*BMP_array)[(i - h + 1) * W + (j - w + 1)].R;
            aux[i * (W + 2 * w - 2) + j].G = (*BMP_array)[(i - h + 1) * W + (j - w + 1)].G;
            aux[i * (W + 2 * w - 2) + j].B = (*BMP_array)[(i - h + 1) * W + (j - w + 1)].B;
        }

        for (j = W + w - 1; j < W + 2 * w - 2; j++)
        {
            aux[i * (W + 2 * w - 2) + j].R = 0;
            aux[i * (W + 2 * w - 2) + j].G = 0;
            aux[i * (W + 2 * w - 2) + j].B = 0;
        }
    }

    for (i = h + H - 1; i < H + 2 * h - 2; i++)
        for (j = 0; j < W + 2 * w - 2; j++)
        {
            aux[i * (W + 2 * w - 2) + j].R = 0;
            aux[i * (W + 2 * w - 2) + j].G = 0;
            aux[i * (W + 2 * w - 2) + j].B = 0;
        }

    free((*BMP_array));
    (*BMP_array) = aux;
}

void removeBlackBorder(Pixel **BMP_array, unsigned int h, unsigned int w, unsigned int H, unsigned int W)
{
    unsigned int i, j;

    for (i = 0; i < H; i++)
        for (j = 0; j < W; j++)
        {
            (*BMP_array)[i * W + j].R = (*BMP_array)[(i + h - 1) * (W + 2 * w - 2) + (j + w - 1)].R;
            (*BMP_array)[i * W + j].G = (*BMP_array)[(i + h - 1) * (W + 2 * w - 2) + (j + w - 1)].G;
            (*BMP_array)[i * W + j].B = (*BMP_array)[(i + h - 1) * (W + 2 * w - 2) + (j + w - 1)].B;
        }
}

void calculateMeanAndDeviation(Pixel *BMP_array, unsigned int x, unsigned int y, unsigned int h, unsigned int w, unsigned int W, float *mean, float *deviation)
{
    unsigned int i, j;
    float sum = 0;

    for (i = x; i <= x + (h - 1) * W; i = i + W)
        for (j = y; j <= y + w - 1; j++)
            sum = sum + BMP_array[i + j].R;
    (*mean) = sum / (h * w);

    sum = 0;
    for (i = x; i <= x + (h - 1) * W; i = i + W)
        for (j = y; j <= y + w - 1; j++)
            sum = sum + (BMP_array[i + j].R - (*mean)) * (BMP_array[i + j].R - (*mean));
    (*deviation) = sqrt(((float)1 / (h * w - 1)) * sum);
}

void templateMatchingForASingleTemplate(Detection **detections_array, unsigned int digit, int *detections_array_length, Pixel *BMP_array, Pixel *template_array, unsigned int h, unsigned int w, unsigned int H, unsigned int W, float ps)
{
    unsigned int i, j, x, y;
    float template_mean, BMP_array_mean, BMP_array_deviation, template_deviation, sum = 0, correlation;

    calculateMeanAndDeviation(template_array, 0, 0, h, w, w, &template_mean, &template_deviation);

    for (x = 0; x <= (H + h - 2) * (W + 2 * w - 2); x = x + W + 2 * w - 2)
        for (y = 0; y <= W + w - 2; y++)
        {
            calculateMeanAndDeviation(BMP_array, x, y, h, w, W + 2 * w - 2, &BMP_array_mean, &BMP_array_deviation);

            sum = 0;
            for (i = 0; i <= h - 1; i++)
                for (j = 0; j <= w - 1; j++)
                    sum = sum + ((float)1 / (BMP_array_deviation * template_deviation)) * (BMP_array[x + i * (W + 2 * w - 2) + y + j].R - BMP_array_mean) * (template_array[i * w + j].R - template_mean);
            correlation = ((float)1 / (h * w)) * sum;

            if (correlation > ps)
            {
                if ((*detections_array_length) == 0)
                    allocateMemoryForDetectionArrays(detections_array, 100000);
                else if ((*detections_array_length) % 99999 == 0)
                {
                    Detection *aux;
                    aux = (Detection *)realloc((*detections_array), ((*detections_array_length) + 1000) * sizeof(Detection));
                    if (aux == NULL)
                    {
                        printf("\nIn function -templateMatching-, memory could not be reallocated!\n");
                        return;
                    }
                    free((*detections_array));
                    (*detections_array) = aux;
                }

                (*detections_array)[(*detections_array_length)].index = x + y;
                (*detections_array)[(*detections_array_length)].digit = digit;
                (*detections_array)[(*detections_array_length)].correlation = correlation;
                (*detections_array_length) = (*detections_array_length) + 1;
            }
        }
}

void templateMatching(FILE *fin, Detection **detections_array, unsigned int *detections_array_length, Pixel **border_color_array, Pixel **BMP_array, unsigned int *h, unsigned int *w, int H, unsigned int W)
{
    unsigned char template_file_name[FILE_NAME_LENGTH];
    unsigned int i;
    unsigned int c;
    Pixel *template_array;

    for (i = 0; i < TEMPLATE_NUMBER; i++)
    {
        fgets(template_file_name, FILE_NAME_LENGTH, fin);
        removeTrailingTerminator(template_file_name);

        readWidthAndHeight(template_file_name, w, h);
        allocateMemoryForPixelArrays(&template_array, (*h) * (*w));
        loadLinearizedBMPImageInInternalMemory(template_file_name, &template_array, *w, *h);
        grayScaleLinearizedImage(&template_array, *h, *w);

        if (i == 0)
            addBlackBorder(BMP_array, *h, *w, H, W);

        templateMatchingForASingleTemplate(detections_array, i, detections_array_length, *BMP_array, template_array, *h, *w, H, W, CORRELATION_THRESHOLD);
        free(template_array);

        fscanf(fin, "%u", &c);
        (*border_color_array)[i].R = c;
        fscanf(fin, "%u", &c);
        (*border_color_array)[i].G = c;
        fscanf(fin, "%u", &c);
        (*border_color_array)[i].B = c;

        // to remove any \n or \r at the end
        fgets(template_file_name, 10, fin);
    }
}

void drawOutline(Pixel **BMP_array, unsigned int position, Pixel *C, unsigned int h, unsigned int w, unsigned int H, unsigned int W)
{
    unsigned int i, j, aux1, aux2;

    aux1 = position;
    if (position / (W + 2 * w - 2) < h - 1)
        aux1 = (h - 1) * (W + 2 * w - 2) + position % (W + 2 * w - 2);

    aux2 = position + (h - 1) * (W + 2 * w - 2);
    if (position / (W + 2 * w - 2) + h - 1 > H + h - 2)
        aux2 = (H + h - 2) * (W + 2 * w - 2) + position % (W + 2 * w - 2);

    for (j = 0; j <= w - 1; j++)
    {
        (*BMP_array)[aux1 + j].B = C->B;
        (*BMP_array)[aux1 + j].G = C->G;
        (*BMP_array)[aux1 + j].R = C->R;

        (*BMP_array)[aux2 + j].B = C->B;
        (*BMP_array)[aux2 + j].G = C->G;
        (*BMP_array)[aux2 + j].R = C->R;
    }

    aux1 = position;
    if (position % (W + 2 * w - 2) < w - 1)
        aux1 = position - (position % (W + 2 * w - 2)) + w - 1;

    aux2 = position + w - 1;
    if ((position % (W + 2 * w - 2) + w - 1) > W + w - 2)
        aux2 = position - (position % (W + 2 * w - 2)) + W + w - 2;

    for (i = 0; i <= h - 1; i++)
    {
        (*BMP_array)[aux1 + i * (W + 2 * w - 2)].B = C->B;
        (*BMP_array)[aux1 + i * (W + 2 * w - 2)].G = C->G;
        (*BMP_array)[aux1 + i * (W + 2 * w - 2)].R = C->R;

        (*BMP_array)[aux2 + i * (W + 2 * w - 2)].B = C->B;
        (*BMP_array)[aux2 + i * (W + 2 * w - 2)].G = C->G;
        (*BMP_array)[aux2 + i * (W + 2 * w - 2)].R = C->R;
    }
}

int cmpcorrelation(const void *a, const void *b)
{
    if (((Detection *)a)->correlation > ((Detection *)b)->correlation)
        return -1;
    else if (((Detection *)a)->correlation < ((Detection *)b)->correlation)
        return 1;
    else
        return 0;
}

void sortDetectionInDescendingOrder(Detection **detections_array, unsigned int detections_array_length)
{
    qsort(*detections_array, detections_array_length, sizeof(Detection), cmpcorrelation);
}

int overlayCheck(unsigned int xC, unsigned int yC, unsigned int xD, unsigned int yD, unsigned int h, unsigned int w, float suprapunereMaxima)
{
    float aria;
    if (xC >= xD || yC >= yD)
        return -1; //the two rectangles do not intersect

    aria = (xD - xC) * (yD - yC);

    if (((float)aria / (2 * h * w - aria)) <= suprapunereMaxima)
        return -1;

    return 1; // the rectangle with index j must be deleted
}

void calculateIntersectionCoordinates(Detection **detections_array, unsigned int *xC, unsigned int *yC, unsigned int *xD, unsigned int *yD, unsigned int i, unsigned int j, unsigned int h, unsigned int w, unsigned int H, unsigned int W)
{
    unsigned int xA, xB, yA, yB;

    // xA,xB,yA,yB are the coordinates of the upper left point corresponding to rectangles A and B
    // xC,yC are the coordinates of the upper left point corresponding to the rectangle formed at the intersection of rectangles A and B

    xA = (*detections_array)[i].index / (W + 2 * w - 2);
    yA = (*detections_array)[i].index % (W + 2 * w - 2);
    xB = (*detections_array)[j].index / (W + 2 * w - 2);
    yB = (*detections_array)[j].index % (W + 2 * w - 2);

    if (xA > xB)
        (*xC) = xA;
    else
        (*xC) = xB;

    if (yA > yB)
        (*yC) = yA;
    else
        (*yC) = yB;

    // xA,xB,yA,yB are the coordinates of the lower right point corresponding to rectangles A and B
    // xD,yD are the coordinates of the lower right point corresponding to the rectangle formed at the intersection of rectangles A and B

    xA = xA + h;
    yA = yA + w;
    xB = xB + h;
    yB = yB + w;

    if (xA < xB)
        (*xD) = xA;
    else
        (*xD) = xB;

    if (yA < yB)
        (*yD) = yA;
    else
        (*yD) = yB;
}

void eliminationOfNonMaximums(Detection **detections_array, unsigned int *detections_array_length, unsigned int h, unsigned int w, unsigned int H, unsigned int W)
{
    unsigned int xC, xD, yC, yD, i, j;

    sortDetectionInDescendingOrder(detections_array, *detections_array_length);

    for (i = 0; i <= (*detections_array_length) - 2; i++)
        if ((*detections_array)[i].correlation != 2)
            for (j = i + 1; j <= (*detections_array_length) - 1; j++)
            {
                calculateIntersectionCoordinates(detections_array, &xC, &yC, &xD, &yD, i, j, h, w, H, W);
                if (overlayCheck(xC, yC, xD, yD, h, w, MAXIMUM_OVERLAY) == 1)
                    (*detections_array)[j].correlation = 2; // the rectangle with index j must be deleted
            }

    j = 0;
    for (i = 0; i < (*detections_array_length); i++)
        if ((*detections_array)[i].correlation != 2)
        {
            (*detections_array)[j].digit = (*detections_array)[i].digit;
            (*detections_array)[j].correlation = (*detections_array)[i].correlation;
            (*detections_array)[j].index = (*detections_array)[i].index;
            j++;
        }

    (*detections_array_length) = j;
}

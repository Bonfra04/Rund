#include "ttf.h"
#include <stdio.h>
#include <stdlib.h>

#define TTF_MAGIC_NUM   (0xF53C0F5F)

// sfnt versions
#define TTF_SFNT_1_0    (0x00010000)
#define TTF_SFNT_OTTO   (0x4F54544F)

// table tags
#define TTF_GLYF_TAG    (0x676C7966)
#define TTF_MAXP_TAG    (0x6D617870)
#define TTF_HEAD_TAG    (0x68656164)
#define TTF_LOCA_TAG    (0x6C6F6361)
#define TTF_GLYF_TAG    (0x676C7966)

#define TTF_ENDIAN_WORD(a) \
    (((0xFF00 & a) >> 8) | ((0x00FF & a) << 8))
#define TTF_ENDIAN_DWORD(a) \
    (((0xFF000000 & a) >> 24) | ((0x00FF0000 & a) >> 8) | \
    ((0x0000FF00 & a) << 8) | ((0x000000FF & a) << 24))

// loading functions

static void read_header(FILE* file, ttf_t* ttf);
static void read_maxp(FILE* file, ttf_t* ttf);
static void read_head(FILE* file, ttf_t* ttf);
static void read_loca(FILE* file, ttf_t* ttf);
static void read_glyf(FILE* file, ttf_t* ttf);

bool ttf_load(const char* filepath, ttf_t* ttf)
{
    // open the file
    FILE* file = fopen(filepath, "rb");

    // load the table directory
    fread(&ttf->table_directory, sizeof(ttf_table_directory_t), 1, file);
    ttf->table_directory.sfnt_version = TTF_ENDIAN_DWORD(ttf->table_directory.sfnt_version);
    ttf->table_directory.num_tables = TTF_ENDIAN_WORD(ttf->table_directory.num_tables);

    read_header(file, ttf);

    read_head(file, ttf);
    read_maxp(file, ttf);
    read_loca(file, ttf);
    read_glyf(file, ttf);

    return true;
}

// takes for granted that the file is advanced right after the table directory
static void read_header(FILE* file, ttf_t* ttf)
{
    // iterate through the tables
    for(int i = 0; i < ttf->table_directory.num_tables; i++)
    {
        // read the table
        ttf_table_header_t header;
        fread(&header, sizeof(ttf_table_header_t), 1, file);
        header.tag = TTF_ENDIAN_DWORD(header.tag);
        header.checksum = TTF_ENDIAN_DWORD(header.checksum);
        header.offset = TTF_ENDIAN_DWORD(header.offset);
        header.length = TTF_ENDIAN_DWORD(header.length);

        switch(header.tag)
        {
            case TTF_MAXP_TAG:
                ttf->maxp_table = header;
                break;
            case TTF_HEAD_TAG:
                ttf->head_table = header;
                break;
            case TTF_LOCA_TAG:
                ttf->loca_table = header;
                break;
            case TTF_GLYF_TAG:
                ttf->glyf_table = header;
                break;
        }
    }
}

static void read_maxp(FILE* file, ttf_t* ttf)
{
    // seek to the table
    fseek(file, ttf->maxp_table.offset, SEEK_SET);
    // read the table
    fread(&ttf->maxp, sizeof(ttf_maxp_t), 1, file);
}

static void read_head(FILE* file, ttf_t* ttf)
{
    // seek to the table
    fseek(file, ttf->head_table.offset, SEEK_SET);
    // read the table
    fread(&ttf->head, sizeof(ttf_head_t), 1, file);
}

static void read_loca(FILE* file, ttf_t* ttf)
{
    // seek to the table
    fseek(file, ttf->loca_table.offset, SEEK_SET);

    // get array size
    size_t size = (ttf->maxp.numGlyphs + 1);
    size *= ttf->head.index_to_loc_format == 0 ? sizeof(uint16_t) : sizeof(uint32_t);

    // alocate the array
    ttf->glyf_offsets = malloc(size);
    // read the table
    fread(ttf->glyf_offsets, size, 1, file);
}

static void read_glyf(FILE* file, ttf_t* ttf)
{
    // seek to the table
    fseek(file, ttf->glyf_table.offset, SEEK_SET);

    ttf->glyfs = (ttf_glyf_t*)malloc(sizeof(ttf_glyf_t) * (ttf->maxp.numGlyphs));

    for(int i = 0; i < ttf->maxp.numGlyphs; i++)
    {
        ttf_glyf_t* glyf = &(ttf->glyfs[i]);

        // read the header
        ttf_glyf_header_t* header = &(glyf->header);
        fread(header, sizeof(ttf_glyf_header_t), 1, file);
        header->numberOfContours = TTF_ENDIAN_WORD(header->numberOfContours);
        header->xMin = TTF_ENDIAN_WORD(header->xMin);
        header->yMin = TTF_ENDIAN_WORD(header->yMin);
        header->xMax = TTF_ENDIAN_WORD(header->xMax);
        header->yMax = TTF_ENDIAN_WORD(header->yMax);

        if(header->numberOfContours < 0)
        {
            printf("NO");
            exit(0);
        }

        // read the descriptor
        ttf_glyf_descriptor_t* descriptor = &ttf->glyfs[i].descriptor;

        // read the end points
        descriptor->endPtsOfContours = (uint16_t*)malloc(sizeof(uint16_t) * header->numberOfContours);
        fread(descriptor->endPtsOfContours, sizeof(uint16_t), header->numberOfContours, file);

        // get max num_points
        size_t num_points = 0;
        for(int j = 0; j < header->numberOfContours; j++)
        {
            descriptor->endPtsOfContours[j] = TTF_ENDIAN_WORD(descriptor->endPtsOfContours[j]);
            if(descriptor->endPtsOfContours[j] > num_points)
                num_points = descriptor->endPtsOfContours[j];
        }
        num_points++;

        // read instructions
        fread(&descriptor->instructionLength, sizeof(uint16_t), 1, file);
        descriptor->instructionLength = TTF_ENDIAN_WORD(descriptor->instructionLength);
        descriptor->instructions = (uint8_t*)malloc(sizeof(uint8_t) * descriptor->instructionLength);
        fread(descriptor->instructions, sizeof(uint8_t), descriptor->instructionLength, file);

        // read flags
        descriptor->flags = (uint8_t*)malloc(sizeof(uint8_t) * num_points);
        fread(descriptor->flags, sizeof(uint8_t), num_points, file);

        // read x_coordinates
        descriptor->xCoordinates = (int16_t*)malloc(sizeof(int16_t) * num_points);
        fread(descriptor->xCoordinates, sizeof(int16_t), num_points, file);

        // read y_coordinates
        descriptor->yCoordinates = (int16_t*)malloc(sizeof(int16_t) * num_points);
        fread(descriptor->yCoordinates, sizeof(int16_t), num_points, file);
    }
}
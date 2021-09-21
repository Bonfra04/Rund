#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct ttf_table_directory
{
    uint32_t sfnt_version;
    uint16_t num_tables;
    uint16_t search_range;
    uint16_t entry_selector;
    uint16_t range_shift;
} __attribute__ ((packed)) ttf_table_directory_t;

typedef struct ttf_table_header
{
    uint32_t tag;
    uint32_t checksum;
    uint32_t offset;
    uint32_t length;
} __attribute__ ((packed)) ttf_table_header_t;

typedef struct ttf_maxp
{
    uint32_t version;
    uint16_t numGlyphs;
    uint16_t maxPoints;
    uint16_t maxContours;
    uint16_t maxCompositePoints;
    uint16_t maxCompositeContours;
    uint16_t maxZones;
    uint16_t maxTwilightPoints;
    uint16_t maxStorage;
    uint16_t maxFunctionDefs;
    uint16_t maxInstructionDefs;
    uint16_t maxStackElements;
    uint16_t maxSizeOfInstructions;
    uint16_t maxComponentElements;
    uint16_t maxComponentDepth;
} __attribute__ ((packed)) ttf_maxp_t;

typedef struct ttf_head
{
    uint32_t version;
    uint32_t font_revision;
    uint32_t checksumAdjust;
    uint32_t magic;
    uint16_t flags;
    uint16_t upem;
    uint8_t created[8];
    uint8_t modified[8];
    int16_t xmin;
    int16_t ymin;
    int16_t xmax;
    int16_t ymax;
    uint16_t mac_style;
    uint16_t lowest_rec_ppm;
    int16_t direction_hint;
    int16_t index_to_loc_format;
    int16_t glyph_data_format;
} __attribute__ ((packed)) ttf_head_t;

typedef struct ttf_glyf_header
{
    int16_t numberOfContours;
    int16_t xMin;
    int16_t yMin;
    int16_t xMax;
    int16_t yMax;
} __attribute__ ((packed)) ttf_glyf_header_t;

typedef struct ttf_glyf_descriptor
{
    uint16_t* endPtsOfContours;
    uint16_t instructionLength;
    uint8_t* instructions;
    uint8_t* flags;
    int16_t* xCoordinates;
    int16_t* yCoordinates;
} __attribute__ ((packed)) ttf_glyf_descriptor_t;

typedef struct ttf_glyf
{
    ttf_glyf_header_t header;
    ttf_glyf_descriptor_t descriptor;
} __attribute__ ((packed)) ttf_glyf_t;

typedef struct ttf
{
    ttf_table_directory_t table_directory;
    ttf_table_header_t maxp_table;
    ttf_table_header_t head_table;
    ttf_table_header_t loca_table;
    ttf_table_header_t glyf_table;

    ttf_maxp_t maxp;
    ttf_head_t head;

    ttf_glyf_t* glyfs;
    void* glyf_offsets;
} ttf_t;

bool ttf_load(const char* filepath, ttf_t* ttf);

#include "ttf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TTF_MAGIC_NUM   (0xF53C0F5F)

// sfnt versions
#define TTF_SFNT_1_0    (0x00010000)
#define TTF_SFNT_OTTO   (0x4F54544F)

// table tags
#define TTF_GLYF_TAG    (0x676C7966)
#define TTF_MAXP_TAG    (0x6D617870)
#define TTF_HEAD_TAG    (0x68656164)
#define TTF_LOCA_TAG    (0x6C6F6361)
#define TTF_HHEA_TAG    (0x68686561)
#define TTF_HMTX_TAG    (0x686D7478)
#define TTF_CMAP_TAG    (0x636D6170)

// flags
#define TTF_FLAG_REPEAT (0x08)
#define TTF_ON_CURVE    (0x01)
#define TTF_XSHORT      (0x02)
#define TTF_YSHORT      (0x04)
#define TTF_XREPEAT     (0x10)
#define TTF_YREPEAT     (0x20)

// platforms
#define TTF_PLATFORM_UNICODE    (0)
#define TTF_PLATFORM_MACINTOSH  (1)
#define TTF_PLATFORM_ISO        (2)
#define TTF_PLATFORM_MICROSOFT  (3)

// unicode encoding
#define TTF_UNICODE_1_0          (0)
#define TTF_UNICODE_1_1          (1)
#define TTF_UNICODE_ISO          (2)
#define TTF_UNICODE_2_0_BMP_ONLY (3)
#define TTF_UNICODE_2_0_ALL      (4)
#define TTF_UNICODE_VAR_SEQ      (5)
#define TTF_UNICODE_LAST_RESORT  (6)

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
static void read_hhea(FILE* file, ttf_t* ttf);
static void read_hmtx(FILE* file, ttf_t* ttf);
static void read_cmap(FILE* file, ttf_t* ttf);

bool ttf_load(const char* filepath, ttf_t* ttf)
{
    // open the file
    FILE* file = fopen(filepath, "rb");

    // load the table directory
    fread(&ttf->table_directory, sizeof(ttf_table_directory_t), 1, file);
    ttf->table_directory.num_tables = TTF_ENDIAN_WORD(ttf->table_directory.num_tables);

    read_header(file, ttf);

    read_head(file, ttf);
    read_maxp(file, ttf);
    read_loca(file, ttf);
    read_glyf(file, ttf);
    read_hhea(file, ttf);
    read_hmtx(file, ttf);
    read_cmap(file, ttf);

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
            case TTF_HHEA_TAG:
                ttf->hhea_table = header;
                break;
            case TTF_HMTX_TAG:
                ttf->hmtx_table = header;
                break;
            case TTF_CMAP_TAG:
                ttf->cmap_table = header;
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

    ttf->maxp.version_maj = TTF_ENDIAN_WORD(ttf->maxp.version_maj);
    ttf->maxp.version_min = TTF_ENDIAN_WORD(ttf->maxp.version_min);

    ttf->maxp.numGlyphs = TTF_ENDIAN_WORD(ttf->maxp.numGlyphs);
    ttf->maxp.maxPoints = TTF_ENDIAN_WORD(ttf->maxp.maxPoints);
    ttf->maxp.maxContours = TTF_ENDIAN_WORD(ttf->maxp.maxContours);
    ttf->maxp.maxComponentPoints = TTF_ENDIAN_WORD(ttf->maxp.maxComponentPoints);
    ttf->maxp.maxComponentContours = TTF_ENDIAN_WORD(ttf->maxp.maxComponentContours);
    ttf->maxp.maxZones = TTF_ENDIAN_WORD(ttf->maxp.maxZones);
    ttf->maxp.maxTwilightPoints = TTF_ENDIAN_WORD(ttf->maxp.maxTwilightPoints);
    ttf->maxp.maxStorage = TTF_ENDIAN_WORD(ttf->maxp.maxStorage);
    ttf->maxp.maxFunctionDefs = TTF_ENDIAN_WORD(ttf->maxp.maxFunctionDefs);
    ttf->maxp.maxInstructionDefs = TTF_ENDIAN_WORD(ttf->maxp.maxInstructionDefs);
    ttf->maxp.maxStackElements = TTF_ENDIAN_WORD(ttf->maxp.maxStackElements);
    ttf->maxp.maxSizeOfInstructions = TTF_ENDIAN_WORD(ttf->maxp.maxSizeOfInstructions);
    ttf->maxp.maxComponentElements = TTF_ENDIAN_WORD(ttf->maxp.maxComponentElements);
    ttf->maxp.maxComponentDepth = TTF_ENDIAN_WORD(ttf->maxp.maxComponentDepth);
}

static void read_head(FILE* file, ttf_t* ttf)
{
    // seek to the table
    fseek(file, ttf->head_table.offset, SEEK_SET);
    
    // read the table
    fread(&ttf->head, sizeof(ttf_head_t), 1, file);

    ttf->head.version_maj = TTF_ENDIAN_WORD(ttf->head.version_maj);
    ttf->head.version_min = TTF_ENDIAN_WORD(ttf->head.version_min);

    ttf->head.font_revision_maj = TTF_ENDIAN_WORD(ttf->head.font_revision_maj);
    ttf->head.font_revision_min = TTF_ENDIAN_WORD(ttf->head.font_revision_min);

    ttf->head.checksumAdjust = TTF_ENDIAN_DWORD(ttf->head.checksumAdjust);
    ttf->head.magic = TTF_ENDIAN_DWORD(ttf->head.magic);
    ttf->head.flags = TTF_ENDIAN_WORD(ttf->head.flags);
    ttf->head.upem = TTF_ENDIAN_WORD(ttf->head.upem);

    ttf->head.xmin = TTF_ENDIAN_WORD(ttf->head.xmin);
    ttf->head.ymin = TTF_ENDIAN_WORD(ttf->head.ymin);
    ttf->head.xmax = TTF_ENDIAN_WORD(ttf->head.xmax);
    ttf->head.ymax = TTF_ENDIAN_WORD(ttf->head.ymax);

    ttf->head.mac_style = TTF_ENDIAN_WORD(ttf->head.mac_style);
    ttf->head.lowest_rec_ppem = TTF_ENDIAN_WORD(ttf->head.lowest_rec_ppem);
    ttf->head.direction_hint = TTF_ENDIAN_WORD(ttf->head.direction_hint);
    ttf->head.index_to_loc_format = TTF_ENDIAN_WORD(ttf->head.index_to_loc_format);
    ttf->head.glyph_data_format = TTF_ENDIAN_WORD(ttf->head.glyph_data_format);
}

static void read_loca(FILE* file, ttf_t* ttf)
{
    // seek to the table
    fseek(file, ttf->loca_table.offset, SEEK_SET);

    // alocate the array
    ttf->glyf_offsets = malloc((ttf->maxp.numGlyphs + 1) * sizeof(uint32_t));

    // get array size
    size_t size = ttf->maxp.numGlyphs + 1;
    size *= ttf->head.index_to_loc_format == 0 ? sizeof(uint16_t) : sizeof(uint32_t);
    void* tmp_arr = malloc(size);

    // read the table
    fread(tmp_arr, size, 1, file);

    for(size_t i = 0; i < ttf->maxp.numGlyphs + 1; i++)
    {
        uint32_t val;
        if(ttf->head.index_to_loc_format == 0)
            val = (uint32_t)TTF_ENDIAN_WORD(((uint16_t*)tmp_arr)[i]) * 2;
        else
            val = TTF_ENDIAN_DWORD(((uint32_t*)tmp_arr)[i]);
        ttf->glyf_offsets[i] = val;
    }
}

static void read_glyf(FILE* file, ttf_t* ttf)
{
    ttf->glyfs = (ttf_glyf_t*)malloc(sizeof(ttf_glyf_t) * (ttf->maxp.numGlyphs));

    for(int i = 0; i < ttf->maxp.numGlyphs; i++)
    {
        ttf_glyf_t* glyf = &(ttf->glyfs[i]);

        // skip null gfyfs
        if(ttf->glyf_offsets[i] == ttf->glyf_offsets[i + 1])
        {
            memset(glyf, 0, sizeof(ttf_glyf_t));
            continue;
        }
        // seek to the offset of the glyf
        fseek(file,  ttf->glyf_table.offset + ttf->glyf_offsets[i], SEEK_SET);

        // read the header
        ttf_glyf_header_t* header = &(glyf->header);
        fread(header, sizeof(ttf_glyf_header_t), 1, file);
        header->numberOfContours = TTF_ENDIAN_WORD(header->numberOfContours);
        header->xMin = TTF_ENDIAN_WORD(header->xMin);
        header->yMin = TTF_ENDIAN_WORD(header->yMin);
        header->xMax = TTF_ENDIAN_WORD(header->xMax);
        header->yMax = TTF_ENDIAN_WORD(header->yMax);

        if(header->numberOfContours < 0) // ignore compund glyf at the moment
        {
            printf("Skipped glyf %d\n", i);
            continue;
        }

        // read the descriptor
        ttf_glyf_descriptor_t* descriptor = &ttf->glyfs[i].descriptor;

        // read the end points
        descriptor->endPtsOfContours = (uint16_t*)malloc(sizeof(uint16_t) * header->numberOfContours);
        fread(descriptor->endPtsOfContours, sizeof(uint16_t), header->numberOfContours, file);
        for(int j = 0; j < header->numberOfContours; j++)
            descriptor->endPtsOfContours[j] = TTF_ENDIAN_WORD(descriptor->endPtsOfContours[j]);

        // read instruction length
        fread(&descriptor->instructionLength, sizeof(uint16_t), 1, file);
        descriptor->instructionLength = TTF_ENDIAN_WORD(descriptor->instructionLength);

        // the number of points is deterimend by the last entry in endPtsOfContours + 1
        size_t num_points = descriptor->endPtsOfContours[header->numberOfContours - 1] + 1;
        if(num_points <= 0)
            continue;

        // read instructions
        descriptor->instructions = (uint8_t*)malloc(sizeof(uint8_t) * descriptor->instructionLength);
        fread(descriptor->instructions, sizeof(uint8_t), descriptor->instructionLength, file);
        // ^ no need to change endian for a single byte ^

        // read flags
        descriptor->flags = (uint8_t*)malloc(sizeof(uint8_t) * num_points);
        uint8_t* flagbuff = (uint8_t*)malloc(sizeof(uint8_t) * num_points);
        long  flagpos = ftell(file);
        fread(flagbuff, sizeof(uint8_t), num_points, file);
        size_t flagindex = 0;
        for(size_t j = 0; j < num_points;)
        {
            uint8_t flag = flagbuff[flagindex++];
            descriptor->flags[j++] = flag;
            if(flag & TTF_FLAG_REPEAT)
            {
                uint8_t repeat_amount = flagbuff[flagindex++];
                for(uint8_t rt = 0; rt < repeat_amount; rt++)
                    descriptor->flags[j++] = flag;
            }
        }
        free(flagbuff);
        fseek(file, flagpos + flagindex, SEEK_SET);
        // ^ no need to change endian for a single byte ^

        descriptor->xCoordinates = malloc(sizeof(int16_t) * num_points);
        descriptor->yCoordinates = malloc(sizeof(int16_t) * num_points);

        uint8_t* coords = malloc(sizeof(uint8_t) * (num_points << 2));
        fread(coords, sizeof(uint8_t), num_points << 2, file);

        uint32_t pointindex = 0;
        int16_t last_point = 0;
        
        // xpass
        for (size_t j = 0; j < num_points; j++)
        {
            bool is_short = descriptor->flags[j] & TTF_XSHORT;
            bool is_prev = descriptor->flags[j] & TTF_XREPEAT;
            
            if (is_short)
            {
                if (is_prev)
                {
                    last_point += coords[pointindex++];
                    descriptor->xCoordinates[j] = last_point;
                } else
                {
                    last_point -= coords[pointindex++];
                    descriptor->xCoordinates[j] = last_point;
                }
            }
            else
            {
                if (is_prev)
                    descriptor->xCoordinates[j] = last_point;
                else
                {
                    last_point += (int16_t)(coords[pointindex + 1] | (coords[pointindex] << 8));
                    pointindex += 2;
                    descriptor->xCoordinates[j] = last_point;
                }
            }
        }

        last_point = 0;

        // ypass
        for (size_t j = 0; j < num_points; j++)
        {
            bool is_short = descriptor->flags[j] & TTF_YSHORT;
            bool is_prev = descriptor->flags[j] & TTF_YREPEAT;
            
            if (is_short)
            {
                if (is_prev)
                {
                    last_point += coords[pointindex++];
                    descriptor->yCoordinates[j] = last_point;
                } else
                {
                    last_point -= coords[pointindex++];
                    descriptor->yCoordinates[j] = last_point;
                }
            }
            else
            {
                if (is_prev)
                    descriptor->yCoordinates[j] = last_point;
                else
                {
                    last_point += (int16_t)(coords[pointindex + 1] | (coords[pointindex] << 8));
                    pointindex += 2;
                    descriptor->yCoordinates[j] = last_point;
                }
            }
        }
    }
}


static void read_hhea(FILE* file, ttf_t* ttf)
{
    // seek to the table
    fseek(file, ttf->hhea_table.offset, SEEK_SET);

    // read the table
    fread(&ttf->hhea, sizeof(ttf_hhea_t), 1, file);

    ttf->hhea.version_maj = TTF_ENDIAN_WORD(ttf->hhea.version_maj);
    ttf->hhea.version_min = TTF_ENDIAN_WORD(ttf->hhea.version_min);
    ttf->hhea.ascent = TTF_ENDIAN_WORD(ttf->hhea.ascent);
    ttf->hhea.descent = TTF_ENDIAN_WORD(ttf->hhea.descent);
    ttf->hhea.lineGap = TTF_ENDIAN_WORD(ttf->hhea.lineGap);
    ttf->hhea.advanceWidthMax = TTF_ENDIAN_WORD(ttf->hhea.advanceWidthMax);
    ttf->hhea.minLeftSideBearing = TTF_ENDIAN_WORD(ttf->hhea.minLeftSideBearing);
    ttf->hhea.minRightSideBearing = TTF_ENDIAN_WORD(ttf->hhea.minRightSideBearing);
    ttf->hhea.xMaxExtent = TTF_ENDIAN_WORD(ttf->hhea.xMaxExtent);
    ttf->hhea.caretSlopeRise = TTF_ENDIAN_WORD(ttf->hhea.caretSlopeRise);
    ttf->hhea.caretSlopeRun = TTF_ENDIAN_WORD(ttf->hhea.caretSlopeRun);
    ttf->hhea.caretOffset = TTF_ENDIAN_WORD(ttf->hhea.caretOffset);
    ttf->hhea.metricDataFormat = TTF_ENDIAN_WORD(ttf->hhea.metricDataFormat);
    ttf->hhea.numberOfLongHorMetrics = TTF_ENDIAN_WORD(ttf->hhea.numberOfLongHorMetrics);
}

static void read_hmtx(FILE* file, ttf_t* ttf)
{
    // seek to the table
    fseek(file, ttf->hmtx_table.offset, SEEK_SET);

    ttf->metrics = malloc(sizeof(longHorMetric_t) * ttf->hhea.numberOfLongHorMetrics);
    fread(ttf->metrics, sizeof(longHorMetric_t), ttf->hhea.numberOfLongHorMetrics, file);

    for(int i = 0; i < ttf->hhea.numberOfLongHorMetrics; i++)
    {
        ttf->metrics[i].advanceWidth = TTF_ENDIAN_WORD(ttf->metrics[i].advanceWidth);
        ttf->metrics[i].leftSideBearing = TTF_ENDIAN_WORD(ttf->metrics[i].leftSideBearing);
    }
}

static void read_cmap(FILE* file, ttf_t* ttf)
{
    // seek to the table
    fseek(file, ttf->cmap_table.offset, SEEK_SET);

    // read intex
    cmap_index_t index;
    fread(&index, sizeof(cmap_index_t), 1, file);
    index.version = TTF_ENDIAN_WORD(index.version);
    index.numberSubtables = TTF_ENDIAN_WORD(index.numberSubtables);

    // read subtables
    for(int i = 0; i < index.numberSubtables; i++)
    {
        cmap_subtable_header_t header;
        fread(&header, sizeof(cmap_subtable_header_t), 1, file);

        header.platformID = TTF_ENDIAN_WORD(header.platformID);
        header.platformSpecificID = TTF_ENDIAN_WORD(header.platformSpecificID);
        header.offset = TTF_ENDIAN_DWORD(header.offset);

        if(header.platformID == TTF_PLATFORM_UNICODE && header.platformSpecificID == TTF_UNICODE_2_0_BMP_ONLY)
        {
            // preserve file offset
            long advance = ftell(file);
            // seek to the format table
            fseek(file, ttf->cmap_table.offset + header.offset, SEEK_SET);

            // read the format
            uint16_t format;
            fread(&format, sizeof(uint16_t), 1, file);
            format = TTF_ENDIAN_WORD(format);

            if(format != 4)
            {
                // restore file offset
                fseek(file, advance, SEEK_SET);
                continue;
            }

            // go back to the start of the format table
            fseek(file, ttf->cmap_table.offset + header.offset, SEEK_SET);

            ttf_cmap4_header_t cmap4_header;
            fread(&cmap4_header, sizeof(ttf_cmap4_header_t), 1, file);
            cmap4_header.segCountX2 = TTF_ENDIAN_WORD(cmap4_header.segCountX2);
            
            uint16_t endCode[cmap4_header.segCountX2 / 2];
            uint16_t startCode[cmap4_header.segCountX2 / 2];
            uint16_t idDelta[cmap4_header.segCountX2 / 2];
            uint16_t idRangeOffset[cmap4_header.segCountX2 / 2];
            uint16_t padding;

            fread(endCode, sizeof(uint16_t), cmap4_header.segCountX2 / 2, file);
            fread(&padding, sizeof(uint16_t), 1, file);
            fread(startCode, sizeof(uint16_t), cmap4_header.segCountX2 / 2, file);
            fread(idDelta, sizeof(uint16_t), cmap4_header.segCountX2 / 2, file);
            fread(idRangeOffset, sizeof(uint16_t), cmap4_header.segCountX2 / 2, file);

            for(int j = 0; j < cmap4_header.segCountX2 / 2; j++)
            {
                endCode[j] = TTF_ENDIAN_WORD(endCode[j]);
                startCode[j] = TTF_ENDIAN_WORD(startCode[j]);
                idDelta[j] = TTF_ENDIAN_WORD(idDelta[j]);
                idRangeOffset[j] = TTF_ENDIAN_WORD(idRangeOffset[j]);
            }

            // map individual characters
            for(int c = 0; c < UINT16_MAX; c++)
            {
                int idx;
                for(idx = 0; idx < cmap4_header.segCountX2 / 2; idx++)
                    if(endCode[idx] >= c && startCode[idx] <= c)
                        break;
                // character not present, map to null-glyph
                if(idx == cmap4_header.segCountX2 / 2)
                {
                    ttf->mapping[c] = 0;
                    continue;
                }

                if(idRangeOffset[idx] == 0)
                    ttf->mapping[c] = c + idDelta[idx];
                // else
                // {
                //     int offset = idRangeOffset[idx] / 2 + (c - startCode[idx]);
                //     ttf->mapping[c] = startCode[idx] + idDelta[idx] + idRangeOffset[idx] / 2 + (c - startCode[idx]);
                // }
            }

            return;
        }
    }

    printf("No supported translation table in cmap\n");
    exit(-1);
}
#include <rund/renderer.h>
#include <rund/ttf/ttf.h>

#include <stdlib.h>
#include <string.h>

static ttf_t ttf;

void renderer_init()
{
    ttf_load("Monaco.ttf", &ttf);
}

void plot_pixel(const buffer_t* buffer, uint64_t x, uint64_t y, const color_t color)
{
    if(x >= 0 && x < buffer->width && y >= 0 && y < buffer->height)
        buffer->data[y * buffer->width + x] = color;
}

uint64_t get_pixel(const buffer_t* buffer, uint64_t x, uint64_t y)
{
    if(x >= 0 && x < buffer->width && y >= 0 && y < buffer->height)
        return buffer->data[y * buffer->width + x];
    return 0;
}

void draw_line(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, color_t color, const buffer_t* buffer)
{
    int32_t x, y, xe, ye;

    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;
    int32_t dx1 = abs(dx);
    int32_t dy1 = abs(dy);
    int32_t px = 2 * dy1 - dx1;
    int32_t py = 2 * dx1 - dy1;

    if (dy1 <= dx1)
    {
        if (dx >= 0)
        {
            x = x0;
            y = y0;
            xe = x1;
        }
        else
        {
            x = x1;
            y = y1;
            xe = x0;
        }

        plot_pixel(buffer, x, y, color);
        
        for (int32_t i = 0; x < xe; i++)
        {
            x = x + 1;
            if (px < 0)
                px = px + 2 * dy1;
            else
            {
                if ((dx < 0 && dy<0) || (dx > 0 && dy > 0))
                    y = y + 1;
                else
                    y = y - 1;
                px = px + 2 * (dy1 - dx1);
            }
            plot_pixel(buffer, x, y, color);
        }
    }
    else
    {
        if (dy >= 0)
        {
            x = x0;
            y = y0;
            ye = y1;
        }
        else
        {
            x = x1;
            y = y1;
            ye = y0;
        }

        plot_pixel(buffer, x, y, color);

        for (int32_t i = 0; y < ye; i++)
        {
            y = y + 1;
            if (py <= 0)
                py = py + 2 * dx1;
            else
            {
                if ((dx<0 && dy<0) || (dx>0 && dy>0))
                    x = x + 1;
                else
                    x = x - 1;
                py = py + 2 * (dx1 - dy1);
            }

            plot_pixel(buffer, x, y, color);
        }
    }
}

// void fillGlyphInside

void boundaryFill4(const buffer_t* buffer, uint64_t x, uint64_t y, color_t boundaryColor, color_t fillColor)
{
    if(x >= 0 && x < buffer->width && y >= 0 && y < buffer->height)
    {
        color_t color = buffer->data[y * buffer->width + x];
        if(color != boundaryColor && color != fillColor)
        {
            plot_pixel(buffer, x, y, fillColor);
            boundaryFill4(buffer, x + 1, y, boundaryColor, fillColor);
            boundaryFill4(buffer, x - 1, y, boundaryColor, fillColor);
            boundaryFill4(buffer, x, y + 1, boundaryColor, fillColor);
            boundaryFill4(buffer, x, y - 1, boundaryColor, fillColor);
        }
    }
}

draw_data_t draw_character(const buffer_t* buffer, color_t color, char character, uint64_t transX, uint64_t transY, size_t scale)
{
    draw_data_t data = NULL_DRAW;
    data.coords.x = transX;

    uint64_t scaleX = buffer->width / scale;
    uint64_t scaleY = buffer->height / scale;
    scale = scaleX < scaleY ? scaleX : scaleY;

    const uint16_t glyf_id = ttf.mapping[character];
    const ttf_glyf_t* glyf = &ttf.glyfs[glyf_id];

    uint64_t offX = glyf->header.xMin < 0 ? -glyf->header.xMin : 0;
    uint64_t offY = glyf->header.yMin < 0 ? -glyf->header.yMin : 0;

    transY += abs(abs(ttf.head.ymax) - abs(ttf.head.ymin));
    transY += offY;
    transY /= scale;
    transY = buffer->height - transY;

    buffer_t tmp_buffer = buffer_create(buffer->width, buffer->height);
    memset(tmp_buffer.data, 0, tmp_buffer.width * tmp_buffer.height * sizeof(color_t));

    uint64_t point = 0;
    for(int c = 0; c < glyf->header.numberOfContours; c++)
    {
        size_t num_points = glyf->descriptor.endPtsOfContours[c] + 1 - point;
        uint64_t xPoints[num_points];
        uint64_t yPoints[num_points];

        for(size_t p = 0; p < num_points; p++)
        {
            uint64_t x = (glyf->descriptor.xCoordinates[p + point] + offX) / scale;
            uint64_t y = (glyf->descriptor.yCoordinates[p + point] + offY) / scale;

            xPoints[p] = x + transX;
            yPoints[p] = buffer->height - (y + transY);
        }

        for(size_t i = 0; i < num_points - 1; i++)
            draw_line(xPoints[i], yPoints[i], xPoints[i + 1], yPoints[i + 1], color, &tmp_buffer);
        draw_line(xPoints[0], yPoints[0], xPoints[num_points - 1], yPoints[num_points - 1], color, &tmp_buffer);

        point = glyf->descriptor.endPtsOfContours[c] + 1;
    }

    for(uint64_t y = 0; y < buffer->height; y++)
    {
        bool state = false;
        for(uint64_t x = 0; x < buffer->width; x++)
        {
            if(tmp_buffer.data[y * buffer->width + x] & 0xFF000000)
            {
                while(x < buffer->width && (tmp_buffer.data[y * buffer->width + x] & 0xFF000000))
                    x++;
                state = !state;
                if(state)
                {
                    uint64_t tmp_x = x;
                    while(tmp_x < buffer->width && !(tmp_buffer.data[y * buffer->width + tmp_x] & 0xFF000000))
                        tmp_x++;
                    if(tmp_x == buffer->width)
                        break;
                }
            }
            tmp_buffer.data[y * buffer->width + x] = state ? color : 0;
        }
    }

    for(size_t i = 0; i < buffer->width * buffer->height; i++)
        if(tmp_buffer.data[i] & 0xFF000000)
            buffer->data[i] = tmp_buffer.data[i]; //todo alpha blending

    buffer_destroy(&tmp_buffer);

    data.dimensions.width = ttf.metrics[glyf_id].advanceWidth / scale;
    data.dimensions.height = (abs(ttf.head.ymax) + abs(ttf.head.ymin)) / scale;
    return data;
}
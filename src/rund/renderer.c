#include <rund/rendering.h>

#include "rund/renderer.h"
#include "rund/ttf/ttf.h"

#include <stdlib.h>
#include <string.h>

typedef struct point
{
    float x;
    float y;
} point_t;

static ttf_t ttf;

void renderer_init()
{
    ttf_load("Monaco.ttf", &ttf);
}

void plot_pixel(const buffer_t* buffer, color_t color, uint64_t x, uint64_t y)
{
    if(x >= 0 && x < buffer->width && y >= 0 && y < buffer->height)
        buffer->data[y * buffer->width + x] = color;
}

void draw_line(const buffer_t* buffer, color_t color, uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1)
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

        plot_pixel(buffer, color, x, y);
        
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
            plot_pixel(buffer, color, x, y);
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

        plot_pixel(buffer, color, x, y);

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

            plot_pixel(buffer, color, x, y);
        }
    }
}

static uint64_t bezier_coord(float t, float p0, float p1, float p2)
{
    return (p0 - 2 * p1 + p2) * t * t + (2 * p1 - 2 * p0) * t + p0;
}

static point_t bezier_point(point_t points[3], float t)
{
    return (point_t){
        .x = bezier_coord(t, points[0].x, points[1].x, points[2].x),
        .y = bezier_coord(t, points[0].y, points[1].y, points[2].y)
    };
}

static void bezier_curve(point_t cpoints[3], size_t num_points, point_t* result)
{
    float t = 1.0 / (num_points - 1);
    for(size_t i = 0; i < num_points; i++)
        result[i] = bezier_point(cpoints, i * t);
}

static void process_bezier(point_t cpoints[3], color_t color, const buffer_t* buffer)
{
    #define BEZIER_POINTS 100

    point_t points[BEZIER_POINTS];
    bezier_curve(cpoints, BEZIER_POINTS, points);
    
    for(size_t i = 0; i < BEZIER_POINTS; i++)
        plot_pixel(buffer, color, points[i].x, points[i].y);

    #undef BEZIER_POINTS
}

draw_data_t draw_character(const buffer_t* buffer, color_t color, wchar_t character, uint64_t transX, uint64_t transY, float scale)
{
    draw_data_t data = NULL_DRAW;
    data.coords.x = transX;

    scale *= 0.001; // scaling factor

    const uint16_t glyf_id = ttf.mapping[character];
    const ttf_glyf_t* glyf = &ttf.glyfs[glyf_id];
 
    uint64_t offX = glyf->header.xMin < 0 ? -glyf->header.xMin : 0;
    uint64_t offY = glyf->header.yMin < 0 ? -glyf->header.yMin : 0;

    transY += abs(ttf.head.ymax) + abs(ttf.head.ymin);
    transY += offY; 
    transY *= scale;
    transY = buffer->height - transY;

    uint8_t tmp_buffer[buffer->width * buffer->height];
    memset(tmp_buffer, 0, buffer->width * buffer->height);

    uint64_t point = 0;
    for(int c = 0; c < glyf->header.numberOfContours; c++)
    {
        buffer_t contour_buffer = buffer_create(buffer->width, buffer->height);

        size_t num_points = glyf->descriptor.endPtsOfContours[c] + 1 - point;
        uint64_t xPoints[num_points];
        uint64_t yPoints[num_points];
        bool onCurve[num_points];

        for(size_t p = 0; p < num_points; p++)
        {
            uint64_t x = (glyf->descriptor.xCoordinates[p + point] + offX) * scale;
            uint64_t y = (glyf->descriptor.yCoordinates[p + point] + offY) * scale;

            xPoints[p] = x + transX;
            yPoints[p] = buffer->height - (y + transY);

            onCurve[p] = glyf->descriptor.onCurve[p + point];
        }

        point = glyf->descriptor.endPtsOfContours[c] + 1;

        uint64_t first = 0;
        while(!onCurve[first])
            first++;
        point_t last_oncurve = {.x = xPoints[first], .y = yPoints[first]};
        uint64_t used = 0;
        for(size_t i = first + 1; i < num_points + first; i++)
        {
            point_t point = {.x = xPoints[i], .y = yPoints[i]};

            if(onCurve[i % num_points])
            {
                draw_line(&contour_buffer, color, last_oncurve.x, last_oncurve.y, point.x, point.y);
                last_oncurve = point;
                continue;
            }

            point_t cpoints[3] = {
                {.x = last_oncurve.x, .y = last_oncurve.y},
                {.x = point.x, .y = point.y},
                {.x = xPoints[(i + 1) % num_points], .y = yPoints[(i + 1) % num_points]}
            };
            if(!onCurve[(i + 1) % num_points]) {
                cpoints[2].x = cpoints[1].x + (cpoints[2].x - cpoints[1].x) / 2;
                cpoints[2].y = cpoints[1].y + (cpoints[2].y - cpoints[1].y) / 2;
            }
            last_oncurve = cpoints[2];
            process_bezier(cpoints, color, &contour_buffer);
        }
        draw_line(&contour_buffer, color, last_oncurve.x, last_oncurve.y, xPoints[first], yPoints[first]);

        for(uint64_t y = 0; y < buffer->height; y++)
            for(uint64_t x = 0; x < buffer->width; x++)
                if(contour_buffer.data[y * buffer->width + x].a & 0xFF)
                    tmp_buffer[y * buffer->width + x] += 1;

        buffer_destroy(&contour_buffer);
    }

    for(size_t i = 0; i < buffer->width * buffer->height; i++)
        if(tmp_buffer[i] % 2 == 1)
            buffer->data[i] = blend(buffer->data[i], color);

    data.dims.width = ttf.metrics[glyf_id].advanceWidth * scale;
    data.dims.height = (abs(ttf.head.ymax) + abs(ttf.head.ymin) + offY) * scale + 1;
    return data;
}
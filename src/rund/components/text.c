#include <rund.h>
#include <rund/memory/gc.h>
#include <rund/log.h>
#include <rund/utils/max.h>
#include <rund/renderer.h>

#include <wchar.h>
#include <string.h>

static draw_data_t draw_text(const text_t* text, const build_context_t context, uint64_t deepness);

text_t* text_create(text_attributes_t attributes)
{
    text_t* component = (text_t*)gc_alloc(sizeof(text_t));

	component->attributes.text = attributes.text;
    component->attributes.font_size = attributes.font_size ?: Val((float)14.0f);

    component->base.draw_func = DrawFunc(draw_text);
    component->base.flags = FLAG_NONE;
	memcpy(component->base.id, attributes.id, ID_LEN);

	TRACE("Component Created {TEXT}\t[%s]", 0, attributes.id);
	return component;
}

static draw_data_t draw_text(const text_t* text, const build_context_t context, uint64_t deepness)
{
    draw_data_t data = NULL_DRAW;

    const text_attributes_t* attributes = &(text->attributes);

    buffer_t chars_buffer = buffer_create(context.max_width, context.max_height);

    for(int i = 0; i < wcslen(attributes->text); i++)
    {
        draw_data_t char_data = draw_character(&chars_buffer, 0xFFFFFFFF, attributes->text[i], data.dims.width, 0, *attributes->font_size);
        
        size_t char_width = data.dims.width - char_data.coords.x + char_data.dims.width;
        data.dims.width += char_width;
        
        size_t char_height = char_data.dims.height;
        data.dims.height = max(char_height, data.dims.height);
    }

    for(size_t x = 0; x < chars_buffer.width; x++)
        for(size_t y = 0; y < chars_buffer.height; y++)
        {
            color_t* bg_pixel = &context.backbuffer.data[y * context.backbuffer.width + x];
            color_t* char_pixel = &chars_buffer.data[y * chars_buffer.width + x];
            *bg_pixel = blend(*bg_pixel, *char_pixel);
        }

    buffer_destroy(&chars_buffer);

    if(data.coords.x + data.dims.width > context.max_width)
        data.dims.width = context.max_width - data.coords.x;
    if(data.coords.y + data.dims.height > context.max_height)
        data.dims.height = context.max_height - data.coords.y;

    return data;
}
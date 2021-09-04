#include <rund.h>
#include "wrapper.h"
#include <stdlib.h>
#include <string.h>
#include "log.h"

draw_data_t draw_component(const component_t* component, const build_context_t context);
draw_data_t draw_container(const container_t* container, const build_context_t context);
draw_data_t draw_row(const row_t* row, const build_context_t context);
draw_data_t draw_expanded(const expanded_t* expanded, const build_context_t context);
draw_data_t draw_align(const align_t* align, const build_context_t context);
draw_data_t draw_center(const center_t* center, const build_context_t context);
draw_data_t draw_constrained_box(const constrained_box_t* constrained_box, const build_context_t context);

void run_app(const rund_app_t* app)
{
	create_window(app);

	do
	{
		build_context_t context;
		context.max_width = app->width;
		context.max_height = app->height;
		context.min_width = context.min_height = 0;
		context.backbuffer = buffer_create(app->width, app->height);
		draw_component(app->root, context);

		for(uint64_t y = 0; y < app->height; y++)
			for(uint64_t x = 0; x < app->width; x++)
				put_pixel(x, y, context.backbuffer.data[y * app->width + x]);

		buffer_destroy(&context.backbuffer);

		flush();
	} while (event_loop());

	TRACE("Rund app terminated\n", 0);
}

draw_data_t draw_component(const component_t* component, const build_context_t context)
{
	draw_data_t data = NULL_DRAW;

	if(component)
		switch (component->type)
		{
		case EMPTY:
			break;
		case CONTAINER:
			data = draw_container((const container_t*)component, context);
			break;
		case ROW:
			data = draw_row((const row_t*)component, context);
			break;
		case EXPANDED:
			data = draw_expanded((const expanded_t*)component, context);
			break;
		case ALIGN:
			data = draw_align((const align_t*)component, context);
			break;
		case CENTER:
			data = draw_center((const center_t*)component, context);
			break;
		case CONSTRAINED_BOX:
			data = draw_constrained_box((const constrained_box_t*)component, context);
			break;
		default:
			ERROR("Unrecognized component type: [%d]", 0, (int)component->type);
			break;
		}

	return data;
}

draw_data_t draw_container(const container_t* container, const build_context_t context)
{
	draw_data_t data;
	data.dimensions.height = (container->height > context.max_height) ? context.max_height : (container->height < context.min_height) ? context.min_height : container->height;
	data.dimensions.width = (container->width > context.max_width) ? context.max_width : (container->width < context.min_width) ? context.min_width : container->width;
	data.coords.x = 0;
	data.coords.y = 0;

	for (uint64_t y = 0; y < data.dimensions.height; y++)
		for (uint64_t x = 0; x < data.dimensions.width; x++)
			context.backbuffer.data[y *context.backbuffer.width + x] = container->decoration.color;

	build_context_t child_context = context;
	child_context.max_width = container->width;
	child_context.max_height = container->height;
	draw_component(container->child, child_context);

	return data;
}

draw_data_t draw_row(const row_t* row, const build_context_t context)
{
	draw_data_t data = { .coords = { 0, 0 }, .dimensions = { context.max_width, context.max_height } };
	
	// decorations
	for(uint64_t y = 0; y < context.max_height; y++)
		for(uint64_t x = 0; x < context.max_width; x++)
			context.backbuffer.data[y * context.backbuffer.width + x] = row->decoration.color;

	buffer_t backbuffers[row->children_count];
	draw_data_t datas[row->children_count];

	uint64_t unflexible_width = 0;
	uint64_t flexible_count = 0;

	// draw unflexibles
	for(uint64_t i = 0; i < row->children_count; i++)
	{
		component_t* child = row->children[i];
		if(child->isFlexible)
		{
			flexible_count += ((flexible_t*)child)->flex;
			continue;
		}

		build_context_t child_context = context;
		child_context.max_width -= unflexible_width;
		backbuffers[i] = buffer_create(child_context.max_width, child_context.max_height);
		child_context.backbuffer = backbuffers[i];

		datas[i] = draw_component(child, child_context);

		unflexible_width += datas[i].dimensions.width;
	}

	uint64_t left_width = context.max_width - unflexible_width;

	// draw flexibles
	for(uint64_t i = 0; i < row->children_count; i++)
	{
		flexible_t* child = (flexible_t*)row->children[i];
		if(!child->isFlexible)
			continue;

		dimension_t space;
		space.width = (uint64_t)(((double)left_width * (double)child->flex) / (double)flexible_count + 0.5);
		space.height = context.max_height;
		backbuffers[i] = buffer_create(space.width, space.height);

		build_context_t child_context = context;
		child_context.max_width = space.width;
		child_context.max_height = space.height;
		child_context.backbuffer = backbuffers[i];

		datas[i] = draw_component((component_t*)child, child_context);
	}

	uint64_t advance = 0;

	// render childs
	buffer_t* main_buffer = &context.backbuffer;
	for(uint64_t i = 0; i < row->children_count; i++)
	{
		buffer_t* child_buffer = &backbuffers[i];
		uint64_t y_offset = datas[i].coords.y;
		uint64_t x_offset = datas[i].coords.x;

		for (uint64_t y = 0; y < datas[i].dimensions.height; y++)
			for (uint64_t x = 0; x < datas[i].dimensions.width; x++)
			{
				color_t* main_pixel = &main_buffer->data[(y + y_offset) * main_buffer->width + (x + advance)]; 
				color_t* child_pixel = &child_buffer->data[(y + y_offset) * child_buffer->width + (x + x_offset)];
				*main_pixel = *child_pixel;
			}

		advance += datas[i].dimensions.width;
	
		buffer_destroy(child_buffer);
	}

	return data;
}

draw_data_t draw_expanded(const expanded_t* expanded, build_context_t context)
{
	build_context_t child_context = context;
	child_context.min_height = child_context.max_height;
	child_context.min_width = child_context.max_width;

	return draw_component(expanded->child, child_context);
}

draw_data_t draw_align(const align_t* align, const build_context_t context)
{
	draw_data_t data = NULL_DRAW;

	buffer_t backbuffer = buffer_create(context.max_width, context.max_height);

	build_context_t child_context = context;
	child_context.backbuffer = backbuffer;

	draw_data_t child_data = draw_component(align->child, child_context);

	float align_x = (align->alignment.x + 1) / 2;
	float align_y = (align->alignment.y + 1) / 2;

	uint64_t xp = context.max_width * align_x;
	uint64_t yp = context.max_height * align_y;

	uint64_t xc = child_data.dimensions.width * align_x;
	uint64_t yc = child_data.dimensions.height * align_y;

	buffer_t* main_buffer = &context.backbuffer;
	uint64_t y_offset = yp - yc;
	uint64_t x_offset = xp - xc;

	for (uint64_t y = 0; y < child_data.dimensions.height; y++)
		for (uint64_t x = 0; x < child_data.dimensions.width; x++)
		{
			color_t* main_pixel = &main_buffer->data[(y_offset + y) * main_buffer->width + (x_offset + x)];
			color_t* child_pixel = &backbuffer.data[y * backbuffer.width + x];
			*main_pixel = *child_pixel;
		}

	buffer_destroy(&backbuffer);

	data.dimensions = child_data.dimensions;
	data.coords = (coord_t){ (xp - xc), (yp - yc) };

	return data;
}

draw_data_t draw_center(const center_t* center, const build_context_t context)
{
	return draw_align(&(center->align), context);
}

draw_data_t draw_constrained_box(const constrained_box_t* constrained_box, const build_context_t context)
{
	build_context_t child_context = context;
	child_context.min_height = context.min_height > constrained_box->constraints.min_height ? context.min_height : constrained_box->constraints.min_height;
	child_context.min_width = context.min_width > constrained_box->constraints.min_width ? context.min_width : constrained_box->constraints.min_width;
	child_context.max_height = context.max_height < constrained_box->constraints.max_height ? context.max_height : constrained_box->constraints.max_height;
	child_context.max_width = context.max_width < constrained_box->constraints.max_width ? context.max_width : constrained_box->constraints.max_width;
	
	return draw_component(constrained_box->child, child_context);
}
#include "rund.h"
#include "wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

draw_data_t draw_component(const component_t* component, const build_context_t context);
draw_data_t draw_container(const container_t* container, const build_context_t context);
draw_data_t draw_row(const row_t* row, const build_context_t context);
draw_data_t draw_expanded(const expanded_t* row, const build_context_t context);
draw_data_t draw_align(const align_t* row, const build_context_t context);

void run_app(const rund_app_t* app)
{
	create_window(app);

	do
	{
		build_context_t context;
		context.max_width = app->width;
		context.max_height = app->height;
		context.min_width = context.min_height = 0;
		context.backbuffer = calloc(app->width * app->height, sizeof(color_t));
		draw_component(app->root, context);

		for(uint64_t y = 0; y < app->height; y++)
			for(uint64_t x = 0; x < app->width; x++)
				put_pixel(x, y, context.backbuffer[y * app->width + x]);

		free(context.backbuffer);

		flush();
	} while (event_loop());

	printf("Rund app terminated\n");
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
			context.backbuffer[y * data.dimensions.width + x] = container->decoration.color;

	build_context_t child_context = context;
	child_context.max_width = container->width;
	child_context.max_height = container->height;
	draw_component(container->child, child_context);

	return data;
}

draw_data_t draw_row(const row_t* row, const build_context_t context)
{
	draw_data_t data = { .coords = { 0, 0 }, .dimensions = { context.max_width, context.max_height } };
	for(uint64_t y = 0; y < context.max_height; y++)
		for(uint64_t x = 0; x < context.max_width; x++)
			context.backbuffer[y * context.max_width + x] = row->decoration.color;

	color_t* backbuffers[row->children_count];
	draw_data_t datas[row->children_count];

	memset(backbuffers, 0, sizeof(color_t*) * row->children_count);

	dimension_t unflexible_size = { 0, 0 };
	uint64_t flexible_count = 0;

	for(uint64_t i = 0; i < row->children_count; i++)
	{
		component_t* child = row->children[i];
		if(child->isFlexible)
		{
			flexible_count += ((flexible_t*)child)->flex;
			continue;
		}

		backbuffers[i] = calloc(context.max_width * context.max_height, sizeof(color_t));

		build_context_t child_context = context;
		child_context.backbuffer = backbuffers[i];

		datas[i] = draw_component(child, child_context);

		unflexible_size.width += datas[i].dimensions.width;
		unflexible_size.height = datas[i].dimensions.height > unflexible_size.height ? datas[i].dimensions.height : unflexible_size.height;
	}

	dimension_t left_size = { context.max_width - unflexible_size.width, context.max_height };

	for(uint64_t i = 0; i < row->children_count; i++)
	{
		flexible_t* child = (flexible_t*)row->children[i];
		if(!child->isFlexible)
			continue;

		dimension_t space;
		space.width = (uint64_t)(((double)left_size.width * (double)child->flex) / (double)flexible_count + 0.5);
		space.height = left_size.height;
		backbuffers[i] = calloc(space.width * space.height, sizeof(color_t));

		build_context_t child_context = context;
		child_context.max_width = space.width;
		child_context.max_height = space.height;
		child_context.backbuffer = backbuffers[i];

		datas[i] = draw_component((component_t*)child, child_context);
	}

	uint64_t advance = 0;
	for(uint64_t i = 0; i < row->children_count; i++)
	{
		for (uint64_t y = 0; y < datas[i].dimensions.height; y++)
			for (uint64_t x = 0; x < datas[i].dimensions.width; x++)
					context.backbuffer[(y + datas[i].coords.y) * context.max_width + (advance + x)] = backbuffers[i][(y + datas[i].coords.y) * datas[i].dimensions.width + (x + datas[i].coords.x)];
		advance += datas[i].dimensions.width;
	}

	for(uint64_t i = 0; i < row->children_count; i++)
		free(backbuffers[i]);

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

	color_t* backbuffer = calloc(context.max_width * context.max_height, sizeof(color_t));

	build_context_t child_context = context;
	child_context.backbuffer = backbuffer;

	draw_data_t child_data = draw_component(align->child, child_context);

	float align_x = (align->alignment.x + 1) / 2;
	float align_y = (align->alignment.y + 1) / 2;

	uint64_t xp = context.max_width * align_x;
	uint64_t yp = context.max_height * align_y;

	uint64_t xc = child_data.dimensions.width * align_x;
	uint64_t yc = child_data.dimensions.height * align_y;

	for (uint64_t y = 0; y < child_data.dimensions.height; y++)
		for (uint64_t x = 0; x < child_data.dimensions.width; x++)
			context.backbuffer[(yp - yc + y) * child_data.dimensions.width + (xp - xc + x)] = backbuffer[y * child_data.dimensions.width + x];

	free(backbuffer);

	data.dimensions = child_data.dimensions;
	data.coords = (coord_t){ (xp - xc), (yp - yc) };

	return data;
}
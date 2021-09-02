#include "rund.h"
#include "wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

dimension_t draw_component(const component_t* component, const build_context_t context);
dimension_t draw_container(const container_t* container, const build_context_t context);
dimension_t draw_row(const row_t* row, const build_context_t context);
dimension_t draw_expanded(const expanded_t* row, const build_context_t context);

void run_app(const rund_app_t* app)
{
	create_window(app);

	do
	{
		build_context_t context;
		context.width = app->width;
		context.height = app->height;
		context.x = 0;
		context.y = 0;
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

dimension_t draw_component(const component_t* component, const build_context_t context)
{
	dimension_t dim = { 0, 0 };

	if(component)
		switch (component->type)
		{
		case EMPTY:
			break;
		case CONTAINER:
			dim = draw_container((const container_t*)component, context);
			break;
		case ROW:
			dim = draw_row((const row_t*)component, context);
			break;
		case EXPANDED:
			dim = draw_expanded((const expanded_t*)component, context);
			break;
		default:
			ERROR("Unrecognized component type: [%d]", 0, (int)component->type);
			break;
		}

	return dim;
}

dimension_t draw_container(const container_t* container, const build_context_t context)
{
	dimension_t dim;
	dim.height = container->height <= abs(context.height - context.y) ? container->height : abs(context.height - context.y);
	dim.width = container->width <= abs(context.width - context.x) ? container->width : abs(context.width - context.x);
	
	for (uint64_t y = 0; y < dim.height; y++)
		for (uint64_t x = 0; x < dim.width; x++)
			context.backbuffer[(context.y + y) * dim.width + (context.x + x)] = container->decoration.color;

	build_context_t child_context = context;
	child_context.width = container->width;
	child_context.height = container->height;
	draw_component(container->child, child_context);

	return dim;
}

dimension_t draw_row(const row_t* row, const build_context_t context)
{
	dimension_t dim = { 0, 0 };

	color_t* backbuffers[row->children_count];
	dimension_t dimensions[row->children_count];

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

		backbuffers[i] = calloc(context.width * context.height, sizeof(color_t));

		build_context_t child_context = context;
		child_context.x = 0;
		child_context.y = 0;
		child_context.backbuffer = backbuffers[i];

		dimensions[i] = draw_component(child, child_context);

		unflexible_size.width += dimensions[i].width;
		unflexible_size.height = dimensions[i].height > unflexible_size.height ? dimensions[i].height : unflexible_size.height;
	}

	dimension_t left_size = { context.width - unflexible_size.width, context.height };

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
		child_context.x = 0;
		child_context.y = 0;
		child_context.width = space.width;
		child_context.height = space.height;
		child_context.backbuffer = backbuffers[i];

		dimensions[i] = draw_component((component_t*)child, child_context);
	}

	for(uint64_t i = 0; i < row->children_count; i++)
	{
		if(backbuffers[i] == 0)
			continue;

		for (uint64_t y = 0; y < dimensions[i].height; y++)
			for (uint64_t x = 0; x < dimensions[i].width; x++)
					context.backbuffer[(context.y + y) * context.width + (context.x + dim.width + x)] = backbuffers[i][y * dimensions[i].width + x];

		if(dimensions[i].height > dim.height)
			dim.height = dimensions[i].height;
		dim.width += dimensions[i].width;
	}

	for(uint64_t i = 0; i < row->children_count; i++)
		if(backbuffers[i] == 0)
			free(backbuffers[i]);

	return dim;
}

dimension_t draw_expanded(const expanded_t* expanded, build_context_t context)
{
	return draw_component(expanded->child, context);
}
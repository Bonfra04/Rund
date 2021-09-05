#include <rund.h>
#include "wrapper.h"
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include <rund/utils/vector.h>

draw_data_t draw_component(const component_t* component, const component_t* parent, const build_context_t context, uint64_t deepness);
draw_data_t draw_container(const container_t* container, const build_context_t context, uint64_t deepness);
draw_data_t draw_row(const row_t* row, const build_context_t context, uint64_t deepness);
draw_data_t draw_expanded(const expanded_t* expanded, const build_context_t context, uint64_t deepness);
draw_data_t draw_align(const align_t* align, const build_context_t context, uint64_t deepness);
draw_data_t draw_center(const center_t* center, const build_context_t context, uint64_t deepness);
draw_data_t draw_constrained_box(const constrained_box_t* constrained_box, const build_context_t context, uint64_t deepness);

typedef struct widget_position
{
	dimension_t dimensions;
	coord_t coords;
	uint64_t z;
	component_t* component;
} widget_position_t;

static vector_t widgets;
static uint64_t deepness;

static void on_mouse_down(uint64_t x, uint64_t y, uint8_t button)
{
	if(!widgets)
		return;

	widget_position_t* target = NULL;
	for(size_t i = 0; i < vector_size(widgets); i++)
	{
		widget_position_t* widget = &((widget_position_t*)widgets)[i];
		if(x >= widget->coords.x && x < widget->coords.x + widget->dimensions.width &&
			y >= widget->coords.y && y < widget->coords.y + widget->dimensions.height)
			if(target == NULL || widget->z > target->z)
				target = widget;
	}

	if(target == NULL)
		return;

	component_t* component = target->component;
	while(component != NULL && component->type != EXPANDED)
		component = component->parent;

	if(component == NULL)
		return;

	TRACE("HIT", 0);
}


void run_app(const rund_app_t* app)
{
	create_window(app, (events_t){ NULL, on_mouse_down });

	widgets = 0;

	do
	{
		if(widgets)
			vector_destroy(widgets);
		deepness = 0;

		build_context_t context = {
			.max_width = app->width, .max_height = app->height,
			.min_width = 0, .min_height = 0,
			.backbuffer = buffer_create(app->width, app->height)
		};

		draw_data_t data = draw_component(app->root, NULL, context, 0);
		widget_position_t root_pos = {
			.dimensions = data.dimensions,
			.coords = data.coords,
			.z = 0,
			.component = app->root
		};

		widgets = data.childs;
		vector_push_back(widgets, root_pos);

		for(uint64_t y = 0; y < app->height; y++)
			for(uint64_t x = 0; x < app->width; x++)
				put_pixel(x, y, context.backbuffer.data[y * app->width + x]);

		buffer_destroy(&context.backbuffer);

		flush();
	} while (event_loop());

	TRACE("Rund app terminated\n", 0);
}

draw_data_t draw_component(const component_t* component, const component_t* parent, const build_context_t context, uint64_t deepness)
{
	draw_data_t data = NULL_DRAW;

	if(component)
	{
		component_t* c = (component_t*)component;
		c->parent = parent;
		switch (component->type)
		{
		case EMPTY:
			break;
		case CONTAINER:
			data = draw_container((const container_t*)component, context, deepness);
			break;
		case ROW:
			data = draw_row((const row_t*)component, context, deepness);
			break;
		case EXPANDED:
			data = draw_expanded((const expanded_t*)component, context, deepness);
			break;
		case ALIGN:
			data = draw_align((const align_t*)component, context, deepness);
			break;
		case CENTER:
			data = draw_center((const center_t*)component, context, deepness);
			break;
		case CONSTRAINED_BOX:
			data = draw_constrained_box((const constrained_box_t*)component, context, deepness);
			break;
		default:
			ERROR("Unrecognized component type: [%d]", 0, (int)component->type);
			break;
		}
	}

	return data;
}

draw_data_t draw_container(const container_t* container, const build_context_t context, uint64_t deepness)
{
	draw_data_t data = NULL_DRAW;
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
	draw_component(container->child, container, child_context, deepness);

	return data;
}

draw_data_t draw_row(const row_t* row, const build_context_t context, uint64_t deepness)
{
	draw_data_t data = { .coords = { 0, 0 }, .dimensions = { context.max_width, context.max_height } };
	data.childs = vector_create(widget_position_t);

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

		datas[i] = draw_component(child, row, child_context, deepness + 1);

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

		datas[i] = draw_component((component_t*)child, row, child_context, deepness + 1);
	}

	uint64_t advance = 0;

	// render childs
	buffer_t* main_buffer = &context.backbuffer;
	for(uint64_t i = 0; i < row->children_count; i++)
	{
		buffer_t* child_buffer = &backbuffers[i];
		uint64_t y_offset = datas[i].coords.y;
		uint64_t x_offset = datas[i].coords.x;

		widget_position_t position;
		position.dimensions = datas[i].dimensions;
		position.coords = (coord_t){ advance, y_offset };
		position.component = row->children[i];
		position.z = deepness + 1;
		vector_push_back(data.childs, position);

		if(datas[i].childs)
		{
			for(size_t c = 0; c < vector_size(datas[i].childs); c++)
			{
				widget_position_t child_position = ((widget_position_t*)datas[i].childs)[c];
				child_position.coords.x += advance - x_offset;
				child_position.coords.y = y_offset;
				vector_push_back(data.childs, child_position);
			}
			vector_destroy(datas[i].childs);
		}

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

draw_data_t draw_expanded(const expanded_t* expanded, build_context_t context, uint64_t deepness)
{
	build_context_t child_context = context;
	child_context.min_height = child_context.max_height;
	child_context.min_width = child_context.max_width;

	draw_data_t data = draw_component(expanded->child, expanded, child_context, deepness + 1);

	if(data.childs == NULL)
		data.childs = vector_create(widget_position_t);

	widget_position_t child_pos = {
		.dimensions = data.dimensions,
		.coords = data.coords,
		.z = deepness + 1,
		.component = expanded->child
	};

	vector_push_back(data.childs, child_pos);

	return data;
}

draw_data_t draw_align(const align_t* align, const build_context_t context, uint64_t deepness)
{
	draw_data_t data = NULL_DRAW;

	buffer_t backbuffer = buffer_create(context.max_width, context.max_height);

	build_context_t child_context = context;
	child_context.backbuffer = backbuffer;

	draw_data_t child_data = draw_component(align->child, align, child_context, deepness + 1);

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
	data.childs = child_data.childs;

	if(data.childs == NULL)
		data.childs = vector_create(widget_position_t);

	widget_position_t child_pos = {
		.dimensions = data.dimensions,
		.coords = data.coords,
		.z = deepness + 1,
		.component = align->child
	};

	vector_push_back(data.childs, child_pos);

	return data;
}

draw_data_t draw_center(const center_t* center, const build_context_t context, uint64_t deepness)
{
	draw_data_t data = draw_component((component_t*)&(center->align), center, context, deepness + 1);

	if(data.childs == NULL)
		data.childs = vector_create(widget_position_t);

	widget_position_t child_pos = {
		.dimensions = data.dimensions,
		.coords = data.coords,
		.z = deepness + 1,
		.component = &(center->align)
	};

	vector_push_back(data.childs, child_pos);

	return data;
}

draw_data_t draw_constrained_box(const constrained_box_t* constrained_box, const build_context_t context, uint64_t deepness)
{
	build_context_t child_context = context;
	child_context.min_height = context.min_height > constrained_box->constraints.min_height ? context.min_height : constrained_box->constraints.min_height;
	child_context.min_width = context.min_width > constrained_box->constraints.min_width ? context.min_width : constrained_box->constraints.min_width;
	child_context.max_height = context.max_height < constrained_box->constraints.max_height ? context.max_height : constrained_box->constraints.max_height;
	child_context.max_width = context.max_width < constrained_box->constraints.max_width ? context.max_width : constrained_box->constraints.max_width;
	
	draw_data_t data = draw_component(constrained_box->child, constrained_box, child_context, deepness + 1);

	if(data.childs == NULL)
		data.childs = vector_create(widget_position_t);

	widget_position_t child_pos = {
		.dimensions = data.dimensions,
		.coords = data.coords,
		.z = deepness + 1,
		.component = constrained_box->child
	};

	vector_push_back(data.childs, child_pos);

	return data;
}
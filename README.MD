# Rund
Rund is a powefull graphics api designed to build fast and responsive application without caring of the developing platforms. It is inspired by [Flutter](https://github.com/flutter/flutter) but it evolved to a pretty different, even if similar, api.<br>
Initially rund was meant to be used in conjunction with [my OS](https://github.com/Bonfra04/BonsOS) (this is still the final goal) but it escalated to something bigger.

### Notes
Rund requires the use of GCC to be compiled since it uses some compiler specific functionalities.<br>
If you'd like to play with the original source code i recommend you to use vscode since the whole build system was ideated with it in mind.<br>
The supported platforms are Windows, Linux, and BonsOS but Linux is considered to be the safest platform to test on while the project is WIP.<br>
**IMPORTANT**: I use wsl under windows to develope so i can't confirm that the vscode setup will work under normal linux.

# Reference
## Introduction
After compiling this project you'll get a file named `rund.a`, it's a static library that needs to be linked with your project. You'll also need the `include` directory for the headers.
<br>
The entry point is handled by the library so you'll need to create a function to be run after the automatic initialization process. The signature of this function is the following.
```c
int rund_main();
```
The return value will be used as return value for the standard c main after some cleanup.
<br>

### Basic usage
Inside rund's main function you are going to need to call `run_app` passing as the only parameter a reference to a rund application. The way to construct an application is as easy as initializing a struct:
```c
int rund_main()
{
    rund_app_t app = {
        "Rund App",
        0, 0, // window spawn coordinates
        500, 500, // window size
        Text(
            .text = L"Hello, world!"
        )
    };

    run_app(&app);
}
```
<br>

## Components tree
Rund works by rendering a series of components starting from a root, parent of all the others.
The last field of the `rund_app` struct is the root component, every child of this component is rendered.

## Fields
Since Rund aims to be dynamic, every component field is allocated trought a custom garbage collector in the haep, this also makes possible having `null` fields. For this reason whenever assigning a field to a static value the `Val` macro must be used among with the appropriate cast, on the other side if the field should be bound to a variable it must be assigned using the `Ref` macro. For example a container will need its dimensions specificied in the following way:
```c
size_t some_value = 200;
//...
Container(
    .width = Val((size_t)100), // static value (casting to the right type)
    .height = Ref(some_value), // dynamic value bound to a variable
)
```

## IDs
Every component has a `id` field that helps differentiate each and every widget and retrive a pointer to them in the future.
```c
void click(component_t* self)
{
    container_t* container = rund_get_component("Container");
    container->color = Val((color_t)0xFF0000FF);
}
//...
Listener(
    .child = Container(
        .id = "Container",
        .width = Val((size_t)100),
        .height = Val((size_t)100),
        .color = Val((color_t)0xFFFF0000)
    ),
    .handlers = Handlers(
        .on_pointer_down = click
    )
)
```

### Components

**Container**<br>
This component is designed to have a single child, with the `color` field you can set a background color for the child (default is transparent). using  the `width` and `height` fields you can specify the maximum dimensions for the child. id the child is smaller the rest of the space is filled using the `color` field. if `width` or `height` are null the size of the child in the missing dimension will be used. If no child is provided a box dimensioned using `width` and `height` is drawed. if neither a child nor a dimension is specified, the minimu imposed by the parents is used (usually 0).
```c
// fields
Container(size_t width, size_t height, color_t color, component_t child)

// example
Container(
    .width = Val((size_t)100),
    .height = Val((size_t)100),
    .color = Val((color_t)0xFFFF0000) // ARGB
)
```

**Align**<br>
This component reposition the child, accordin to the `alignment` field (default to `(0.0, 0.0)`), inside the parent. an alignment of `(0.0, 0.0)` will center the child; `(-1.0, -1.0)` will put it in the top left corner; `(1.0, 1.0)` will put it in the bottom right corner. Any other value will place the widget according to its value.
```c
// fields
Align(alignment_t alignment, component_t child)

// example
Align(
    .alignment = Alignment(0.0, 0.0), // center
    .child = Container(
        .width = Val((size_t)100),
        .height = Val((size_t)100),
        .color = Val((color_t)0xFFFF0000) // ARGB
    )
)
```

## Notes:
* All the compoents not listed in the reference are considered unfinished or unreliable.
* Even if a component is considered finished and is documented it may be bug prone and  incline to modifications.
* Finally this is still a working process to don't expect things to stay the same.
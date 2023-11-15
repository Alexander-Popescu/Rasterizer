project('Renderer', 'cpp')

sdl2_dep = dependency('sdl2', required : true)

# imgui static lib
imgui_src = files('imgui/imgui.cpp', 'imgui/imgui_draw.cpp', 'imgui/imgui_widgets.cpp')
imgui_inc = include_directories('imgui')
imgui_lib = static_library('imgui', imgui_src, include_directories : imgui_inc)

executable('Renderer', 'main.cpp', dependencies : [sdl2_dep], link_with : imgui_lib)
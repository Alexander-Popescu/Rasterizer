project('Renderer', 'cpp')

sdl2_dep = dependency('sdl2', required : true)
gl_dep = dependency('gl', required : true)
glfw_dep = dependency('glfw3', required : true)

sources = files('src/main.cpp', 'src/renderer.h', 'src/renderer.cpp')

executable('Renderer', sources, dependencies : [sdl2_dep, gl_dep, glfw_dep])


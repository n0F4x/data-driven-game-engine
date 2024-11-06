add_library(stb_image_resize2)

target_include_directories(stb_image PUBLIC src)

target_sources(stb_image_resize2 PRIVATE src/stb_image_resize2_impl.c)

add_library(stb_image)

target_include_directories(stb_image PUBLIC src)

target_sources(stb_image PRIVATE src/stb_image_impl.c)

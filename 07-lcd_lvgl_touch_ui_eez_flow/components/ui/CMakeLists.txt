
# Search for src files


file(GLOB_RECURSE UI_SRCS *.c *.cpp)

# Resister components
idf_component_register(
    SRCS
        ${UI_SRCS}
    INCLUDE_DIRS
        "."
    REQUIRES 
        driver
        esp_lvgl_port	
        lvgl
        ) 

# add_compile_definitions(EEZ_FOR_LVGL)


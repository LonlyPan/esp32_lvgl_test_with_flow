# Search for src files
set(LONLYLIB_SOURCES
	led.c
	key.c
	heater.c
	lcd_lvgl.c
	esp_lcd_touch_ns2009.c
	esp_lcd_st7796s.c)
	
# Resister components
idf_component_register(
    SRCS
        ${LONLYLIB_SOURCES}
    INCLUDE_DIRS
        "include"
    REQUIRES
        driver
        main
        ui) 

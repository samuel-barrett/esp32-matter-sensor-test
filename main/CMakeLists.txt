set(PRIV_REQUIRES_LIST device esp_matter esp_matter_console route_hook app_reset)

idf_component_register(SRC_DIRS          "." "bmp280" "i2cdev"
                      PRIV_INCLUDE_DIRS  "." "bmp280" "esp_idf_lib_helpers" "i2cdev"
                      PRIV_REQUIRES      ${PRIV_REQUIRES_LIST})

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 14)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")

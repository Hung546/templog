idf_component_register(SRCS "templogger.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver esp_wifi esp_event nvs_flash esp_netif mdns mqtt oledi2c sht30)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES 
        freertos 
        driver 
        esp_wifi 
        nvs_flash
        esp_netif
        esp_event
        lwip
        json
)

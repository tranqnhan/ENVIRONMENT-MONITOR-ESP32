#define SCD41_SDA     GPIO_NUM_4
#define SCD41_SCL     GPIO_NUM_5
#define SCD41_PORT    I2C_NUM_0

#define SCD41_ADDR    0x62


typedef struct {
    uint16_t co2;
    float temperature;
    float humidity;
} measurement_t;


esp_err_t scd41_init();
esp_err_t scd41_start_periodic_measurement();
esp_err_t scd41_read_measurement(measurement_t* m);
esp_err_t scd41_data_ready(uint32_t *is_ready);
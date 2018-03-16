#include <application.h>
#include <bc_usb_cdc.h>
// LED instance
bc_led_t led;

// Button instance
bc_button_t button;

uint8_t button_flag = 0;
uint8_t debug_col = 1;
uint8_t debug_row = 0;

uint8_t brightness = 128;
uint8_t display_temperature = 0;

void button_event_handler(bc_button_t *self, bc_button_event_t event, void *event_param)
{
    if (event == BC_BUTTON_EVENT_PRESS)
    {
        //bc_led_set_mode(&led, BC_LED_MODE_TOGGLE);
        //button_flag = !button_flag;

        brightness >>= 1;

        if(brightness == 0)
        {
            brightness = 128;
        }
    }

    if(event == BC_BUTTON_EVENT_HOLD)
    {
        display_temperature = !display_temperature;
    }
}

//the colors we will be using
const uint16_t camColors[] = {0x480F,
0x400F,0x400F,0x400F,0x4010,0x3810,0x3810,0x3810,0x3810,0x3010,0x3010,
0x3010,0x2810,0x2810,0x2810,0x2810,0x2010,0x2010,0x2010,0x1810,0x1810,
0x1811,0x1811,0x1011,0x1011,0x1011,0x0811,0x0811,0x0811,0x0011,0x0011,
0x0011,0x0011,0x0011,0x0031,0x0031,0x0051,0x0072,0x0072,0x0092,0x00B2,
0x00B2,0x00D2,0x00F2,0x00F2,0x0112,0x0132,0x0152,0x0152,0x0172,0x0192,
0x0192,0x01B2,0x01D2,0x01F3,0x01F3,0x0213,0x0233,0x0253,0x0253,0x0273,
0x0293,0x02B3,0x02D3,0x02D3,0x02F3,0x0313,0x0333,0x0333,0x0353,0x0373,
0x0394,0x03B4,0x03D4,0x03D4,0x03F4,0x0414,0x0434,0x0454,0x0474,0x0474,
0x0494,0x04B4,0x04D4,0x04F4,0x0514,0x0534,0x0534,0x0554,0x0554,0x0574,
0x0574,0x0573,0x0573,0x0573,0x0572,0x0572,0x0572,0x0571,0x0591,0x0591,
0x0590,0x0590,0x058F,0x058F,0x058F,0x058E,0x05AE,0x05AE,0x05AD,0x05AD,
0x05AD,0x05AC,0x05AC,0x05AB,0x05CB,0x05CB,0x05CA,0x05CA,0x05CA,0x05C9,
0x05C9,0x05C8,0x05E8,0x05E8,0x05E7,0x05E7,0x05E6,0x05E6,0x05E6,0x05E5,
0x05E5,0x0604,0x0604,0x0604,0x0603,0x0603,0x0602,0x0602,0x0601,0x0621,
0x0621,0x0620,0x0620,0x0620,0x0620,0x0E20,0x0E20,0x0E40,0x1640,0x1640,
0x1E40,0x1E40,0x2640,0x2640,0x2E40,0x2E60,0x3660,0x3660,0x3E60,0x3E60,
0x3E60,0x4660,0x4660,0x4E60,0x4E80,0x5680,0x5680,0x5E80,0x5E80,0x6680,
0x6680,0x6E80,0x6EA0,0x76A0,0x76A0,0x7EA0,0x7EA0,0x86A0,0x86A0,0x8EA0,
0x8EC0,0x96C0,0x96C0,0x9EC0,0x9EC0,0xA6C0,0xAEC0,0xAEC0,0xB6E0,0xB6E0,
0xBEE0,0xBEE0,0xC6E0,0xC6E0,0xCEE0,0xCEE0,0xD6E0,0xD700,0xDF00,0xDEE0,
0xDEC0,0xDEA0,0xDE80,0xDE80,0xE660,0xE640,0xE620,0xE600,0xE5E0,0xE5C0,
0xE5A0,0xE580,0xE560,0xE540,0xE520,0xE500,0xE4E0,0xE4C0,0xE4A0,0xE480,
0xE460,0xEC40,0xEC20,0xEC00,0xEBE0,0xEBC0,0xEBA0,0xEB80,0xEB60,0xEB40,
0xEB20,0xEB00,0xEAE0,0xEAC0,0xEAA0,0xEA80,0xEA60,0xEA40,0xF220,0xF200,
0xF1E0,0xF1C0,0xF1A0,0xF180,0xF160,0xF140,0xF100,0xF0E0,0xF0C0,0xF0A0,
0xF080,0xF060,0xF040,0xF020,0xF800,};


bc_led_strip_t led_strip;
static uint32_t _dma_buffer[256 * BC_LED_STRIP_TYPE_RGB * 2]; // count * type * 2

const bc_led_strip_buffer_t _led_strip_buffer =
        {
                .type = BC_LED_STRIP_TYPE_RGB,
                .count = 256,
                .buffer = _dma_buffer
        };

#define PCTL 0x00
#define RST 0x01
#define FPSC 0x02
#define INTC 0x03
#define STAT 0x04
#define SCLR 0x05
#define AVE 0x07
#define INTHL 0x08
#define TTHL 0x0E
#define TTHH 0x0F
#define INT0 0x10
#define T01L 0x80

#define AMG88_ADDR 0x68 // in 7bit

bc_gfx_t gfx;

bool is_ready(void *self)
{
    return bc_led_strip_is_ready((bc_led_strip_t *)self);
}

void clear(void *self)
{
    bc_led_strip_fill((bc_led_strip_t *)self, 0);
}

void draw_pixel(void *self, int left, int top, uint32_t color)
{
    // Transparent background
    if (!color)
    {
        return;
    }
         
    if (top % 2 == 0)
    {
        left = (15 - left);
    }

    int position = left + top * 16;

    bc_led_strip_set_pixel((bc_led_strip_t *)self, position , color << 8);
}

bool update(void *self)
{
    return bc_led_strip_write((bc_led_strip_t *)self);
}

bc_gfx_size_t get_size(void *self)
{
    (void) self;
    static bc_gfx_size_t size = { .width = 16, .height = 16};
    return size;
}

const bc_gfx_driver_t gfx_driver = {
       .is_ready = is_ready,
       .clear = clear,
       .draw_pixel = draw_pixel,
       .get_pixel = NULL,
       .update = update,
       .get_size = get_size
};

void application_init(void)
{
    // Initialize LED
    bc_led_init(&led, BC_GPIO_LED, false, false);
    bc_led_set_mode(&led, BC_LED_MODE_ON);

    // Initialize button
    bc_button_init(&button, BC_GPIO_BUTTON, BC_GPIO_PULL_DOWN, false);
    bc_button_set_event_handler(&button, button_event_handler, NULL);

    bc_i2c_init(BC_I2C_I2C0, BC_I2C_SPEED_100_KHZ);

    bc_system_pll_enable();


    // 10fps
    bc_i2c_memory_write_8b (BC_I2C_I2C0, AMG88_ADDR, FPSC, 0x00);
    // diff interrpt mode, INT output reactive
    bc_i2c_memory_write_8b (BC_I2C_I2C0, AMG88_ADDR, INTC, 0x00);
    // moving average output mode active
    bc_i2c_memory_write_8b (BC_I2C_I2C0, AMG88_ADDR, 0x1F, 0x50);
    bc_i2c_memory_write_8b (BC_I2C_I2C0, AMG88_ADDR, 0x1F, 0x45);
    bc_i2c_memory_write_8b (BC_I2C_I2C0, AMG88_ADDR, 0x1F, 0x57);
    bc_i2c_memory_write_8b (BC_I2C_I2C0, AMG88_ADDR, AVE, 0x20);
    bc_i2c_memory_write_8b (BC_I2C_I2C0, AMG88_ADDR, 0x1F, 0x00);

    //int sensorTemp[2];
    //dataread(AMG88_ADDR,TTHL,sensorTemp,2);

    int8_t temperature[2];
    bc_i2c_memory_read_8b(BC_I2C_I2C0, AMG88_ADDR, TTHL, (uint8_t*)&temperature[0]);
    bc_i2c_memory_read_8b(BC_I2C_I2C0, AMG88_ADDR, TTHH, (uint8_t*)&temperature[1]);

    // 

    volatile float t = (temperature[1]*256 + temperature[0]) * 0.0625;
    t++;

    //bc_log_init(BC_LOG_LEVEL_OFF, BC_LOG_TIMESTAMP_OFF);


    bc_module_power_init();
    bc_led_strip_init(&led_strip, bc_module_power_get_led_strip_driver(), &_led_strip_buffer);  

    bc_gfx_init(&gfx, &led_strip, &gfx_driver);
    bc_gfx_clear(&gfx);
    bc_gfx_font_set(&gfx, &bc_font_ubuntu_11);

    for (int i = 0; i < 256; ++i) {
        bc_led_strip_set_pixel_rgbw(&led_strip, i, 0, 0, 0, 0);
    }

    bc_led_strip_write(&led_strip);

    bc_usb_cdc_init();
}

int32_t map_c(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
  int32_t val = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

    if (val > out_max)
    {
        val = out_max;
    }
    if (val < out_min)
    {
        val = out_min;
    }

  return val;
}

int16_t sensorData[64];
float temperatures[64];

uint8_t usb_str[512];

void application_task()
{

    
        // read each 32 bytes 
        //dataread(AMG88_ADDR, T01L + i*0x20, sensorData, 32);
        bc_i2c_memory_transfer_t transfer;
        transfer.device_address = AMG88_ADDR;
        transfer.memory_address = T01L; // + i*0x20;
        transfer.buffer = sensorData;
        transfer.length = 32*4;
        bc_i2c_memory_read(BC_I2C_I2C0, &transfer);

        float min_temperature = 20;
        float max_temperature = 24;

        for(int l = 0 ; l < 64 ; l++)
        {
            int16_t temporaryData = sensorData[l];
            float temperature;
            if (temporaryData > 0x200)
            {
                temperature = (-temporaryData +  0xfff) * -0.25;
            }
            else
            {
                temperature = temporaryData * 0.25;
            }
            
            if (temperature > max_temperature)
            {
                max_temperature = temperature;
            }

            temperatures[l] = temperature;
        }


        uint8_t row;
        uint8_t col;


        for (col = 0; col < 16; col++)
        {
            for (row = 0; row < 16; row++)
            {
                uint8_t map_index;
                uint32_t index_temp = (row/2) + (col/2) * 8;
                float temperature;
                uint8_t dark = 0;

                if (((col % 2) == 0) && ((row % 2) == 0))
                {
                    temperature = temperatures[index_temp];
                }
                else if (((row % 2) == 1) && ((col % 2) == 0))
                {
                    temperature = (temperatures[index_temp] + temperatures[index_temp + 1]) / 2;
                }
                 else 
                {
                    temperature = (temperatures[row / 2 + (col/2)*8] + temperatures[row / 2 + ((col+1)/2)*8]) / 2;
                }

                map_index = map_c(((uint32_t)temperature), min_temperature, max_temperature, 0, 255);
       
                uint16_t rgb565 = camColors[map_index];

                if (col == 15 || row == 15)
                {
                    dark = 1;
                }

                uint8_t r = (rgb565 >> 8) & 0xF8;
                uint8_t g = (rgb565 >> 3) & 0xFC;
                uint8_t b = (rgb565 << 3) & 0xF8;

                // Lower brightness
                r /= brightness;
                g /= brightness;
                b /= brightness;

                if (dark)
                {
                    r = 0;
                    g = 0;
                    b = 0;
                }

                if (button_flag && debug_col == col && debug_row == row)
                {
                    r = 200;
                    g = 0;
                    b = 0;
                }

                // Actual pixel
                uint8_t col_led = col;
                uint8_t row_led;
                // Liché řádky jsou opačně
                if (col % 2 == 0)
                {
                    row_led = row;
                }
                else
                {
                    row_led = 15 - row;
                }
                uint32_t index_led = row_led + col_led * 16;

                bc_led_strip_set_pixel_rgbw(&led_strip, index_led, r, g, b, 0);
            
            }
        }

    if (display_temperature)
    {
        char buff[8];
        uint32_t color = 128;
        snprintf(buff, sizeof(buff), "%2.0f", temperatures[52]);
        bc_gfx_draw_string(&gfx, 3, -2, buff, color<<16);

        bc_led_strip_set_pixel_rgbw(&led_strip, 12*16+7, 0, 0, color, 0);
    }

    bc_led_strip_write(&led_strip);

    strncpy((char*)usb_str, "[\"a7c8b05762d0/thermo/-/values\", [", sizeof(usb_str));
    uint32_t i;
    char str_buffer[16];
    for (i = 0; i < 64; i++)
    {
        snprintf(str_buffer, sizeof(str_buffer), "%.1f", temperatures[i]);
        
        if(i != 63)
        {
            strncat(str_buffer, ",", sizeof(str_buffer));
        }

        strncat((char*)usb_str, str_buffer, sizeof(str_buffer));
    }
    strncat((char*)usb_str, "]]\n", sizeof(str_buffer));
    //static uint8_t json[] = "[\"a7c8b05762d0/thermo/-/values\", [21.7, 22.9, 22.0,21.7, 22.9, 22.0,21.7, 22.9,21.7, 22.9, 22.0,21.7, 22.9, 22.0,21.7, 22.9,21.7, 22.9, 22.0,21.7, 22.9, 22.0,21.7, 22.9,21.7, 22.9, 22.0,21.7, 22.9, 22.0,21.7, 22.9,21.7, 22.9, 22.0,21.7, 22.9, 22.0,21.7, 22.9,21.7, 22.9, 22.0,21.7, 22.9, 22.0,21.7, 22.9,21.7, 22.9, 22.0,21.7, 22.9, 22.0,21.7, 22.9,21.7, 22.9, 22.0,21.7, 22.9, 22.0,21.7, 22.9]]\n";
    bc_usb_cdc_write(usb_str, strlen((const char*)usb_str));

    bc_scheduler_plan_current_relative(20);
}

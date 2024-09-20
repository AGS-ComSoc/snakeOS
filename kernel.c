#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

uint8_t* framebuffer;
uint32_t fb_pitch;
uint32_t fb_height;
uint32_t fb_width;

typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms1;
    uint32_t syms2;
    uint32_t syms3;
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint32_t vbe_interface_off;
    uint32_t vbe_interface_len;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint8_t color_info[5];
} multiboot_header;

char keymap[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
    '9', '0', '-', '=', '\b', /* Backspace */
    '\t', /* Tab */
    'q', 'w', 'e', 'r', /* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0, /* 29 - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
    '\'', '`', 0, /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', /* 49 */
    'm', ',', '.', '/', 0, /* Right shift */
    '*',
    0, /* Alt */
    ' ', /* Space bar */
    0, /* Caps lock */
    0, /* 59 - F1 key ... > */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, /* < ... F10 */
    0, /* 69 - Num lock*/
    0, /* Scroll Lock */
    0, /* Home key */
    0, /* Up Arrow */
    0, /* Page Up */
    '-',
    0, /* Left Arrow */
    0,
    0, /* Right Arrow */
    '+',
    0, /* 79 - End key*/
    0, /* Down Arrow */
    0, /* Page Down */
    0, /* Insert Key */
    0, /* Delete Key */
    0, 0, 0, 0, /* F11 Key */
    0, /* F12 Key */
    0, /* All other keys are undefined */
};

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}

void set_pixel(uint8_t* framebuffer, uint32_t pitch, uint32_t x, uint32_t y, uint32_t colour)
{
    uint32_t *pixel = (uint32_t *)(framebuffer + y * pitch + x * 4);
    *pixel = colour;
}

void clear()
{
    for(uint32_t y = 0; y < fb_height; y++)
    {
        for(uint32_t x = 0; x < fb_width; x++)
        {
            set_pixel(framebuffer, fb_pitch, x, y, 0x000000);
        }
    }
}

void draw_rectangle(uint8_t* framebuffer, uint32_t framebuffer_width, uint32_t framebuffer_height, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color)
{
    for (uint32_t i = 0; i < height; i++) {
        if (y + i >= framebuffer_height) break;

        for (uint32_t j = 0; j < width; j++) {
            if (x + j >= framebuffer_width) break;

            uint32_t pixel_index = ((y + i) * framebuffer_width + (x + j)) * 4;
            framebuffer[pixel_index + 0] = (color >> 0) & 0xFF;   // Red
            framebuffer[pixel_index + 1] = (color >> 8) & 0xFF;   // Green
            framebuffer[pixel_index + 2] = (color >> 16) & 0xFF;  // Blue
            framebuffer[pixel_index + 3] = (color >> 24) & 0xFF;  // Alpha
        }
    }
}

void wait(unsigned int milliseconds) {
    volatile unsigned int iterations = 1000 * milliseconds;
    for (volatile unsigned int i = 0; i < iterations; i++) {
    }
}

static uint32_t seed = 123456789;  // Default seed

void srand(uint32_t new_seed) {
    seed = new_seed;
}

uint32_t rand() {
    // Constants for LCG (Linear Congruential Generator)
    seed = (seed * 1664525 + 1013904223) & 0xFFFFFFFF; // Linear congruential formula
    return seed;
}

typedef struct {
    int x, y;
} Position;

#define MAX_SNAKE_LENGTH 100

Position snake[MAX_SNAKE_LENGTH];
int snake_length;
Position food;
int direction;  // 0 = up, 1 = right, 2 = down, 3 = left
int game_over;

void spawn_food() {
    food.x = (rand() % (fb_width / 25)) * 25;
    food.y = (rand() % (fb_height / 25)) * 25;
}

void initialize_snake() {
    snake_length = 5;
    for (int i = 0; i < snake_length; i++) {
        snake[i].x = 50 - i * 25;
        snake[i].y = 50;
    }
    direction = 1;
    game_over = 0;
    spawn_food();
}

void draw_snake() {
    for (int i = 0; i < snake_length; i++) {
        draw_rectangle(framebuffer, fb_width, fb_height, snake[i].x, snake[i].y, 25, 25, 0x00FF00);
    }
}

void draw_food() {
    draw_rectangle(framebuffer, fb_width, fb_height, food.x, food.y, 25, 25, 0xFF0000);
}

void update_snake_position() {
    for (int i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    if (direction == 0) snake[0].y -= 25;
    if (direction == 1) snake[0].x += 25;
    if (direction == 2) snake[0].y += 25;
    if (direction == 3) snake[0].x -= 25;

    if (snake[0].x == food.x && snake[0].y == food.y) {
        snake_length++;
        spawn_food();
    }

    if (snake[0].x < 0 || snake[0].x >= fb_width || snake[0].y < 0 || snake[0].y >= fb_height) {
        game_over = 1;
    }

    for (int i = 1; i < snake_length; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            game_over = 1;
        }
    }
}

void kernel_main(uint32_t stackPointer, const multiboot_header* multiboot_structure, uint32_t) {
    framebuffer = (uint8_t*)(uintptr_t)multiboot_structure->framebuffer_addr;
    fb_pitch = multiboot_structure->framebuffer_pitch;
    fb_width = multiboot_structure->framebuffer_width;
    fb_height = multiboot_structure->framebuffer_height;

    clear();
    initialize_snake();

    while (!game_over) {
        clear();
        draw_snake();
        draw_food();
        update_snake_position();

        wait(10000);

        uint8_t scancode = inb(0x60);
        if (!(scancode & 0x80)) {
            if (scancode == 0x48 && direction != 2) direction = 0;  // Up arrow
            if (scancode == 0x4D && direction != 3) direction = 1;  // Right arrow
            if (scancode == 0x50 && direction != 0) direction = 2;  // Down arrow
            if (scancode == 0x4B && direction != 1) direction = 3;  // Left arrow
        }
    }

    // Game over screen
    while (1);
}

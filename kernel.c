#include "libkern.h"


Position snake[MAX_SNAKE_LENGTH];
int snake_length;
Position food;
int direction;  // 0 = up, 1 = right, 2 = down, 3 = left
int game_over;

void spawnFood(framebuffer_t* framebuffer) {
    food.x = (rand() % (framebuffer->width / 25)) * 25;
    food.y = (rand() % (framebuffer->height / 25)) * 25;
}

void initializeSnake(framebuffer_t* framebuffer) {
    snake_length = 5;
    for (int i = 0; i < snake_length; i++) {
        snake[i].x = 50 - i * 25;
        snake[i].y = 50;
    }
    direction = 1;
    game_over = 0;
    spawnFood(framebuffer);
}

void drawSnake(framebuffer_t* framebuffer) {
    for (int i = 0; i < snake_length; i++) {
        drawRect(framebuffer, snake[i].x, snake[i].y, 25, 25, 0x00FF00);
    }
}

void drawFood(framebuffer_t* framebuffer) {
    drawRect(framebuffer, food.x, food.y, 25, 25, 0xFF0000);
}

void updateSnakePosition(framebuffer_t* framebuffer) {
    for (int i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    if (direction == 0) snake[0].y -= 25;
    if (direction == 1) snake[0].x += 25;
    if (direction == 2) snake[0].y += 25;
    if (direction == 3) snake[0].x -= 25;

    if (snake[0].x == food.x && snake[0].y == food.y) {
        snake_length++;
        spawnFood(framebuffer);
    }

    if (snake[0].x < 0 || snake[0].x >= framebuffer->width || snake[0].y < 0 || snake[0].y >= framebuffer->height) {
        game_over = 1;
    }

    for (int i = 1; i < snake_length; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            game_over = 1;
        }
    }
}

void kernel_main(uint32_t stackPointer, const multiboot_t* multiboot_structure) {


    framebuffer_t frontBuffer;

    if(initFB(multiboot_structure, &frontBuffer))
    {
        serial_write("FAIL... Could not enable graphics. HALT.");
        __asm__("hlt");
    }


    serial_write("  ********                    **               *******    ********\n");
    serial_write(" **//////                    /**              **/////**  **////// \n");
    serial_write("/**        *******   ******  /**  **  *****  **     //**/**       \n");
    serial_write("/*********//**///** //////** /** **  **///**/**      /**/*********\n");
    serial_write("////////** /**  /**  ******* /****  /*******/**      /**////////**\n");
    serial_write("       /** /**  /** **////** /**/** /**//// //**     **        /**\n");
    serial_write(" ********  ***  /**//********/**//**//****** //*******   ******** \n");
    serial_write("////////  ///   //  //////// //  //  //////   ///////   ////////  \n");

    serial_write("By Zoe Summers\n");

    initializeSnake(&frontBuffer);

    while (!game_over) {
        clearFB(&frontBuffer, 0x000000);
        drawSnake(&frontBuffer);
        drawFood(&frontBuffer);
        updateSnakePosition(&frontBuffer);

        wait(10000);
        getInput(&direction);
    }

    // Game over screen
    while (1);
}

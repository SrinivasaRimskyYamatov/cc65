#include <nes.h>
#include <neslib.h>
#include <stdint.h>

// OAM buffer for sprite data
unsigned char oam_buffer[256];

// Sprite position
unsigned char sprite_x = 128;  // Center X
unsigned char sprite_y = 120;  // Center Y

// Input state
unsigned char pad_state;

void init_palette(void) {
    // Set background palette (black background)
    pal_bg(0);
    
    // Palette data: background and sprite palettes
    // Sprite palette 0: black, green, light green, white
    unsigned char palette[32] = {
        0x0f, 0x30, 0x27, 0x37,  // Sprite palette 0: black, dark green, green, light green
        0x0f, 0x16, 0x27, 0x37,  // Sprite palette 1: black, gray, green, light green
        0x0f, 0x00, 0x10, 0x20,  // Sprite palette 2: black, black, black, black
        0x0f, 0x00, 0x10, 0x20,  // Sprite palette 3: black, black, black, black
        
        0x0f, 0x30, 0x27, 0x37,  // BG palette 0: black, dark green, green, light green
        0x0f, 0x16, 0x27, 0x37,  // BG palette 1: black, gray, green, light green
        0x0f, 0x00, 0x10, 0x20,  // BG palette 2: black, black, black, black
        0x0f, 0x00, 0x10, 0x20   // BG palette 3: black, black, black, black
    };
    
    pal_all(palette);
}

void init_sprites(void) {
    // Clear OAM buffer
    memset(oam_buffer, 0, 256);
}

void update_sprite(void) {
    // Update sprite 0
    // OAM format: Y, tile, attributes, X
    oam_buffer[0] = sprite_y;      // Y position
    oam_buffer[1] = 0x00;          // Tile number 0
    oam_buffer[2] = 0x01;          // Attributes: palette 0, no flip
    oam_buffer[3] = sprite_x;      // X position
}

void handle_input(void) {
    pad_state = pad_poll(PAD_1);
    
    // D-pad left
    if (pad_state & PAD_LEFT) {
        if (sprite_x > 0) {
            sprite_x -= 2;
        }
    }
    
    // D-pad right
    if (pad_state & PAD_RIGHT) {
        if (sprite_x < 248) {  // 256 - 8 (sprite width)
            sprite_x += 2;
        }
    }
    
    // D-pad up
    if (pad_state & PAD_UP) {
        if (sprite_y > 0) {
            sprite_y -= 2;
        }
    }
    
    // D-pad down
    if (pad_state & PAD_DOWN) {
        if (sprite_y < 232) {  // 240 - 8 (sprite height)
            sprite_y += 2;
        }
    }
}

void main(void) {
    // Disable IRQs
    cli();
    
    // Initialize the PPU
    ppu_off();
    
    // Wait for vblank
    waitvblank();
    
    // Initialize palettes
    init_palette();
    
    // Initialize sprites
    init_sprites();
    
    // Enable PPU rendering
    ppu_on_all();
    
    // Main game loop
    while (1) {
        // Wait for vblank to perform safe updates
        waitvblank();
        
        // Handle input
        handle_input();
        
        // Update sprite positions
        update_sprite();
        
        // Upload OAM data during vblank (safe update)
        oam_dma_copy(oam_buffer);
    }
}
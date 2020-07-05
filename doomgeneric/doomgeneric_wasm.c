#include "doomgeneric.h"
#include "emscripten.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

double start_time;
extern void init_js(uint32_t resX, uint32_t resY, uint32_t *ptr);
extern void draw_js();

void DG_Init(){
	start_time = emscripten_get_now();
//	init_js(DOOMGENERIC_RESX, DOOMGENERIC_RESY, DG_ScreenBuffer);

	EM_ASM_({
			canvas = document.getElementById('canvas');
			canvas.width = $0;
			canvas.height = $1;
		}, DOOMGENERIC_RESX, DOOMGENERIC_RESY);

}

void DG_DrawFrame(){
	for(uint32_t i = 0; i < DOOMGENERIC_RESX * DOOMGENERIC_RESY; i++){
		// This is super weird, but something is off with colors. this
		// works anyway, but unclear why. It is like endianness or
		// something is wrong.  transform 0xAABBGGRR -> 0xAARRGGBB and
		// set alpha to 1.  Even though imageData should be on RGBA
		// order, it seems like RGBA is what it actually needs. DOOM
		// seem to think that alpha=0 == no transparency, and so
		// doesnt draw it, while canvas thinks that alpha=0 == full
		// transparency. Also DOOM says it draws BGRA, but it obviously draws ABGR.

		// 0xAABBGGRR -> 0xAARRGGBB
		// And fill alpha -> |= 0xFF000000
		uint32_t px = DG_ScreenBuffer[i];
		DG_ScreenBuffer[i] =
			(px & 0xFF000000)         |
			((px & 0x00FF0000) >> 16) |
			(px & 0x0000FF00)         |
			((px & 0x000000FF) << 16) | 0xFF000000;
	}
//	draw_js();
	

	EM_ASM_({
			let data = Module.HEAPU8.slice($0, $0 + $1 * $2 * 4);
			let context = Module['canvas'].getContext('2d');
			let imageData = context.getImageData(0, 0, $1, $2);
			imageData.data.set(data);
			context.putImageData(imageData, 0, 0);
		}, DG_ScreenBuffer, DOOMGENERIC_RESX, DOOMGENERIC_RESY);

}

void DG_SleepMs(uint32_t ms){
	emscripten_sleep(ms);
}

uint32_t DG_GetTicksMs() {
	return (uint32_t)(emscripten_get_now() - start_time);
}

int DG_GetKey(int* pressed, unsigned char* key){
	return 0;
}

EM_JS(void, setWindowTitle, (const char* title), {
		window.document.tile = UTF8ToString(title);
});

void DG_SetWindowTitle(const char * title){
	setWindowTitle(title);
}



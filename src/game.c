/*
 * PixelBox
 * Copyright (C) 2022 UtoECat

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <main.h>
#include <box.h>
#include <stdio.h>
#include <stdlib.h>

static int game_change_texture_size(struct box* b);

int  (game_init) (struct box* b, uint16_t w, uint16_t h) {
	if (!b) return -1;
	if (!w) w = 640;
	if (!h) h = 480;
	b->w = w;
	b->h = h;
	b->arr = calloc(w * h + 1, sizeof(struct pixel));
	if (!b->arr) {
		crash("Can't allocate memory!");
		return -1;
	}
	b->texture.id = 0;
	game_change_texture_size(b);
	return 0;
}



void (game_free) (struct box* b) {
	if (!b) return;
	b->w = 0;
	b->h = 0;
	game_change_texture_size(b);
	free(b->arr);
}

// reloads existed world, and restore existed if loading fails
int  (game_reload) (struct box* b, const char* filename) {
	struct box cache = *b;
	int v = game_load(b, filename);
	if (v < 0) *b = cache;
	else game_free(&cache);
	return v;
}

int  (game_load) (struct box* b, const char* filename) {
	if (!b) return -1;
	FILE *f = fopen(filename, "rb");
	if (!f) {
		errorf("Can't open file %s!", filename);
		return -1;
	}
	size_t sz = fread(&b->w, sizeof(uint16_t), 2, f);
	if (sz < 2) {
		errorf("Can't read world width :(");
		fclose(f); return -2;
	}
	debugf("World width = %i, world height = %i", b->w, b->h);
	game_init(b, b->w, b->h);
	size_t size = (size_t)b->w * b->h;
	sz = fread(b->arr, sizeof(struct pixel), size, f);
	if (sz < size) {
		errorf("World is not fully readed! (%li)", sz);
		errorf("But excepted size %li", sizeof(struct pixel) * size);
	}
	fclose(f);
	debugf("World [%i:%i] loaded from %s", b->w, b->h, filename);
	return 0;
}

int  (game_save) (struct box* b, const char* filename) {
	if (!b) return -1;
	FILE *f = fopen(filename, "wb");
	if (!f) {
		errorf("Can't open file %s!", filename);
		return -1;
	}
	size_t sz = fwrite(&b->w, sizeof(uint16_t), 2, f);
	if (sz < 2) {
		errorf("Can't write world width :(");
		fclose(f); return -2;
	}
	size_t size = (size_t)b->w * b->h;
	sz = fwrite(b->arr, sizeof(struct pixel), size, f);
	if (sz < size) {
		errorf("World is not fully writed! (%li)", sz);
		errorf("But excepted size %li", sizeof(struct pixel) * size);
	}
	fclose(f);
	debugf("World [%i:%i] saved to %s", b->w, b->h, filename);
	return 0;
}

void (game_noise) (struct box* b, float k) {
	if (!b) return;
	int kk = 1/k;
	if (kk < 0) kk = -kk;
	if (!kk) kk = 1;
	for (uint16_t y = 0; y < b->h; y++) {
		for (uint16_t x = 0; x < b->w; x++) {
			if (rand() % kk == 0)
			b->arr[x + b->w * y].type = rand() % MAX_PIXELS_TYPE;
			else
			b->arr[x + b->w * y].type = 0;
		}
	}
}

// Internal function!
static int game_change_texture_size(struct box* b) {
	if (!b) return -1;
	// generate world representation texture
	// red color -> type
	// greeen color -> pack
	if (!b->w && !b->h) {
		d_texture_free(&b->texture);
		return 0;
	} else if (!b->texture.id) {
		d_texture_init_ex(&b->texture, GL_TEXTURE_2D, GL_RG, GL_RG);
		d_texture_load(&b->texture, (uint8_t*)b->arr, b->w, b->h);
		debugf("Initialized world texture (%i)", b->texture.id);
	} else { // resize
		d_texture_load(&b->texture, (uint8_t*)b->arr, b->w, b->h);
		debugf("Resized world texture");
	}
	gl_check_error("game_change_texture_size");
	return 0;
}


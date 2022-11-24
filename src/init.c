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
#include <stdio.h>
#include <stdlib.h>

#define COLOR_DEFAULT "\033[0m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_PURPLE  "\033[0;35m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_GREEN   "\033[0;32m"

__attribute__ ((format (printf, 1, 2))) 
void debugf (const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, COLOR_YELLOW "[DEBUG] : ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n" COLOR_DEFAULT);
	va_end(args);
}

__attribute__ ((format (printf, 1, 2)))
void successf (const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, COLOR_GREEN "[DEBUG] : ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n" COLOR_DEFAULT);
	va_end(args);
}

__attribute__ ((format (printf, 1, 2)))
void errorf (const char* fmt, ...)  {
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, COLOR_RED "[ERROR] : ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, COLOR_DEFAULT "\n");
	va_end(args);
}

__attribute__ ((format (printf, 1, 2)))
void warningf (const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, COLOR_PURPLE "[WARNING] : ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, COLOR_DEFAULT "\n");
	va_end(args);
}

#include <execinfo.h> // stacktrace and symbols finding

__attribute__((__noreturn__))
void crash (const char* fmt) {
	fprintf(stderr, COLOR_RED "[FATAL] : %s\n" COLOR_DEFAULT, fmt);
	
	fprintf(stderr, COLOR_RED "[STACKTRACE] : ");
	void* trace[128] = {0};
	int frames = backtrace(trace, 128);
	backtrace_symbols_fd(trace, frames, 2);
	fprintf(stderr, COLOR_DEFAULT);
	abort();
}

#include <init.h>
#include <stdlib.h>

static void libs_free() {
	music_free();
	draw_free();
	window_free();
	pixtyp_free();
}

// init

static int libs_init() {
	pixtyp_init();
	if (window_init() != 0) return 1;
	if (draw_init() != 0) {
		window_free();
		return 2;
	}

	if (music_init() != 0) {
		warningf("Audio is disabled");
	}
	return 0;
}

void app_tick (void) {
	music_tick();
	window_tick();	
}

int app_test_mode = 0;

int main(int argc, char** argv) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--test") == 0) {
			debugf("Test mode enabled!");
			app_test_mode = 1;
		}
	}
	debugf("Hello World!");

	if (libs_init() != 0) {
		errorf("Can't init game libraries properly!");
		return -1;
	}
	if (app_init() != 0) {
		errorf("Can't init game itself properly!");
		return -2;
	}
	app_loop();
	app_free();

	libs_free();
	debugf("Goodbye!");
	return 0;
}

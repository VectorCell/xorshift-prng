#include <iostream>
#include <vector>
#include <atomic>

#include <cstdio>
#include <cstdint>
#include <ctime>
#include <cstring>

#include <unistd.h>
#include <signal.h>

using namespace std;

size_t LINESIZE = 64 / (sizeof(uint32_t) << 1);

atomic<bool> stopped(false);

void sig_handler(int sig) {
	if (sig == SIGINT || sig == SIGTERM) {
		stopped = true;
	}
}

uint32_t xorshift128(vector<uint32_t>& state) {
	uint32_t t = state[3];
	t ^= t << 11;
	t ^= t >> 8;
	state[3] = state[2]; state[2] = state[1]; state[1] = state[0];
	t ^= state[0];
	t ^= state[0] >> 19;	
	state[0] = t;
	return t;
}

void write_random(FILE *outfile, bool hex = false) {
	time_t t = time(NULL);
	vector<uint32_t> state(4);
	vector<uint32_t> buffer(4096 / sizeof(uint32_t));
	memcpy(&state[0], &t, min(sizeof(t), 4 * sizeof(uint32_t)));
	while (!stopped) {
		for (size_t k = 0; k < buffer.size(); ++k) {
			buffer[k] = xorshift128(state);
		}
		if (hex) {
			for (size_t k = 0; k < sizeof(buffer); k += LINESIZE) {
				for (size_t j = k; j < k + LINESIZE; ++j) {
					printf("%08x ", buffer[j]);
				}
				printf("\n");
			}
		} else {
			fwrite(&buffer[0], sizeof(char), sizeof(uint32_t) * buffer.size(), outfile);
		}
	}
}

int main(int argc,char *argv[]) {
	signal(SIGINT,  sig_handler);
	signal(SIGTERM, sig_handler);
	write_random(stdout, isatty(fileno(stdout)));
	return EXIT_SUCCESS;
}

#include "ConfigWatcher.h"
#include "Globals.h"
#include "Command.h"
#import <Foundation/Foundation.h>
#include <sys/stat.h>

namespace {

uint64_t GetConfigFileModTime(const std::string& path) {
	struct stat st;
	if (stat(path.c_str(), &st) != 0)
		return 0;
	return (uint64_t)st.st_mtimespec.tv_sec * 1000000000ull + st.st_mtimespec.tv_nsec;
}

void ReloadOnMain() {
	// Marshalled to the main thread so the trie reload happens on the same
	// thread as the event tap callbacks.
	dispatch_async(dispatch_get_main_queue(), ^{
		bool prev = listening;
		listening = true;
		runCommand("RELOAD");
		listening = prev;
	});
}

void WatchConfigFileProc() {
	std::string path = config_path;
	uint64_t last = GetConfigFileModTime(path);

	auto start = std::chrono::steady_clock::now();
	const auto timeout = std::chrono::minutes(10);

	while (std::chrono::steady_clock::now() - start < timeout) {
		usleep(800 * 1000);
		uint64_t current = GetConfigFileModTime(path);
		if (current == 0) continue;
		if (current == last) continue;

		// Debounce: require two consecutive equal readings before declaring done.
		uint64_t pending = current;
		for (int i = 0; i < 8; i++) {
			usleep(700 * 1000);
			uint64_t after = GetConfigFileModTime(path);
			if (after == 0) continue;
			if (after == pending) {
				ReloadOnMain();
				g_watching_config = false;
				return;
			}
			pending = after;
		}
		last = pending;
	}
	g_watching_config = false;
}

} // anonymous namespace

void StartConfigWatcher() {
	bool expected = false;
	if (g_watching_config.compare_exchange_strong(expected, true))
		std::thread(WatchConfigFileProc).detach();
}

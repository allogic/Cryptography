#include <iostream>
#include <bitset>
#include <limits>
#include <vector>
#include <random>
#include <functional>
#include <cassert>

std::random_device rd{};
std::default_random_engine eng{ rd() };
std::uniform_int_distribution<int> dist{ 0, std::numeric_limits<int>::max() };

auto next = std::bind(dist, eng);

void print(const std::string& msg) {
	assert(msg.size() % 2 == 0 && "Message must be divisible by 2");

	const std::size_t half = msg.size() / 2;
	std::string buf(msg.size(), '\0');

	for (std::size_t i = 0; i < msg.size(); i++) {
		if (msg[i] >= 32) buf[i] = msg[i];
		else buf[i] = '.';
	}

	std::cout << buf << " | ";

	for (std::size_t i = 0; i < half; i++)
		std::cout << std::bitset<8>(buf[i]) << ' ';

	std::cout << "| ";

	for (std::size_t i = 0; i < half; i++)
		std::cout << std::bitset<8>(buf[half + i]) << ' ';

	std::cout << '\n';
}

void encrypt(std::size_t rounds, std::string& msg, std::vector<char>& keys) {
	assert(msg.size() % 2 == 0 && "Message must be divisible by 2");

	const std::size_t half = msg.size() / 2;

	for (std::size_t i = 0; i < rounds; i++) {
		std::string buf(msg.size(), '\0');

		keys[i] = next();

		for (std::size_t j = 0; j < msg.size(); j++) {
			if (j < half) buf[j + half] = msg[j];
			else buf[j - half] = msg[j - half] ^ msg[j] ^ keys[i];
		}

		msg = buf;

		print(msg);
	}
}

void decrypt(std::size_t rounds, std::string& msg, std::vector<char>& keys) {
	assert(msg.size() % 2 == 0 && "Message must be divisible by 2");

	const std::size_t half = msg.size() / 2;

	for (std::size_t i = 0; i < rounds; i++) {
		std::string buf(msg.size(), '\0');

		for (std::size_t j = 0; j < msg.size(); j++) {
			if (j < half) buf[j + half] = msg[j];
			else buf[j - half] = msg[j - half] ^ msg[j] ^ keys[i];
		}

		msg = buf;

		print(msg);
	}
}

void swap(std::string& msg) {
	assert(msg.size() % 2 == 0 && "Message must be divisible by 2");

	const std::size_t half = msg.size() / 2;

	std::string buf(msg.size(), '\0');

	for (std::size_t j = 0; j < msg.size(); j++) {
		if (j < half) buf[j + half] = msg[j];
		else buf[j - half] = msg[j];
	}

	msg = buf;
}

int main() {
	const std::size_t n = 16;

	std::string msg{ "this is some foo" };
	std::vector<char> keys(n);

	print(msg);

	encrypt(n, msg, keys);

	std::cout << '\n';

	std::reverse(keys.begin(), keys.end());

	swap(msg);

	decrypt(n, msg, keys);

	swap(msg);

	print(msg);

	return 0;
}
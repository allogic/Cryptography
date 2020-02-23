#include <iostream>
#include <limits>
#include <vector>
#include <random>
#include <functional>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

std::random_device rd{};
std::default_random_engine eng{ rd() };
std::uniform_int_distribution<int> dist{ 0, std::numeric_limits<int>::max() };

auto nextInt = std::bind(dist, eng);

class CImage final {
public:
	CImage(const std::string& file) {
		int w, h, c;

		dataBuffer = stbi_load(file.c_str(), &w, &h, &c, STBI_rgb);

		assert(width % 2 == 0 && "Image width be divisible by 2");
		assert(height % 2 == 0 && "Image height be divisible by 2");

		width = static_cast<unsigned int>(w);
		height = static_cast<unsigned int>(h);
		channels = static_cast<unsigned int>(c);

		size = width * height * channels;
		half = size / 2;

		swapBuffer = new unsigned char[size];
	}
	~CImage() {
		delete[] swapBuffer;

		stbi_image_free(dataBuffer);
	}

public:
	void Transform(const unsigned int rounds, const std::vector<unsigned int>& keys) {
		for (unsigned int i = 0; i < rounds; i++) {
			for (unsigned int j = 0; j < size; j++) {
				if (j < half) swapBuffer[j + half] = dataBuffer[j];
				else swapBuffer[j - half] = dataBuffer[j - half] ^ dataBuffer[j] ^ keys[i];
			}

			SwapBuffers();
		}
	}

	void Flip() {
		for (unsigned int i = 0; i < size; i++) {
			if (i < half) swapBuffer[i + half] = dataBuffer[i];
			else swapBuffer[i - half] = dataBuffer[i];
		}

		SwapBuffers();
	}

	void Write(const std::string& file) {
		stbi_write_jpg(file.c_str(), width, height, channels, dataBuffer, 100);
	}

private:
	void SwapBuffers() {
		for (unsigned int i = 0; i < size; i++)
			dataBuffer[i] = swapBuffer[i];
	}

private:
	unsigned char* dataBuffer = nullptr;
	unsigned char* swapBuffer = nullptr;

	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int channels = 0;

	unsigned int size = 0;
	unsigned int half = 0;
};

auto GenRandomKeys(const unsigned int rounds) {
	std::vector<unsigned int> keys(rounds);

	for (unsigned int i = 0; i < keys.size(); i++) keys[i] = nextInt();

	return keys;
}

int main() {
	const unsigned int rounds = 512;

	CImage image{ "kalti.jpg" };

	auto keys = GenRandomKeys(rounds);

	image.Transform(rounds, keys);

	image.Write("kalti_encoded.jpg");

	std::reverse(keys.begin(), keys.end());

	image.Flip();
	image.Transform(rounds, keys);
	image.Flip();

	image.Write("kalti_decoded.jpg");

	return 0;
}
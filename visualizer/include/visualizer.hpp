
#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include <functional>

namespace cv {
	class Mat;
}

namespace visualizer {
	void load_cfg(
		const std::string& fn
	);

	namespace img {
		void show(
			const std::string& path,
			const uint8_t* pix,
			uint16_t width,
			uint16_t height
			//TODO Format.
		);
		void show(
			const std::string& path,
			const cv::Mat& m
			//TODO Format.
		);
	}

	namespace plot {
		void clear();
		void plot(
			const std::string& path,
			unsigned idx, //TODO Remove.
			const std::vector<double>& x,
			const std::vector<double>& y,
			const std::string& label
		);
	}

	void flush();

	namespace slider {
		/**
		 * Create slider or if is already created set value.
		 */
		void slider(
			const std::string& path,
			int& var,
			std::function<void(int&)> callback = nullptr
		);
		/**
		 * @return true if any slider changed.
		 */
		bool update();
	}
}


#include "VisualizerCore.hpp"


/*
To have buffers free from drawing for writing in program:
Visualizer class with slots per type of visualization
 for moving obj with data and code in Qt thread.
If volatile busy flag set, do not make signal.
Two funtors: copyData, drawData
Copy data to Qt structures (QImage, QVector<double>...)
and attach them to be pending for draw
On flush() set busy flag true and signal drawing.


class ImagePacket {
	QImage image;
};

class PlotPacket {
	QVector <double> x;
	QVector <double> y;
	function<void(void)> copyData;
	function<void(void)> drawData;
};

void plot(const std::vector<double>& x) {
	PlotPacket p;
	p.copyData = [&](){
		p.x = QwVector<double>::fromStdVector(x);
		...
	}
	signalize p
}

void plot(const std::vector<float>& x) {
	p.copyData = [&](){
		p.x.resize(x.size());
		for(int i = 0; i < x.size(); i++){
			p.x[i] = x[i];
		}
		...
	}
	signalize p
}
*/



/*
struct PlotPacket {
	unsigned idx;
	const std::vector<double>* x;
	const std::vector<double>* y;
	std::string label;

	PlotPacket(){

	}

	PlotPacket(const PlotPacket& o) :
		idx(o.idx),
		x(o.x),
		y(o.y),
		label(o.label)
	{
	}


	PlotPacket& operator=(const PlotPacket& o) {
		idx = o.idx;
		x = o.x;
		y = o.y;
		label = o.label;
		return *this;
	}
};


#include <queue>
#include <mutex>
//TODO Use boost::spsc_queue
std::queue<PlotPacket> forDrawing;
std::mutex forDrawingMux;

//TODO To some other class.
void MainWindow::drawSomething(

) {
	bool have = false;
	PlotPacket p;
	{
		std::lock_guard<std::mutex> guard(forDrawingMux);
		if(!forDrawing.empty()){
			p = forDrawing.front();
			forDrawing.pop();
			have = true;
		}
	}
	if(have){

	}

}
*/

#include "visualizer.hpp"

#include "opencv2/opencv.hpp"
using namespace cv;

struct SliderStruct {
	int* var;
	function<void(int&)> callback;

	SliderStruct() :
		var(nullptr),
		callback(nullptr)
	{}
	SliderStruct(
		int* var,
		function<void(int&)> callback
	) :
		var(var),
		callback(callback)
	{}
};
static map<string, SliderStruct> sliders;

namespace visualizer {
	void load_cfg(
		const std::string& fn
	) {
		lock_guard<mutex> guard(q.mux);
		q.load_cfg.push(fn);
	}

	namespace img {
		void show(
			const std::string& path,
			const uint8_t* pix,
			uint16_t width,
			uint16_t height
			//TODO Format.
		) {
			pkg__img__show p;
			p.path = path;
			p.pix = pix;
			p.width = width;
			p.height = height;
			p.B_per_line = width*3;
			lock_guard<mutex> guard(q.mux);
			q.img__show.push(p);
		}


		void show(
			const std::string& path,
			const cv::Mat& m
			//TODO Format.
		) {
			pkg__img__show p;
			p.path = path;
			p.pix = m.data;
			p.width = m.cols;
			p.height = m.rows;
			p.B_per_line = m.step;
			lock_guard<mutex> guard(q.mux);
			q.img__show.push(p);
		}
	}

	namespace plot {
		void clear() {
			//TODO
		}
		void plot(
			const std::string& path,
			unsigned idx,
			const std::vector<double>& x,
			const std::vector<double>& y,
			const std::string& label
		) {
			pkg__plot__plot p;
			p.path = path;
			p.idx = idx; //TODO remove idx.
			p.x = &x;
			p.y = &y;
			p.label = label;
			lock_guard<mutex> guard(q.mux);
			q.plot__plot.push(p);
		}
	}

	void flush() {
		//TODO
	}

	namespace slider {
		void slider(
			const std::string& path,
			int& var,
			std::function<void(int&)> callback
		) {
			sliders[path] = SliderStruct(&var, callback);

			// Pkg to set value.
			pkg__slider__slider p;
			p.path = path;
			p.value_to_set = var;
			lock_guard<mutex> guard(q.mux);
			q.slider__slider.push(p);
		}

		bool update() {
			bool any_slider_changed = false;
			lock_guard<mutex> guard(s.mux);

			for(auto const& [path, ss] : sliders) {
				if(s.path_to_value.count(path)){
					int old_var = *ss.var;
					*ss.var = s.path_to_value[path];
					if(ss.callback){
						ss.callback(*ss.var);
					}
					if(old_var != *ss.var){
						any_slider_changed = true;
					}
				}
			}

			return any_slider_changed;
		}
	}
}

#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <map>
#include <assert.h>
using namespace std;

#include <QDebug>

//TODO Forward declare YAML::Node
#include <yaml-cpp/yaml.h>

#define DEBUG(x) do{ std::cout << #x << " = " << x << std::endl; }while(0)

struct pkg__img__show {
	string path;
	const uint8_t* pix;
	uint16_t width;
	uint16_t height;
	uint32_t B_per_line;
};

struct pkg__plot__plot {
	string path;
	unsigned idx;
	const std::vector<double>* x;
	const std::vector<double>* y;
	std::string label;
};

struct pkg__slider__slider {
	string path;
	int value_to_set;
};

struct pkg_queues {
	mutex mux;
	queue<string> load_cfg;
	queue<pkg__img__show> img__show;
	queue<pkg__plot__plot> plot__plot;
	// If user changed var from prev, it should change GUI slider.
	queue<pkg__slider__slider> slider__slider;
};
extern pkg_queues q;

struct sliders_stuff {
	mutex mux;
	map<string, int> path_to_value;
};
extern sliders_stuff s;

// TODO create template class PathMap po uzoru na imageContainer here. 
template <class T> 
class PathMap {
public:
	PathMap();

	void add(
		const std::string&,
		const T&
	);

	std::vector<std::pair<std::string, T>> search(
		const std::string&
	);

	static std::string ambiguity(
		const std::vector<std::pair<std::string, T>>&
	);



private:
	std::vector<std::pair<std::string, T>> cont;

	bool endsWith(
		const std::string&,
		const std::string&
	);

};

class QBoxLayout;
class QCustomPlot;
class ImageViewerWithSelecter;
class Slider;

class VisualizerCore : public QObject {
	Q_OBJECT
public:
	VisualizerCore();

public slots:
	void processRequests();

private:
	std::thread* t;

	vector<QWidget*> q_wins;
	// TODO use PathMap class
	// map<string, ImageViewerWithSelecter*> path_to_img_viewer;
	// map<string, QCustomPlot*> path_to_plot;
	// map<string, Slider*> path_to_slider;
	PathMap<QCustomPlot*> path_to_plot;
	PathMap<Slider*> path_to_slider;
	PathMap<ImageViewerWithSelecter*> path_to_img_viewer;

	void recursive_tree_build(
		const YAML::Node& y,
		const string& parent_path,
		QWidget* w_parent,
		QBoxLayout* l_parent
	);

	void load_cfg(
		const std::string& fn
	);

	void plot__plot(
		const std::string& path,
		unsigned idx,
		const std::vector<double>& x,
		const std::vector<double>& y,
		const std::string& label
	);

};

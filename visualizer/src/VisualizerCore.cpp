#include "VisualizerCore.hpp"

#include <QApplication>
#include <QTimer>
#include <QBoxLayout>
#include <QTabWidget>

#include <yaml-cpp/parser.h>
#include <queue>
//TODO What is this?
#define YAML_CPP_STATIC_DEFINE


#include "qcustomplot.h"
#include "ImageViewerWithSelecter.hpp"
#include "Slider.hpp"

//for debug
#include <unistd.h>


class Starter {
	std::thread* t;
public:
	Starter() {
		t = new std::thread([&](){
			int argc = 0;
			char argv0[] = "visualizer";
			char* argv[] = {argv0, nullptr};
			QApplication a(argc, argv);
			VisualizerCore vc;

			QTimer* timer = new QTimer();
			vc.connect(timer, &QTimer::timeout, &vc, &VisualizerCore::processRequests);
			timer->start(0);

			a.exec();
		});
	}
};
static Starter _starter;

pkg_queues q;
sliders_stuff s;

VisualizerCore::VisualizerCore() {
	//imageContainer = ImageContainer();
}


template <class T> PathMap<T>::PathMap() {

}

template <class T> void PathMap<T>::add(
	const std::string& pPath,
	const T& val
) {
	std::pair<std::string, T> element(pPath, val);
	this->cont.push_back(element);
}

template <class T> bool PathMap<T>::endsWith(
	const std::string& s1,
	const std::string& s2
) {
	return (s1.find(s2, s1.size() - s2.size()) + s2.size() == s1.size());
}

template <class T> std::vector<std::pair<std::string, T>> PathMap<T>::search(
	const std::string& pPath
) {
	std::vector<std::pair<std::string, T>> retvec;
	for (auto const& x: this->cont) {
		string fullPath = x.first;
		if (this->endsWith(fullPath, pPath)) {
			retvec.push_back(x);
		}
	}
	return retvec;
}

template <class T> std::string PathMap<T>::ambiguity(
	const std::vector<std::pair<std::string, T>>& vect
) {
	std::string ret(" ");
	switch(vect.size()){
		case 0:
			ret.append("No match found for");
			ret.append(vect.front().first);
			break;
		case 1:
			ret.append("Match found");  
			ret.append(vect.front().first);
			break;
		default:
			ret.append("Ambiguity\n");
			for(auto el : vect){
				ret.append("\t");
				ret.append(el.first);
				ret.append(" \n");
			}
			break;
	}
	return ret;
}



void VisualizerCore::processRequests() {
	lock_guard<mutex> guard(q.mux);
	while(!q.load_cfg.empty()){
		string fn = q.load_cfg.front();
		q.load_cfg.pop();
		load_cfg(fn);
	}
	while(!q.img__show.empty()){
		auto p = q.img__show.front();
		q.img__show.pop();

		// std::queue<string> satisf;
		// // TODO use ImageContainer instead
		// for(auto const& x: path_to_img_viewer){
		// 	string full_path = x.first;
		// 	if(endsWith(full_path, p.path)){
		// 		// TODO use ImageContainer instead
		// 		auto img_viewer = path_to_img_viewer.at(full_path);
		// 		// TODO QImage::Format_BGR888 in Qt v5.14
		// 		QImage img(p.pix, p.width, p.height, p.B_per_line, QImage::Format_RGB888);
		// 		QImage img2 = img.rgbSwapped();
		// 		img_viewer->setImage(img2);
		// 		satisf.push(full_path);
		// 	}
		// }

		auto vect = path_to_img_viewer.search(p.path);
		auto img_viewer = vect.front().second;
		QImage img(p.pix, p.width, p.height, p.B_per_line, QImage::Format_RGB888);
		QImage img2 = img.rgbSwapped();
		img_viewer->setImage(img2);
		// ambiguity check
		/*switch(vect.size()) {
			case 0:
				std::cout << "no match in " << vect.front().first << std::endl;
				break;
			case 1:
				std::cout << "match" << std::endl;
				break;
			default:
				std::cout << "ambiguity:" << std::endl;
				for (auto const& x: vect) {
					std::cout << "\t" << x.first << std::endl;
				}
				break;
		}*/
		std::cout << path_to_img_viewer.ambiguity(vect) << std::endl;


	}
	while(!q.plot__plot.empty()){
		auto p = q.plot__plot.front();
		q.plot__plot.pop();
		plot__plot(
			p.path,
			p.idx,
			*p.x,
			*p.y,
			p.label
		);
	}

	while(!q.slider__slider.empty()){
		auto p = q.slider__slider.front();
		q.slider__slider.pop();

		// if(path_to_slider.count(p.path)){
		// 	auto slider = path_to_slider.at(p.path);
		// 	slider->setValue(p.value_to_set);
		// }
		auto vec = path_to_slider.search(p.path);
		if (vec.size()) {
			auto slider = vec.front().second;
			slider->setValue(p.value_to_set);
		}
		std::cout << path_to_slider.ambiguity(vec) << std::endl;
	}
}


//TODO Add to class.

//TODO static map<string, string> path_shortucts;


void VisualizerCore::recursive_tree_build(
	const YAML::Node& y,
	const string& parent_path,
	QWidget* w_parent,
	QBoxLayout* l_parent
) {
	auto type = y["type"].as<string>();
	auto name = y["name"].as<string>();
	string path = parent_path + "/" + name;
    // std::cout << "type = " << type << std::endl;
	// DEBUG(type);
    // DEBUG(path);
	/*if(type == "vert"){
		auto layout = new QVBoxLayout();

		if(l_parent){
			l_parent->addLayout(layout);
		}else if(w_parent){
			w_parent->setLayout(layout);
		}

		for(auto cy : y["children"]){
			recursive_tree_build(cy, path, w_parent, layout);
		}
	}else if(type == "horiz"){

		auto layout = new QHBoxLayout();

		if(l_parent){
			l_parent->addLayout(layout);
		}else if(w_parent){
			w_parent->setLayout(layout);
		}

		for(auto cy : y["children"]){
			recursive_tree_build(cy, path, w_parent, layout);
		}
	}*/
    if(type == "layout"){
        string orientation_str = (y["orientation"].as<string>());    
        if(orientation_str == "vert"){
            auto layout = new QVBoxLayout();

            if(l_parent){
                l_parent->addLayout(layout);
            }else if(w_parent){
                w_parent->setLayout(layout);
            }

            for(auto cy : y["children"]){
                recursive_tree_build(cy, path, w_parent, layout);
            }
        }else if(orientation_str == "horiz"){
            auto layout = new QHBoxLayout();

            if(l_parent){
                l_parent->addLayout(layout);
            }else if(w_parent){
                w_parent->setLayout(layout);
            }

            for(auto cy : y["children"]){
                recursive_tree_build(cy, path, w_parent, layout);
            }
        }
    }
    else if(type == "tab"){
		QTabWidget* tab = new QTabWidget(w_parent);

		if(l_parent){
			l_parent->addWidget(tab);
		}else if(w_parent){
			//TODO Insert some layout in between.
			cout << "To implement: " << type << endl;
		}

		for(auto cy : y["children"]){
			auto name = QString::fromStdString(cy["name"].as<string>());
			QWidget* temp = new QWidget();
			temp->setAccessibleName(name);
			tab->addTab(temp, name);
			recursive_tree_build(cy, path, temp, nullptr);
		}
	}else if(type == "image"){
		auto img_viewer = new ImageViewerWithSelecter();
		img_viewer->setText(QString::fromStdString(name));
		//todo ovde
		//prolazis kroz path_to_img_viewer[path] kljuceve i trazis da li se poklapa sa
		
		// TODO use ImageContainer instead
		
		//imageContainer.add(path, img_viewer);
		//path_to_img_viewer[path] = img_viewer;
		path_to_img_viewer.add(path, img_viewer);

        // if(y["src"]) { 
        //     string src = y["src"].as<string>();

        //     char buff[1024]; //create string buffer to hold path
        //     getcwd(buff, FILENAME_MAX);
        //     string cu_wo_dir(buff);

        //     std::cout << "path= " <<  cu_wo_dir + "/" + src << std::endl;
            
        //     QString qPath = QString::fromStdString(cu_wo_dir + "/" + src);
        //     QImage *img = new QImage(qPath);
        //     img_viewer->setImage(*img);
        // }else{
        //     std::cout << "no src pic included" << std::endl;
        // }

		if(l_parent){
			l_parent->addWidget(img_viewer);
		}else if(w_parent){
			//TODO Insert some layout in between.
			cout << "To implement: " << type << endl;
		}
	}else if(type == "slider"){
		int var_max = y["max"].as<int>();
		auto orientation_str = QString::fromStdString(y["orientation"].as<string>());
		Qt::Orientation orientation;
		if(orientation_str == "horiz") {
			orientation = Qt::Horizontal;
		}else if (orientation_str == "vert") {
			orientation = Qt::Vertical;
		}else{
			cout << "To implement error: " << type << endl;
		}
		int var_init = 0;
		{
			lock_guard<mutex> guard(s.mux);
			if(!s.path_to_value.count(path)){
				s.path_to_value[path] = 0;
			}
		}
		auto* ps = &s;
		auto slider = new Slider(
			QString::fromStdString(name),
			var_max,
			[=](int value){
				lock_guard<mutex> guard(ps->mux);
				ps->path_to_value[path] = value;
			},
			orientation,
			w_parent
		);
		//path_to_slider[path] = slider;
		path_to_slider.add(path, slider);

		if(l_parent){
			l_parent->addWidget(slider);
		}else if(w_parent){
			//TODO Insert some layout in between.
			cout << "To implement: " << type << endl;
		}

		//auto vect = path_to_slider.search(path);
		//auto st = path_to_slider.ambiguity(vect);

	}else if(type == "plot"){
		auto plot = new QCustomPlot();
		//TODO plot->setText(QString::fromStdString(name));
		DEBUG(plot);
		//path_to_plot[path] = plot;
		path_to_plot.add(path, plot);

		if(l_parent){
			l_parent->addWidget(plot);
		}else if(w_parent){
			//TODO Insert some layout in between.
			cout << "To implement: " << type << endl;
		}

		plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

		plot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
		plot->legend->setVisible(true);
		/*
		TODO
		QFont legendFont = font();  // start out with MainWindow's font..
		legendFont.setPointSize(9); // and make a bit smaller for legend
		plot->legend->setFont(legendFont);
		*/
		plot->legend->setBrush(QBrush(QColor(255,255,255,230)));
		// by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
		plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

		int line_width = 3;
		// Hardcoded.
		plot->addGraph();
		plot->graph(0)->setPen(QPen(Qt::red, line_width));
		plot->graph(0)->setLineStyle(QCPGraph::lsLine);

		plot->addGraph();
		plot->graph(1)->setPen(QPen(Qt::green, line_width));
		plot->graph(1)->setLineStyle(QCPGraph::lsLine);

		plot->addGraph();
		plot->graph(2)->setPen(QPen(Qt::blue, line_width));
		plot->graph(2)->setLineStyle(QCPGraph::lsLine);

		plot->addGraph();
		plot->graph(3)->setPen(QPen(Qt::magenta, line_width));
		plot->graph(3)->setLineStyle(QCPGraph::lsLine);

		plot->xAxis->setRange(0, 100);
		plot->yAxis->setRange(0, 1000);

		plot->replot();
	}else{
		//TODO Others
		cout << "To implement: " << type << endl;
	}
}

void VisualizerCore::load_cfg(
	const std::string& fn
) {

	YAML::Node config = YAML::LoadFile(fn);

	//TODO What if new load is called. Kill all windows.
	if(config["windows"]){
		auto y_wins = config["windows"];
		assert(y_wins.IsSequence());
		for(const YAML::Node& y_win : y_wins){
			auto q_win = new QWidget();
			q_wins.push_back(
				q_win
			);
			q_win->setAccessibleName(
				QString::fromStdString(
					y_win["name"].as<string>()
				)
			);
			recursive_tree_build(y_win, "", q_win, nullptr);
		}
	}

	cout << "Parsing done" << endl; // TODO Nicer.


	for(auto q_win: q_wins){
		q_win->show();
	}
}


void VisualizerCore::plot__plot(
	const std::string& path,
	unsigned idx,
	const std::vector<double>& x,
	const std::vector<double>& y,
	const std::string& label
) {
	// if(path_to_plot.count(path)){
	// 	auto plot = path_to_plot.at(path);

	// 	plot->graph(idx)->setData(
	// 		QVector<double>::fromStdVector(x),
	// 		QVector<double>::fromStdVector(y)
	// 	);
	// 	plot->graph(idx)->setName(
	// 		QString::fromStdString(label)
	// 	);
	// 	plot->replot();
	// }
	auto vect = path_to_plot.search(path);
	
	auto st = path_to_plot.ambiguity(vect); //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	std::cout << path_to_plot.ambiguity(vect) << std::endl;
	if (vect.size()) {
		auto plot = vect.front().second;

		plot->graph(idx)->setData(
			QVector<double>::fromStdVector(x),
			QVector<double>::fromStdVector(y)
		);
 //0:
	// 		std::cout << "no match in " << vect.front().first << std::endl;
	// 		break;
	// 	case 1:
	// 		std::cout << "match" << std::endl;
	// 		break;
	// 	default:
	// 		std::cout << "ambiguity:" << std::endl;
	// 		for (auto const& x: vect) {
	// 			std::cout << "\t" << x.first << std::endl;
	// 		}
	// 		break;
	// 
	}
}

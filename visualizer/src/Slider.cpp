
#include "Slider.hpp"

#include <QSlider>
#include <QLabel>
#include <QBoxLayout>

Slider::Slider(
	const QString& name,
	int var_max,
	std::function<void(int)> updateVar,
	Qt::Orientation orientation,
	QWidget* parent
) :
	QWidget(parent), updateVar(updateVar)
{
	slider = new QSlider(orientation, this);
	slider->setValue(0);
	slider->setRange(0, var_max);
	QBoxLayout* layout = nullptr;
	if(orientation == Qt::Horizontal) {
		layout = new QHBoxLayout();
	}else{
		layout = new QVBoxLayout();
	}
	layout->addWidget(new QLabel(name));
	layout->addWidget(new QLabel(QString::number(0)));
	layout->addWidget(slider);
	layout->addWidget(new QLabel(QString::number(var_max)));
	setLayout(layout);

	connect(slider, &QSlider::valueChanged, this, &Slider::valueChanged);
}

void Slider::setValue(int value) {
	slider->setValue(value);
}

void Slider::valueChanged(int value) {
	updateVar(value);
}

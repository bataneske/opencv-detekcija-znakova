

#pragma once

#include <QWidget>

#include <functional>

class QSlider;

class Slider : public QWidget {
	Q_OBJECT
public:
	Slider(
		const QString& name,
		int var_max,
		std::function<void(int)> updateVar,
		Qt::Orientation orientation,
		QWidget* parent = nullptr
	);
	~Slider() override = default;
	void setValue(int value);
private:
	std::function<void(int)> updateVar;
	QSlider* slider;
private slots:
	void valueChanged(int value);
};

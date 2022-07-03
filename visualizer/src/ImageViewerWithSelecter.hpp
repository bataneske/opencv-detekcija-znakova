
#pragma once

#include "image-viewer.h"

class ImageViewerWithSelecter : public pal::ImageViewer {
	Q_OBJECT
public:
	ImageViewerWithSelecter(QWidget *parent = nullptr);
	~ImageViewerWithSelecter() override = default;
};

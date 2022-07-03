#include "ImageViewerWithSelecter.hpp"
#include "rect-selection.h"

#include <QGraphicsView>

ImageViewerWithSelecter::ImageViewerWithSelecter(
	QWidget *parent
) :
	ImageViewer(parent)
{
	setToolBarMode(pal::ImageViewer::ToolBarMode::AutoHidden);
	view()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

/*
	//TODO
	// selection tool
	auto selecter = new pal::SelectionItem(pixmapItem());
	selecter->setVisible(false);

	auto sel = new QToolButton(this);
	sel->setToolTip(tr("Selects a rectangle area in the image"));
	sel->setIcon(QIcon(":select"));
	sel->setCheckable(true);

	// cropping only allowed when an image is visible
	auto updater = [=] {
		const bool ok = !this->image().isNull();
		if (ok)
			selecter->resetSelection();
		else
			sel->setChecked(false);
		sel->setEnabled(ok);
	};

	connect(sel, &QToolButton::toggled, selecter, &pal::SelectionItem::setVisible);
	connect(this, &pal::ImageViewer::imageChanged, selecter, updater);
	updater();
	addTool(sel);
*/
	// Fit image in window on double click.
//		connect(this, &pal::PixmapItem::doubleClicked, this, &pal::ImageViewer::zoomFit);
}


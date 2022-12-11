/**
MIT License
Copyright (c) 2022 Analyzable
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "imageviewer.h"
#include "./ui_imageviewer.h"

ImageViewer::ImageViewer(QWidget *parent)
    : QGraphicsView(parent), ui(new Ui::ImageViewer), scene(new QGraphicsScene(this)), image(new QGraphicsPixmapItem()), brushPixmap(QStringLiteral(":/assets/cursor.png")), painter(new QPainter()), currentZoom(1), factor(1), isDrawable(true), undoAction(new QAction(this)) {
  ui->setupUi(this);
  image->setAcceptDrops(true);
  scene->addItem(image);
  this->setScene(scene);
  this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  this->setBrush();
  this->undoAction->setShortcut(QKeySequence(QKeySequence::Undo));
  connect(undoAction, &QAction::triggered, this, &ImageViewer::undo);
  this->addAction(this->undoAction);
}

void ImageViewer::setBrush(QColor color, double size) {
  this->brushColor = color;
  this->brushSize = size;
  drawingCursor = new QCursor(brushPixmap.scaledToHeight(int(this->brushSize * this->currentZoom)));
}

void ImageViewer::setImage(const QString &path) {
  this->clear();
  QPixmap pixmap(path);
  this->image->setPixmap(pixmap);
}

void ImageViewer::clear() {
  this->undoStack.clear();
  this->image->setPixmap(QPixmap());
}

void ImageViewer::wheelEvent(QWheelEvent *event) {
  double factor = 0;
  if (event->angleDelta().y() > 0) {
    factor = 1.25;
    this->currentZoom *= factor;
    this->setBrush(this->brushColor, this->brushSize);
  }
  else {
    factor = 0.8;
    this->currentZoom *= factor;
    this->setBrush(this->brushColor, this->brushSize);
  }
  this->scale(factor, factor);
  event->accept();
}

void ImageViewer::mousePressEvent(QMouseEvent *event) {
  if (event->buttons() == Qt::MiddleButton) {  // Get pan coordinates reference with middle click
    QApplication::setOverrideCursor(Qt::ClosedHandCursor);
    this->panReferenceClick = event->pos();
  }
  else if (event->buttons() == Qt::LeftButton and event->modifiers() == Qt::ControlModifier) {  // Get coordinates reference for brush size
    this->brushReference = event->pos();
  }
  if (event->buttons() == Qt::LeftButton) {  // Get drawing coordinates reference with left click
    QApplication::setOverrideCursor(*this->drawingCursor);
    this->drawReference = this->mapToScene(event->pos());
  }
}

void ImageViewer::mouseReleaseEvent(QMouseEvent *event) {
  QApplication::restoreOverrideCursor();
}

void ImageViewer::mouseMoveEvent(QMouseEvent *event) {
  if (event->buttons() == Qt::MiddleButton) {  // pan with middle click pressed
    this->horizontalScrollBar()->setValue(this->horizontalScrollBar()->value() + (this->panReferenceClick.x() - event->pos().x()));
    this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() + (this->panReferenceClick.y() - event->pos().y()));
    this->panReferenceClick = event->pos();
  }
  else if (event->buttons() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier) {
    if (event->pos().x() - this->brushReference.x() > 25) {
      this->brushSize += 5;
      this->setBrush(this->brushColor, this->brushSize);
      this->cursor().setPos(this->mapToGlobal(this->brushReference));
      QApplication::restoreOverrideCursor();
      QApplication::setOverrideCursor(*this->drawingCursor);
    }
    else if (event->pos().x() - this->brushReference.x() < -25) {
      this->brushSize -= 5;
      this->setBrush(this->brushColor, qMax(this->brushSize, 5.));
      this->cursor().setPos(this->mapToGlobal(this->brushReference));
      QApplication::restoreOverrideCursor();
      QApplication::setOverrideCursor(*this->drawingCursor);
    }
  }

  if (event->buttons() == Qt::LeftButton && this->isDrawable && event->modifiers() == Qt::NoModifier) {  // Draw with left click pressed
    this->addToUndoStack();
    QPixmap pixmap = this->image->pixmap();
    this->painter->begin(&pixmap);
    this->painter->setPen(QPen(this->brushColor, this->brushSize, Qt::SolidLine, Qt::RoundCap));
    this->painter->drawLine(this->drawReference, this->mapToScene(event->pos()));
    this->drawReference = this->mapToScene(event->pos());
    this->painter->end();
    this->image->setPixmap(pixmap);
  }
}

void ImageViewer::dropEvent(QDropEvent *event) {
  const QMimeData *mimeData = event->mimeData();
  if (mimeData->hasUrls()) {
    this->setImage(mimeData->urls().at(0).toLocalFile());
  }
}

void ImageViewer::dragEnterEvent(QDragEnterEvent *event) {
  event->acceptProposedAction();
}

void ImageViewer::addToUndoStack() {
  this->undoStack.push(this->image->pixmap().copy());
}

void ImageViewer::undo() {
  if (!this->undoStack.isEmpty()) {
    this->image->setPixmap(this->undoStack.pop());
  }
}

ImageViewer::~ImageViewer() {
  delete ui;
}


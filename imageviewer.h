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

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QColor>
#include <QCursor>
#include <QDropEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QScrollBar>
#include <QWheelEvent>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class ImageViewer;
}
QT_END_NAMESPACE

class ImageViewer : public QGraphicsView {
  Q_OBJECT

 public:
  ImageViewer(QWidget *parent = nullptr);
  ~ImageViewer();
  void setBrush(QColor color = Qt::white, double size = 25);
  void setImage(const QString &path);
  void clear();

 private:
  QGraphicsScene *scene;
  QGraphicsPixmapItem *image;
  QPixmap *pixmap;
  QPixmap *brushPixmap;
  QPainter *painter;
  QColor brushColor;
  double brushSize;
  double factor;
  double currentZoom;
  QCursor *drawingCursor;
  QPoint panReferenceClick;
  QPoint brushReference;
  QPointF drawReference;
  bool isDrawable;
  void wheelEvent(QWheelEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void dropEvent(QDropEvent *event) override;
  void dragEnterEvent(QDragEnterEvent *event) override;
  Ui::ImageViewer *ui;
};
#endif  // IMAGEVIEWER_H

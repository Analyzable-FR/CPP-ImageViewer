#include <QApplication>
#include "imageviewer.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  ImageViewer w;
  w.setImage(":/assets/test.jpg");
  w.show();
  return a.exec();
}

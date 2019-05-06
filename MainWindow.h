#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__ (1)

#include <qvbox.h>

class QMenuBar;
class QSplitter;
class QAxCtl;
class IToolbarControl;

class MainWindow: public QVBox {

  Q_OBJECT
   
 public:
  MainWindow(QWidget* parent = 0);
  ~MainWindow();
  void init(void);
 private slots:
  void open();
  void getTopologyReport(void);
  void doNothing(void) {};
  void testFunc(void);
  void clearLayers(void);
 private:
  void add_toolbar_items(IToolbarControl* pToolbar);
 private:
  QMenuBar* menuBar;
  QSplitter* splitter;
  QAxCtl* tlb;
  QAxCtl* toc;
  QAxCtl* map;
};

#endif //__MAINWINDOW_H__

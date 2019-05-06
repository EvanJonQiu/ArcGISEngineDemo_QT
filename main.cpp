#include <stdio.h>
#include <iostream>
#include <qapplication.h>
#include <qvbox.h>
#include <qsplitter.h>
#include <qtextcodec.h>

//#include <ArcSDK.h>
#include <AxCtl/qt3axctl.h>
#include <Ao/AoControls.h>
#include "LicenseUtilities.h"
#include "MainWindow.h"

void add_toolbar_items(IToolbarControl* pToolbar);

int main(int argc, char** argv) {

  QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

  // Initialize the engine
  if (!InitializeApp()) {
    AoExit(0);
  }

  QApplication qapp(argc, argv);
  MainWindow mainWindow(NULL);
  mainWindow.resize(800, 600);
  mainWindow.init();
  qapp.setMainWidget(&mainWindow);
  mainWindow.show();

  qapp.exec();

  // Uninitialize the engine
  ShutdownApp();

  AoExit(0);
  return 0;
}

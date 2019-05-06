#ifndef __CREATEGDBDLG_H__
#define __CREATEGDBDLG_H__ (1)

#include <qdialog.h>
#include <ArcSDK.h>

class QLabel;
class QLineEdit;
class QPushButton;
class QGridLayout;
class QHBoxLayout;
class QComboBox;

class CreateGDBDlg : public QDialog {
  Q_OBJECT

 public:
  CreateGDBDlg(QWidget *parent = 0);
  ~CreateGDBDlg();
 protected:
  HRESULT createGDB(const char* fullpath);
  HRESULT createFeatureSet(const char* fullpath, const char* featureSetName,
                           const char* destFeatureClassName, const char* srcFullpath);
  HRESULT createTopology(const char* fullpath, const char* featureSetName,
                         const char* featureClassName, const char* topologyName);
  HRESULT validateTopology(ITopologyPtr ipTopology);
  HRESULT checkTopology(const char* fullpath, const char* featureSetName, const char* topologyName);
 private slots:
  void onFindButton(void);
  void onCreateGDBButton(void);
  void onFindSrcBtn(void);
 private:
  QGridLayout *mainLayout;
  QHBoxLayout *topLeftLayout;
  QLabel *label;
  QLineEdit *lineEdit;
  QPushButton *dirButton;

  QLabel *srcLabel;
  QLineEdit *srcEdit;
  QPushButton *srcBtn;
  QHBoxLayout *srcLayout;

  QLabel *gdbNameLabel;
  QLineEdit *gdbNameEdit;
  QHBoxLayout * gdbNameLayout;
  QLabel *featureSetLabel;
  QLineEdit *featureSetEdit;
  QHBoxLayout *featureSetLayout;
  QPushButton *createGDBButton;
  QLabel *ruleLabel;
  QComboBox *ruleComboBox;
};

#endif //__CREATEGDBDLG_H__

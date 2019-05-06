#ifndef __TOPOLOGYREPORTDLG_H__
#define __TOPOLOGYREPORTDLG_H__ (1)

#include <qdialog.h>
#include <ArcSDK.h>

class QLabel;
class QLineEdit;
class QPushButton;
class QListView;
class QAxCtl;

class TopologyReportDlg: public QDialog {
  Q_OBJECT

 public:
  TopologyReportDlg(QWidget *parent, QAxCtl* pMap);
  ~TopologyReportDlg();

 private slots:
  void onFindGDBBtn(void);
  void onShowReportBtn(void);

 private:
  HRESULT showTopologyReport(const char* fullpath);
  QString getRuleNameByType(esriTopologyRuleType type);

 private:
  QLabel *gdbLabel;
  QLineEdit *gdbEdit;
  QPushButton *gdbButton;
  QPushButton* showReportButton;
  QListView* reportListView;
  QAxCtl* map;
};

#endif //__TOPOLOGYREPORTDLG_H__

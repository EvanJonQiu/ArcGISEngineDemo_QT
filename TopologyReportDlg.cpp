#include "TopologyReportDlg.h"
#include "PathUtilities.h"

#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qfiledialog.h>
#include <qlistview.h>
#include <qfont.h>
#include <qmessagebox.h>
#include <iostream>
#include <AxCtl/qt3axctl.h>

TopologyReportDlg::TopologyReportDlg(QWidget* parent, QAxCtl* pMap)
  :QDialog(parent, 0, TRUE) {
  this->setCaption("查看拓扑结果");

  this->map = pMap;

  QGridLayout* mainLayout = new QGridLayout(this, 3, 1);
  mainLayout->setMargin(10);

  gdbLabel = new QLabel("文件数据库", this);
  gdbEdit = new QLineEdit(this, "");
  gdbLabel->setBuddy(gdbEdit);
  gdbButton = new QPushButton("查找", this);
  gdbButton->setFont(QFont("Times", 10, QFont::Bold)); 

  QHBoxLayout* gdbLayout = new QHBoxLayout();
  gdbLayout->setSpacing(10);
  gdbLayout->addWidget(gdbLabel);
  gdbLayout->addWidget(gdbEdit);
  gdbLayout->addWidget(gdbButton);
  connect(gdbButton, SIGNAL(clicked()), this, SLOT(onFindGDBBtn()));

  showReportButton = new QPushButton("查看结果", this);
  connect(showReportButton, SIGNAL(clicked()), this, SLOT(onShowReportBtn()));

  mainLayout->addLayout(gdbLayout, 0, 0);
  mainLayout->addWidget(showReportButton, 1, 0);

  QVBoxLayout* reportLayout = new QVBoxLayout();
  reportListView = new QListView(this);
  reportLayout->addWidget(reportListView);
  mainLayout->addLayout(reportLayout, 2, 0);

  reportListView->addColumn("拓扑规则");
  reportListView->addColumn("错误");
  reportListView->addColumn("异常");
  reportListView->setRootIsDecorated(FALSE);

  this->resize(400, 300);
}

TopologyReportDlg::~TopologyReportDlg() {
}

void TopologyReportDlg::onFindGDBBtn(void) {
  QFileDialog* dlg = new QFileDialog("", QString::null, 0, 0, TRUE);
  dlg->setCaption(QFileDialog::tr("Open"));
  dlg->setMode(QFileDialog::DirectoryOnly);
  QString result;
  if (dlg->exec() == QDialog::Accepted) {
    result = dlg->selectedFile();
    if (!result.isNull()) {
      if (result.endsWith("/")) {
        result.remove(result.length() - 1, 1);
      }
      if (!result.endsWith(".gdb")) {
        QMessageBox::critical(this, "Error", "Please input the name of GDB!",
                              QMessageBox::Ok, QMessageBox::Cancel);
      } else {
        this->gdbEdit->setText(result);
      }
    }
  }
  delete dlg;
}

void TopologyReportDlg::onShowReportBtn(void) {
  QString gdbName = this->gdbEdit->text();
  std::cerr << gdbName << std::endl;
  if (!gdbName.endsWith(".gdb")) {
    QMessageBox::critical(this, "Error", "GDB name is incorrect!",
                          QMessageBox::Ok, QMessageBox::Cancel);
    return;
  }
  const char* fullpath = gdbName.latin1();
  this->showTopologyReport(fullpath);
}

HRESULT TopologyReportDlg::showTopologyReport(const char* fullpath) {
  HRESULT hr = S_OK;

  IMapControl3Ptr ipMap;
  this->map->getInterface((IUnknown**)&ipMap);
  
  // clear all data from list view
  this->reportListView->clear();

  CComBSTR path;
  CComBSTR name;
  CComBSTR bstrFullPath;
  hr = GetParentDirFromFullPath(fullpath, &path);
  if (FAILED(hr) || path.Length() <= 0) {
    std::cerr << "Couldn't get path" << std::endl;
    return E_FAIL;
  } else {
    std::wcout << L"path: " << (BSTR)path << std::endl;
  }
  hr = GetFileFromFullPath(fullpath, &name);
  if (FAILED(hr) || name.Length() <= 0) {
    std::cerr << "Couldn't get name" << std::endl;
    return E_FAIL;
  } else {
    std::wcout << L"name: " << (BSTR)name << std::endl;
  }

  bstrFullPath = path;
  bstrFullPath.Append("/");
  bstrFullPath.Append(name);
  std::wcout << L"fullpath: " << (BSTR)bstrFullPath << std::endl;

  IWorkspaceFactoryPtr ipWorkspaceFactory(CLSID_FileGDBWorkspaceFactory);
  VARIANT_BOOL chk;
  hr = ipWorkspaceFactory->IsWorkspace(bstrFullPath, &chk);
  if (FAILED(hr)) {
    std::cerr << "Couldn't check workspace" << std::endl;
    return E_FAIL;
  } else {
    if (chk == VARIANT_FALSE) {
      std::cerr << "Invalid workspace" << std::endl;
      return E_FAIL;
    }
  }

  IWorkspacePtr ipWorkspace;
  hr = ipWorkspaceFactory->OpenFromFile(bstrFullPath, 0, &ipWorkspace);
  if (FAILED(hr) || ipWorkspace == 0) {
    std::cerr << "Failed to open the dest folder." << std::endl;
    return E_FAIL;
  } else {
    std::cerr << "open the dest folder success" << std::endl;
  }

  IEnumDatasetNamePtr ipEnumDatasetName;
  hr = ipWorkspace->get_DatasetNames(esriDTFeatureDataset, &ipEnumDatasetName);
  if (FAILED(hr) || ipEnumDatasetName == 0) {
    std::cerr << "Get dataset names failed" << std::endl;
  } else {
    std::cerr << "Get dataset names success" << std::endl;
  }

  ILayerFactoryHelperPtr ipLayerFactoryHelper(CLSID_LayerFactoryHelper);

  IDatasetNamePtr ipDatasetName;
  ipEnumDatasetName->Next(&ipDatasetName);
  while (ipDatasetName != 0) {
    CComBSTR name;
    ipDatasetName->get_Name(&name);
    std::wcerr << L"dataset name: " << (BSTR)name << std::endl;

    IFeatureDatasetName2Ptr ipFeatureDatasetName2(ipDatasetName);
    IEnumDatasetNamePtr ipFeatureClassNames;
    hr = ipFeatureDatasetName2->get_FeatureClassNames(&ipFeatureClassNames);
    if (FAILED(hr)) {
      std::wcerr << L"get feature class names failed" << std::endl;
    } else {
      std::wcerr << L"get feature class names success" << std::endl;
    }
    IDatasetNamePtr ipFeatureName;
    ipFeatureClassNames->Next(&ipFeatureName);
    while (ipFeatureName != 0) {
      ipFeatureName->get_Name(&name);
      std::wcerr << L"feature Name: " << (BSTR)name << std::endl;
      IEnumLayerPtr ipEnumLayer;
      hr = ipLayerFactoryHelper->CreateLayersFromName((INamePtr)ipFeatureName, &ipEnumLayer);
      if (FAILED(hr)) {
        std::cerr << "create layer failed" << std::endl;
      } else {
        std::cerr << "create layer success" << std::endl;
        ipEnumLayer->Reset();
        ILayerPtr ipLayer;
        ipEnumLayer->Next(&ipLayer);
        while (ipLayer != 0) {
          ipMap->AddLayer(ipLayer, 0);
          ipEnumLayer->Next(&ipLayer);
        }
      }
      ipFeatureClassNames->Next(&ipFeatureName);
    }

    IEnumDatasetNamePtr ipTopologyNames;
    ipFeatureDatasetName2->get_TopologyNames(&ipTopologyNames);
    IDatasetNamePtr ipTopologyName;
    ipTopologyNames->Next(&ipTopologyName);
    while (ipTopologyName != 0) {
      ipTopologyName->get_Name(&name);
      std::wcerr << L"topology name: " << (BSTR)name << std::endl;

      IEnumLayerPtr ipEnumLayer;
      hr = ipLayerFactoryHelper->CreateLayersFromName((INamePtr)ipTopologyName, &ipEnumLayer);
      if (FAILED(hr)) {
        std::cerr << "create topology layer failed" << std::endl;
      } else {
        std::cerr << "create topology layer success" << std::endl;
        ipEnumLayer->Reset();
        ILayerPtr ipLayer;
        ipEnumLayer->Next(&ipLayer);
        while (ipLayer != 0) {
          ipMap->AddLayer(ipLayer, 0);
          ipEnumLayer->Next(&ipLayer);
        }
      }
      
      ITopologyWorkspacePtr ipTopologyWorkspace(ipWorkspace);
      ITopologyPtr ipTopology;
      hr = ipTopologyWorkspace->OpenTopology(name, &ipTopology);
      if (FAILED(hr) || ipTopology == 0) {
        std::cerr << "open topology failed" << std::endl;
      } else {
        std::cerr << "open topology success" << std::endl;
      }
  
      ITopologyRuleContainerPtr ipTopoRuleContainer(ipTopology);
      IEnumRulePtr ipEnumRule;
      hr = ipTopoRuleContainer->get_Rules(&ipEnumRule);
      if (FAILED(hr) || ipEnumRule == 0) {
        std::cerr << "get topology rules failed" << std::endl;
      } else {
        std::cerr << "get topology rules success" << std::endl;
      }

      IGeoDatasetPtr ipGeoDataset(ipTopology);
      ISpatialReferencePtr ipSpatialReference;
      ipGeoDataset->get_SpatialReference(&ipSpatialReference);

      IEnvelopePtr ipEnvelope;
      ipGeoDataset->get_Extent(&ipEnvelope);

      ipEnumRule->Reset();
      IRulePtr ipRule;
      ipEnumRule->Next(&ipRule);
      while (ipRule != 0) {
        // show topology rule name
        ITopologyRulePtr ipTopologyRule(ipRule);
        esriTopologyRuleType ruleType;
        CComBSTR name;
        ipTopologyRule->get_TopologyRuleType(&ruleType);
        ipTopologyRule->get_Name(&name);
        std::wcerr << L"rule type: " << ruleType << std::endl;
        std::wcerr << L"rule name: " << (BSTR)name << std::endl;

        IErrorFeatureContainerPtr ipErrorFeatureContainer(ipTopology);
        IEnumTopologyErrorFeaturePtr ipEnumTopologyErrorFeature;
        hr = ipErrorFeatureContainer->get_ErrorFeatures(ipSpatialReference,
                                                        (ITopologyRulePtr)ipRule,
                                                        ipEnvelope,
                                                        TRUE,
                                                        TRUE,
                                                        &ipEnumTopologyErrorFeature);
        if (FAILED(hr)) {
          std::cerr << "Get topology error feature failed" << std::endl; 
        } else {
          std::cerr << "Get topology error feature success" << std::endl; 
        }

        int errorCount = 0;
        int exceptionCount = 0;

        ITopologyErrorFeaturePtr ipTopologyErrorFeature;
        ipEnumTopologyErrorFeature->Next(&ipTopologyErrorFeature);
        while (ipTopologyErrorFeature != 0) {
          long oid;
          ipTopologyErrorFeature->get_OriginOID(&oid);
          std::cerr << "Origin feature OID: " << oid << std::endl;

          VARIANT_BOOL isException;
          ipTopologyErrorFeature->get_IsException(&isException);
          if (isException == VARIANT_TRUE) {
            ++exceptionCount;
          } else {
            ++errorCount;
          }

          ipEnumTopologyErrorFeature->Next(&ipTopologyErrorFeature);
        }
        std::cerr << "error: " << errorCount << ", exception: " << exceptionCount << std::endl;
        new QListViewItem(reportListView, this->getRuleNameByType(ruleType), QString::number(errorCount), QString::number(exceptionCount));
        ipEnumRule->Next(&ipRule);
      }

      ipTopologyNames->Next(&ipTopologyName);
    }

    ipEnumDatasetName->Next(&ipDatasetName);
  }

  return hr;
}

QString TopologyReportDlg::getRuleNameByType(esriTopologyRuleType type) {
  switch (type) {
  case esriTRTAny:
    return QString("任何类型");
  case esriTRTFeatureLargerThanClusterTolerance:
    return QString("Feature larege than cluster tolerance");
  case esriTRTAreaNoGaps:
    return QString("Area no gaps");
  case esriTRTLineNoOverlap:
    return QString("不能重叠");
  default:
    return QString("不支持的拓扑规则");
  }
}

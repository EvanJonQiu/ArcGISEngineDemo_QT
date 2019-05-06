#include "MainWindow.h"
#include "PathUtilities.h"
#include "CreateGDBDlg.h"
#include "TopologyReportDlg.h"

#include <iostream>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qsplitter.h>

#include <ArcSDK.h>
#include <AxCtl/qt3axctl.h>
#include <Ao/AoControls.h>

MainWindow::MainWindow(QWidget* parent)
  :QVBox(parent) {
}

MainWindow::~MainWindow() {
  //delete this->splitter;
  //delete this->tlb;
  //delete this->toc;
  //delete this->map;
}

void MainWindow::init(void) {

  QPopupMenu* fileMenu = new QPopupMenu(this);
  fileMenu->insertItem("打开", this, SLOT(open()));
  fileMenu->insertItem("查看拓扑分析结果", this, SLOT(getTopologyReport()));
  //fileMenu->insertItem("test", this, SLOT(testFunc()));
  fileMenu->insertItem("清除所有图层", this, SLOT(clearLayers()));

  QPopupMenu* editMenu = new QPopupMenu(this);
  editMenu->insertItem("撤消", this, SLOT(doNothing()));
  editMenu->insertItem("恢复", this, SLOT(doNothing()));
  editMenu->insertSeparator();
  editMenu->insertItem("剪切", this, SLOT(doNothing()));
  editMenu->insertItem("复制", this, SLOT(doNothing()));
  editMenu->insertItem("粘贴", this, SLOT(doNothing()));
  editMenu->insertItem("选择粘贴", this, SLOT(doNothing()));

  QPopupMenu* dataMenu = new QPopupMenu(this);
  dataMenu->insertItem("导入", this, SLOT(doNothing()));

  QPopupMenu* wndMenu = new QPopupMenu(this);
  wndMenu->insertItem("概览", this, SLOT(doNothing()));

  QPopupMenu* helpMenu = new QPopupMenu(this);
  helpMenu->insertItem("关于", this, SLOT(doNothing()));
  
  this->menuBar = new QMenuBar(this);
  this->menuBar->insertItem("文件", fileMenu);
  this->menuBar->insertItem("编辑", editMenu);
  this->menuBar->insertItem("数据", dataMenu);
  this->menuBar->insertItem("窗口", wndMenu);
  this->menuBar->insertItem("帮助", helpMenu);

  this->tlb = new QAxCtl(AoPROGID_ToolbarControl, this, "Toolbar Control");
  this->tlb->setMinimumHeight(30);
  this->tlb->setMaximumHeight(30);

  this->splitter = new QSplitter(this);

  this->toc = new QAxCtl(AoPROGID_TOCControl, this->splitter, "TOC Control");
  this->map = new QAxCtl(AoPROGID_MapControl, this->splitter, "map Control");

  IToolbarControlPtr ipToolbar;
  IMapControl3Ptr ipMap;
  ITOCControlPtr ipToc;
  HRESULT hr;
 
  hr = this->tlb->getInterface((IUnknown**)&ipToolbar);
  hr = this->toc->getInterface((IUnknown**)&ipToc);
  hr = this->map->getInterface((IUnknown**)&ipMap);

  if (ipToolbar != 0) {
    ipToolbar->SetBuddyControl(ipMap);
  }
  if (ipToc != 0) {
    ipToc->SetBuddyControl(ipMap);
  }

  add_toolbar_items(ipToolbar);
}

void MainWindow::add_toolbar_items(IToolbarControl* pToolbar) {
  CComVariant varTool;
  long itemindex;
  if (!pToolbar) {
    return;
  }

  varTool = L"esriControlCommands.ControlsAddDataCommand";
  pToolbar->AddItem(varTool, 0, -1, VARIANT_FALSE, 0, esriCommandStyleIconOnly, &itemindex);

  varTool = L"esriControlCommands.ControlsOpenDocCommand";
  pToolbar->AddItem(varTool, 0, -1, VARIANT_FALSE, 0, 
                    esriCommandStyleIconOnly, &itemindex);
  
  varTool = L"esriControlCommands.ControlsMapZoomInTool";
  pToolbar->AddItem(varTool, 0, -1, VARIANT_TRUE, 0,
                    esriCommandStyleIconOnly, &itemindex);
  varTool = L"esriControlCommands.ControlsMapZoomOutTool";
  pToolbar->AddItem(varTool, 0, -1, VARIANT_FALSE, 0,
                    esriCommandStyleIconOnly, &itemindex);
  varTool = L"esriControlCommands.ControlsMapZoomInFixedCommand";
  pToolbar->AddItem(varTool, 0, -1, VARIANT_FALSE, 0,
                    esriCommandStyleIconOnly, &itemindex);
  varTool = L"esriControlCommands.ControlsMapZoomOutFixedCommand";
  pToolbar->AddItem(varTool, 0, -1, VARIANT_FALSE, 0,
                    esriCommandStyleIconOnly, &itemindex);
  varTool = L"esriControlCommands.ControlsMapPanTool"; 
  pToolbar->AddItem(varTool, 0, -1, VARIANT_FALSE, 0,
                    esriCommandStyleIconOnly, &itemindex);
  varTool = L"esriControlCommands.ControlsMapFullExtentCommand";
  pToolbar->AddItem(varTool, 0, -1, VARIANT_FALSE, 0,
                    esriCommandStyleIconOnly, &itemindex);
  varTool = L"esriControlCommands.ControlsMapZoomToLastExtentBackCommand";
  pToolbar->AddItem(varTool, 0, -1, VARIANT_FALSE, 0,
                    esriCommandStyleIconOnly, &itemindex);
  varTool = L"esriControlCommands.ControlsMapZoomToLastExtentForwardCommand";
  pToolbar->AddItem(varTool, 0, -1, VARIANT_FALSE, 0,
                    esriCommandStyleIconOnly, &itemindex);
  varTool = L"esriControlCommands.ControlsSelectFeaturesTool";
  pToolbar->AddItem(varTool, 0, -1, VARIANT_FALSE, 0,
                    esriCommandStyleIconOnly, &itemindex);
  varTool = L"esriControlCommands.ControlsSelectTool";
  pToolbar->AddItem(varTool, 0, -1, VARIANT_FALSE, 0,
                    esriCommandStyleIconOnly, &itemindex);
  varTool = L"esriControlCommands.ControlsFeatureSelectionToolbar";
  pToolbar->AddItem(varTool, 0, -1, VARIANT_FALSE, 0,
                    esriCommandStyleIconOnly, &itemindex);
  varTool = L"esriControlCommands.ControlsSelectByGraphicsCommand";
  pToolbar->AddItem(varTool, 0, -1, VARIANT_FALSE, 0,
                    esriCommandStyleIconOnly, &itemindex);
}

void MainWindow::open() {
  CreateGDBDlg *dlg = new CreateGDBDlg();
  if (dlg->exec() == QDialog::Accepted) {
  }
  delete dlg;
}

void MainWindow::getTopologyReport(void) {
  TopologyReportDlg* dlg = new TopologyReportDlg(0, this->map);
  if (dlg->exec() == QDialog::Accepted) {
    
  }
  delete dlg;
}

void MainWindow::testFunc(void) {
  IMapControl3Ptr ipMap;
  this->map->getInterface((IUnknown**)&ipMap);


  HRESULT hr = S_OK;
  const char * fullpath = "/home/admin/Documents/test/report.gdb";
  CComBSTR path;
  CComBSTR name;
  CComBSTR bstrFullPath;
  hr = GetParentDirFromFullPath(fullpath, &path);
  if (FAILED(hr) || path.Length() <= 0) {
    std::cerr << "Couldn't get path" << std::endl;
    return;
  } else {
    std::wcout << L"path: " << (BSTR)path << std::endl;
  }
  hr = GetFileFromFullPath(fullpath, &name);
  if (FAILED(hr) || name.Length() <= 0) {
    std::cerr << "Couldn't get name" << std::endl;
    return;
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
    return;
  } else {
    if (chk == VARIANT_FALSE) {
      std::cerr << "Invalid workspace" << std::endl;
      return;
    }
  }

  IWorkspacePtr ipWorkspace;
  hr = ipWorkspaceFactory->OpenFromFile(bstrFullPath, 0, &ipWorkspace);
  if (FAILED(hr) || ipWorkspace == 0) {
    std::cerr << "Failed to open the dest folder." << std::endl;
    return;
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

    // get feature class that included in a feature set
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
      ipFeatureClassNames->Next(&ipFeatureName);
    }

    ipFeatureClassNames->Reset();
    ipFeatureClassNames->Next(&ipFeatureName);
    while (ipFeatureName != 0) {
      IEnumLayerPtr ipEnumLayer;
      hr = ipLayerFactoryHelper->CreateLayersFromName((INamePtr)ipFeatureName, &ipEnumLayer);
      if (FAILED(hr)) {
        std::cerr << "create layer failed" << std::endl;
      } else {
        std::cerr << "create layer success" << std::endl;
      }

      ipEnumLayer->Reset();
      ILayerPtr ipLayer;
      ipEnumLayer->Next(&ipLayer);
      while (ipLayer != 0) {
        ipMap->AddLayer(ipLayer, 0);

        ipEnumLayer->Next(&ipLayer);
      }
      
      ipFeatureClassNames->Next(&ipFeatureName);
    }
    
    IEnumDatasetNamePtr ipTopologyNames;
    ipFeatureDatasetName2->get_TopologyNames(&ipTopologyNames);
    IDatasetNamePtr ipTopologyName;
    ipTopologyNames->Next(&ipTopologyName);
    while (ipTopologyName != 0) {
      IEnumLayerPtr ipEnumLayer;
      hr = ipLayerFactoryHelper->CreateLayersFromName((INamePtr)ipTopologyName, &ipEnumLayer);
      if (FAILED(hr)) {
        std::cerr << "create topology layer failed" << std::endl;
      } else {
        std::cerr << "create topology layer success" << std::endl;
      }

      ipEnumLayer->Reset();
      ILayerPtr ipLayer;
      ipEnumLayer->Next(&ipLayer);
      while (ipLayer != 0) {
        ipMap->AddLayer(ipLayer, 0);
        ipEnumLayer->Next(&ipLayer);
      }
      
      ipTopologyNames->Next(&ipTopologyName);
    }

    ipTopologyNames->Reset();
    ipTopologyNames->Next(&ipTopologyName);
    while (ipTopologyName != 0) {
      ipTopologyName->get_Name(&name);
      std::wcerr << L"topology name: " << (BSTR)name << std::endl;
      ipTopologyNames->Next(&ipTopologyName);
    }

    ipEnumDatasetName->Next(&ipDatasetName);
  }

  IFeatureWorkspacePtr ipFeatureWorkspace(ipWorkspace);
  IFeatureDatasetPtr ipFeatureDataset;
  hr = ipFeatureWorkspace->OpenFeatureDataset(CComBSTR("test"), &ipFeatureDataset);
  if (SUCCEEDED(hr) && ipFeatureDataset != 0) {
    std::cerr << "Feature set exists" << std::endl;
  } else {
    std::cerr << "Feature set does not exists" << std::endl;
    return;
  }

  ITopologyWorkspacePtr ipTopologyWorkspace(ipWorkspace);
  ITopologyPtr ipTopology;
  hr = ipTopologyWorkspace->OpenTopology(CComBSTR("topology"), &ipTopology);
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

  // enum topology rules type
  ipEnumRule->Reset();
  IRulePtr ipRule;
  ipEnumRule->Next(&ipRule);
  while (ipRule != 0) {
    esriRuleType type;
    ipRule->get_Type(&type);
    std::cerr << "topology type: " << type << std::endl;
    ipEnumRule->Next(&ipRule);
  }

  IGeoDatasetPtr ipGeoDataset(ipTopology);
  ISpatialReferencePtr ipSpatialReference;
  ipGeoDataset->get_SpatialReference(&ipSpatialReference);

  IEnvelopePtr ipEnvelope;
  ipGeoDataset->get_Extent(&ipEnvelope);

  ipEnumRule->Reset();
  ipEnumRule->Next(&ipRule);
  while (ipRule != 0) {
    // show topology rule name
    ITopologyRulePtr ipTopologyRule(ipRule);
    CComBSTR name;
    ipTopologyRule->get_Name(&name);
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
    ITopologyErrorFeaturePtr ipTopologyErrorFeature;
    ipEnumTopologyErrorFeature->Next(&ipTopologyErrorFeature);
    while (ipTopologyErrorFeature != 0) {
      long oid;
      ipTopologyErrorFeature->get_OriginOID(&oid);
      std::cerr << "Origin feature OID: " << oid << std::endl;
      ipEnumTopologyErrorFeature->Next(&ipTopologyErrorFeature);
    }
    ipEnumRule->Next(&ipRule);
  }
}

void MainWindow::clearLayers(void) {
  ITOCControlPtr ipToc;
  IMapControl3Ptr ipMap;
  HRESULT hr = S_OK;
 
  hr = this->map->getInterface((IUnknown**)&ipMap);
  ipMap->ClearLayers();

  hr = this->toc->getInterface((IUnknown**)&ipToc);
  ipToc->Update();
}

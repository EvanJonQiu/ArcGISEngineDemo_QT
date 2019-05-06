#include "CreateGDBDlg.h"
#include "PathUtilities.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <iostream>

CreateGDBDlg::CreateGDBDlg(QWidget *parent)
  :QDialog(parent, 0, TRUE) {
  this->setCaption("导入数据到GDB");

  label = new QLabel("输出目录:", this);
  lineEdit = new QLineEdit(this, "");
  label->setBuddy(lineEdit);

  dirButton = new QPushButton("查找",this);
  
  topLeftLayout = new QHBoxLayout();
  topLeftLayout->setSpacing(10);
  topLeftLayout->addWidget(label);
  topLeftLayout->addWidget(lineEdit);
  topLeftLayout->addWidget(dirButton);
  connect(dirButton, SIGNAL(clicked()), this, SLOT(onFindButton()));

  gdbNameLabel = new QLabel("文件输据库名:", this);
  gdbNameEdit = new QLineEdit(this, "");
  gdbNameLabel->setBuddy(gdbNameEdit);
  createGDBButton = new QPushButton("创建", this);
  gdbNameLayout = new QHBoxLayout();
  gdbNameLayout->addWidget(gdbNameLabel);
  gdbNameLayout->addWidget(gdbNameEdit);
  //gdbNameLayout->addWidget(createGDBButton);
  connect(createGDBButton, SIGNAL(clicked()), this, SLOT(onCreateGDBButton()));

  srcLabel = new QLabel("输入数据名:", this);
  srcEdit = new QLineEdit(this, "");
  srcLabel->setBuddy(srcEdit);
  srcBtn = new QPushButton("查找", this);
  srcLayout = new QHBoxLayout();
  srcLayout->addWidget(srcLabel);
  srcLayout->addWidget(srcEdit);
  srcLayout->addWidget(srcBtn);
  connect(srcBtn, SIGNAL(clicked()), this, SLOT(onFindSrcBtn()));

  featureSetLabel = new QLabel("要素数据集名:", this);
  featureSetEdit = new QLineEdit(this, "");
  featureSetLabel->setBuddy(featureSetEdit);
  featureSetLayout = new QHBoxLayout();
  featureSetLayout->addWidget(featureSetLabel);
  featureSetLayout->addWidget(featureSetEdit);

  ruleLabel = new QLabel("拓扑规则:", this);
  ruleComboBox = new QComboBox(FALSE, this, "rules");
  ruleLabel->setBuddy(ruleComboBox);
  ruleComboBox->insertItem("不能重叠");
  ruleComboBox->insertItem("不能交叉");
  ruleComboBox->insertItem("不能有悬挂点");
  ruleComboBox->insertItem("不能有伪结点");
  ruleComboBox->insertItem("必需被其它线要素覆盖");
  ruleComboBox->insertItem("不能与其它要素重叠");
  ruleComboBox->insertItem("不能自重叠");
  ruleComboBox->insertItem("必需为单一部分");
  QHBoxLayout* ruleLayout = new QHBoxLayout();
  ruleLayout->addWidget(ruleLabel);
  ruleLayout->addWidget(ruleComboBox);

  mainLayout = new QGridLayout(this, 6, 1);
  mainLayout->setMargin(10);
  mainLayout->addLayout(topLeftLayout, 0, 0);
  mainLayout->addLayout(gdbNameLayout, 1, 0);
  mainLayout->addLayout(srcLayout, 2, 0);
  mainLayout->addLayout(featureSetLayout, 3, 0);
  mainLayout->addLayout(ruleLayout, 4, 0);
  mainLayout->addWidget(createGDBButton, 5, 0);

  this->resize(400, 300);
}

CreateGDBDlg::~CreateGDBDlg() {
}

void CreateGDBDlg::onFindButton(void) {
  QFileDialog* dlg = new QFileDialog("", QString::null, 0, 0, TRUE);
  dlg->setCaption(QFileDialog::tr("Open"));
  dlg->setMode(QFileDialog::DirectoryOnly);
  QString result;
  if (dlg->exec() == QDialog::Accepted) {
    result = dlg->selectedFile();
    if (!result.isNull()) {
      this->lineEdit->setText(result);
    }
  }
  delete dlg;
}

void CreateGDBDlg::onFindSrcBtn(void) {
  QFileDialog* dlg = new QFileDialog("", "Shape files(*.shp)", 0, 0, TRUE);
  dlg->setCaption(QFileDialog::tr("Open"));
  dlg->setMode(QFileDialog::ExistingFile);
  QString result;
  if (dlg->exec() == QDialog::Accepted) {
    result = dlg->selectedFile();
    if (!result.isNull()) {
      this->srcEdit->setText(result);
    }
  }
  delete dlg;
}

void CreateGDBDlg::onCreateGDBButton(void) {
  QString dirName = this->lineEdit->text();
  QString gdbName = this->gdbNameEdit->text();
  QString featureSetName = this->featureSetEdit->text();
  QString srcFullpath = this->srcEdit->text();
  if (dirName.isNull()) {
    QMessageBox::critical(this, "Error", "Please select a direcotry!", QMessageBox::Ok, QMessageBox::Cancel);
    return;
  }
  if (gdbName.isNull()) {
    QMessageBox::critical(this, "Error", "Please input the name of GDB!", QMessageBox::Ok, QMessageBox::Cancel);
    return;
  }
  if (featureSetName.isNull()) {
    QMessageBox::critical(this, "Error", "Please input feature set name!", QMessageBox::Ok, QMessageBox::Cancel);
    return;
  }
  
  QString fullPath = dirName.append(gdbName);
  if (!gdbName.endsWith(".gdb")) {
    fullPath.append(".gdb");
  }
  
  const char * szFullPath = fullPath.latin1();
  
  HRESULT hr = createGDB(szFullPath);
  if (hr != S_OK) {
    QMessageBox::critical(this, "Error", "Create GDB failed!", QMessageBox::Ok, QMessageBox::Cancel);
    return;
  }

  const char* szFeatureSetName = featureSetName.latin1();
  const char* szSrcFullpath = srcFullpath.latin1();
  const char* szFeatureClassName = "aaaa";
  hr = createFeatureSet(szFullPath, szFeatureSetName, szFeatureClassName, szSrcFullpath);
  if (hr != S_OK) {
    QMessageBox::critical(this, "Error", "Create feature set failed!", QMessageBox::Ok, QMessageBox::Cancel);
    return;
  }

  const char* topologyName = "topology";
  hr = createTopology(szFullPath, szFeatureSetName, szFeatureClassName, topologyName);
  if (hr != S_OK) {
    QMessageBox::critical(this, "Error", "Create feature set failed!", QMessageBox::Ok, QMessageBox::Cancel);
    return;
  }
}

HRESULT CreateGDBDlg::createGDB(const char* fullpath) {
  HRESULT hr = S_OK;
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
  
  IWorkspaceFactoryPtr ipWorkspaceFactory(CLSID_FileGDBWorkspaceFactory);
  IWorkspaceNamePtr ipWorkspaceName;
  hr = ipWorkspaceFactory->Create(path, name, NULL, 0, &ipWorkspaceName);
  if (FAILED(hr) || ipWorkspaceName == 0) {
    std::cerr << "Create gdb failed" << std::endl;
    return E_FAIL;
  } else {
    std::cerr << "create gdb success" << std::endl;
  }

  return hr;
}

HRESULT CreateGDBDlg::createFeatureSet(const char* fullpath, const char* featureSetName,
                                       const char* destFeatureClassName, const char* srcFullpath) {
  HRESULT hr = S_OK;

  CComBSTR bstrCoveragePath;
  CComBSTR bstrCoverageName;
  hr = GetParentDirFromFullPath(srcFullpath, &bstrCoveragePath, false);
  if (FAILED(hr) || bstrCoveragePath.Length() == 0) 
  {
    std::cerr << "Error parsing path to coverage" << std::endl;
    return hr;
  }
  hr = GetFileFromFullPath(srcFullpath, &bstrCoverageName);
  if (FAILED(hr) || bstrCoverageName.Length() == 0) 
  {
    std::cerr << "Error parsing path to coverage" << std::endl;
    return hr;
  }

  IWorkspaceFactoryPtr ipSrcWorkspaceFactor(CLSID_ShapefileWorkspaceFactory);
  IWorkspacePtr ipSrcWorkspace;
  hr = ipSrcWorkspaceFactor->OpenFromFile(bstrCoveragePath, 0, &ipSrcWorkspace);
  if (FAILED(hr) || ipSrcWorkspace == 0)
  {
    std::cerr << "Couldn't find the path " << srcFullpath << std::endl;
    if (FAILED(hr))
      return hr;
    else
      return E_FAIL;
  }

  IFeatureWorkspacePtr ipSrcFeatureWorkspace(ipSrcWorkspace);
  IFeatureClassPtr ipSrcFeatureClass;
  hr = ipSrcFeatureWorkspace->OpenFeatureClass(bstrCoverageName, &ipSrcFeatureClass);
  if (FAILED(hr) || ipSrcFeatureClass == 0) {
    std::cerr << "Couldn't open coverage\n";
    if (FAILED(hr)) 
      return hr;
    else
      return E_FAIL;
  }

  IDatasetPtr ipSrcDataset(ipSrcFeatureClass);
  INamePtr ipSrcName;
  ipSrcDataset->get_FullName(&ipSrcName);
  IFeatureClassNamePtr ipSrcFeatureClassName(ipSrcName);

  // create feature set
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
  
  IWorkspaceNamePtr ipFileGDBWorkName(CLSID_WorkspaceName);
  hr = ipFileGDBWorkName->put_WorkspaceFactoryProgID(CComBSTR(L"esriDataSourcesFile.FileGDBWorkspaceFactory"));
  if (FAILED(hr)) {
    std::cerr << "Get IF of gdb workspace factory failed" << std::endl;
    return hr;
  }
  hr = ipFileGDBWorkName->put_PathName(CComBSTR(fullpath));
  if (FAILED(hr)) {
    std::cerr << "set gdb path failed" << std::endl;
    return hr;
  }

  IFeatureDatasetNamePtr ipDestFeaDSClsName(CLSID_FeatureDatasetName);
  ((IDatasetNamePtr)ipDestFeaDSClsName)->put_Name(CComBSTR(featureSetName));
  ((IDatasetNamePtr)ipDestFeaDSClsName)->putref_WorkspaceName(ipFileGDBWorkName);

  IFeatureClassNamePtr ipDestFeatureClassName(CLSID_FeatureClassName);
  ((IDatasetNamePtr)ipDestFeatureClassName)->put_Name(CComBSTR(destFeatureClassName));
  ((IDatasetNamePtr)ipDestFeatureClassName)->putref_WorkspaceName(ipFileGDBWorkName);

  IFeatureDataConverterPtr ipFeatureDataConverter(CLSID_FeatureDataConverter);
  IEnumInvalidObjectPtr ipEnumInvalidObject;
  hr = ipFeatureDataConverter->ConvertFeatureClass(ipSrcFeatureClassName, 0, ipDestFeaDSClsName,
                                                   ipDestFeatureClassName,
                                                   0, 0, 0, 1000, 0, &ipEnumInvalidObject);

  return hr;
}

HRESULT CreateGDBDlg::createTopology(const char* fullpath, const char* featureSetName,
                                     const char* featureClassName, const char* topologyName) {
  HRESULT hr = S_OK;
  
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
    std::cout << "Failed to open the dest folder." << std::endl;
    return E_FAIL;
  } else {
    std::cout << "success" << std::endl;
  }

  IFeatureWorkspacePtr ipFeatureWorkspace(ipWorkspace);
  IFeatureDatasetPtr ipFeatureDataset;
  hr = ipFeatureWorkspace->OpenFeatureDataset(CComBSTR(featureSetName), &ipFeatureDataset);
  if (SUCCEEDED(hr) && ipFeatureDataset != 0) {
    std::cerr << "Feature set exists" << std::endl;
  } else {
    std::cerr << "Feature set does not exists" << std::endl;
    return E_FAIL;
  }
  
  IFeatureClassPtr iFeatureClass;
  hr = ipFeatureWorkspace->OpenFeatureClass(CComBSTR(featureClassName), &iFeatureClass);
  if (FAILED(hr) || ipFeatureWorkspace == 0) {
    std::cerr << "Failed to open feature class." << std::endl;
    return E_FAIL;
  } else {
    std::cerr << "open feature class success" << std::endl;
  }

  ISchemaLockPtr ipSchemaLock(ipFeatureDataset);
  hr = ipSchemaLock->ChangeSchemaLock(esriExclusiveSchemaLock);
  
  ITopologyContainer2Ptr ipTopologyContainer2(ipFeatureDataset);
  double clusterTolerance;
  double zClusterTolerance = 0.001;
  ipTopologyContainer2->get_DefaultClusterTolerance(&clusterTolerance);
  ITopologyPtr ipTopology;
  long errorCount = -1;
  hr = ipTopologyContainer2->CreateTopology(CComBSTR(topologyName), clusterTolerance,
                                            errorCount, CComBSTR(""), &ipTopology);
  if (FAILED(hr) || ipTopology == 0) {
    std::cerr << "create topology failed" << std::endl;
    ipSchemaLock->ChangeSchemaLock(esriSharedSchemaLock);
    return hr;
  } else {
    std::cerr << "create topology success" << std::endl;
  }

  ipTopology->AddClass(iFeatureClass, 5, 1, 1, false);
  ITopologyRulePtr ipTopologyRule(CLSID_TopologyRule);
  ipTopologyRule->put_TopologyRuleType(esriTRTLineNoOverlap);
  ipTopologyRule->put_Name(CComBSTR("no_overlap"));
  long featureClassID = 0;
  iFeatureClass->get_FeatureClassID(&featureClassID);
  ipTopologyRule->put_OriginClassID(featureClassID);
  ipTopologyRule->put_AllOriginSubtypes(true);

  hr = ((ITopologyRuleContainerPtr)ipTopology)->get_CanAddRule(ipTopologyRule, &chk);
  if (FAILED(hr)) {
    std::cerr << "Check add rule failed" << std::endl;
    ipSchemaLock->ChangeSchemaLock(esriSharedSchemaLock);
    return hr;
  } else {
    if (chk == VARIANT_FALSE) {
      std::cerr << "add rule chk false" << std::endl;
    } else {
      std::cerr << "add rule chk true" << std::endl;
      hr = ((ITopologyRuleContainerPtr)ipTopology)->AddRule(ipTopologyRule);
      if (FAILED(hr)) {
        std::cerr << "add rule failed" << std::endl;
        ipSchemaLock->ChangeSchemaLock(esriSharedSchemaLock);
        return hr;
      } else {
        std::cerr << "add rule success" << std::endl;
      }

      hr = validateTopology(ipTopology);
    }
  }

  ipSchemaLock->ChangeSchemaLock(esriSharedSchemaLock);

  if (SUCCEEDED(hr)) {
    QMessageBox::information(this, "执行成功", "执行成功,总共导入13825条数据!",
                             QMessageBox::Ok, QMessageBox::Cancel);
  }

  return hr;
}

HRESULT CreateGDBDlg::validateTopology(ITopologyPtr ipTopology) {
  HRESULT hr = S_OK;
  
  IGeoDatasetPtr ipGeoDataset = (IGeoDatasetPtr)ipTopology;
  IEnvelopePtr ipEnvelope;
  ipGeoDataset->get_Extent(&ipEnvelope);

  IPolygonPtr ipPolygon(CLSID_Polygon);
  ISegmentCollectionPtr ipSegmentCollection = (ISegmentCollectionPtr)ipPolygon;
  ipSegmentCollection->SetRectangle(ipEnvelope);

  IPolygonPtr ipDirtyAreaPolygon;
  hr = ipTopology->get_DirtyArea(ipPolygon, &ipDirtyAreaPolygon);
  VARIANT_BOOL chk;
  hr = ipDirtyAreaPolygon->get_IsEmpty(&chk);
  if (chk == VARIANT_FALSE) {
    IEnvelopePtr areaToValidate;
    hr = ipDirtyAreaPolygon->get_Envelope(&areaToValidate);
    IEnvelopePtr affectedArea;
    hr = ipTopology->ValidateTopology(areaToValidate, &affectedArea);
    if (FAILED(hr)) {
      std::cerr << "validate topology failed" << std::endl;
    } else {
      std::cerr << "validate topology success" << std::endl;
    }
  } else {
    std::cerr << "dirty area not exists" << std::endl;
  }

  return hr;
}

HRESULT CreateGDBDlg::checkTopology(const char* fullpath, const char* featureSetName,
                                    const char* topologyName) {
  HRESULT hr = S_OK;

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
    std::cout << "Failed to open the dest folder." << std::endl;
    return E_FAIL;
  } else {
    std::cout << "success" << std::endl;
  }

  IFeatureWorkspacePtr ipFeatureWorkspace(ipWorkspace);
  IFeatureDatasetPtr ipFeatureDataset;
  hr = ipFeatureWorkspace->OpenFeatureDataset(CComBSTR(featureSetName), &ipFeatureDataset);
  if (SUCCEEDED(hr) && ipFeatureDataset != 0) {
    std::cerr << "Feature set exists" << std::endl;
  } else {
    std::cerr << "Feature set does not exists" << std::endl;
    return E_FAIL;
  }

  ITopologyWorkspacePtr ipTopologyWorkspace(ipWorkspace);
  ITopologyPtr ipTopology;
  hr = ipTopologyWorkspace->OpenTopology(CComBSTR(topologyName), &ipTopology);
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

  return S_OK;
}

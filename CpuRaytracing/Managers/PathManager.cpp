#include "PathManager.h"

#include <CellarWorkbench/Path/PathVisitor.h>
#include <CellarWorkbench/Path/LinearPath.h>
#include <CellarWorkbench/Path/CubicSplinePath.h>
#include <CellarWorkbench/Path/BasisSplinePath.h>
#include <CellarWorkbench/Path/CompositePath.h>

#include <PropRoom3D/Node/StageSet.h>

#include "ui_RaytracerGui.h"
#include "../PathModel.h"
#include "../TheFruitChoreographer.h"

using namespace cellar;

Q_DECLARE_METATYPE(AbstractPath<double>*)
Q_DECLARE_METATYPE(AbstractPath<glm::dvec3>*)

Spin::Spin(double value)
{
    setDecimals(3);
    setMinimum(-100);
    setMaximum( 100);
    setSingleStep(0.01);
    setValue(value);
}

template<typename Data>
class TreeBuilder : public PathVisitor<Data>
{
public:
    TreeBuilder(std::vector<std::function<void(void)>>& segmentModels,
                std::vector<std::string>& segmentParentName,
                const std::function<void(void)>& refreshCallBack,
                Ui::RaytracerGui* ui,
                AbstractPath<Data>* path,
                const std::string& name) :
        _segmentModels(segmentModels),
        _segmentParentName(segmentParentName),
        _refreshCallBack(refreshCallBack),
        _ui(ui),
        _path(path),
        _name(name)
    {
        _path->accept(*this);
        _last->setText(QString((name +" [%1s]").c_str()).arg(path->duration()));
        _root = _last;
    }

    virtual ~TreeBuilder() {}


    virtual void visit(LinearPath<Data>& path) override;

    virtual void visit(CubicSplinePath<Data>& path) override;

    virtual void visit(BasisSplinePath<Data>& path) override;

    virtual void visit(CompositePath<Data> &path) override;



    QStandardItem* getRoot() {return _root;}

protected:
    void setupTable(QTableWidget* table, int rowCount);
    void putValue(QTableWidget* table, int row, Data& value);

    void updatePath();

private:
    std::vector<std::function<void(void)>> &_segmentModels;
    std::vector<std::string>& _segmentParentName;
    std::function<void(void)> _refreshCallBack;
    Ui::RaytracerGui* _ui;
    AbstractPath<Data>* _path;
    std::string _name;

    QStandardItem* _root;
    QStandardItem* _last;
};



template<>
void TreeBuilder<double>::setupTable(QTableWidget* table, int rowCount)
{
    table->clear();
    table->setColumnCount(1);
    table->setRowCount(rowCount);
    table->setHorizontalHeaderLabels({"Value"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

template<>
void TreeBuilder<glm::dvec3>::setupTable(QTableWidget* table, int rowCount)
{
    table->clear();
    table->setColumnCount(3);
    table->setRowCount(rowCount);
    table->setHorizontalHeaderLabels({"X", "Y", "Z"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

template<>
void TreeBuilder<double>::putValue(QTableWidget* table, int row, double& value)
{
    Spin* spin =  new Spin(value);

    table->setCellWidget(row, 0, spin);

    QObject::connect(spin, static_cast<void(Spin::*)(double)>(&Spin::valueChanged),
                     [this, &value](double nv){ value = nv; _refreshCallBack();});
}

template<>
void TreeBuilder<glm::dvec3>::putValue(QTableWidget* table, int row, glm::dvec3& value)
{
    Spin* spinX =  new Spin(value.x);
    Spin* spinY =  new Spin(value.y);
    Spin* spinZ =  new Spin(value.z);

    table->setCellWidget(row, 0, spinX);
    table->setCellWidget(row, 1, spinY);
    table->setCellWidget(row, 2, spinZ);

    QObject::connect(spinX, static_cast<void(Spin::*)(double)>(&Spin::valueChanged),
                     [this, &value](double nv){ value.x = nv; _refreshCallBack();});
    QObject::connect(spinY, static_cast<void(Spin::*)(double)>(&Spin::valueChanged),
                     [this, &value](double nv){ value.y = nv; _refreshCallBack();});
    QObject::connect(spinZ, static_cast<void(Spin::*)(double)>(&Spin::valueChanged),
                     [this, &value](double nv){ value.z = nv; _refreshCallBack();});
}

template<typename Data>
void TreeBuilder<Data>::updatePath()
{

}

template<typename Data>
void TreeBuilder<Data>::visit(LinearPath<Data>& path)
{
    _last = new QStandardItem(QString("Linear [%1s]").arg(path.duration()));
    int callbackIdx = _segmentModels.size();
    _last->setData(QVariant(callbackIdx));

    _segmentParentName.push_back(_name);

    Ui::RaytracerGui* ui = _ui;
    _segmentModels.push_back([this, ui, &path](){
        ui->durationSpin->setValue(path.duration());

        setupTable(ui->segmentTable, 2);
        putValue(ui->segmentTable, 0, path.begin());
        putValue(ui->segmentTable, 1, path.end());
    });
}

template<typename Data>
void TreeBuilder<Data>::visit(CubicSplinePath<Data>& path)
{
    _last = new QStandardItem(QString("Cubic Spline [%1s]").arg(path.duration()));
    int callbackIdx = _segmentModels.size();
    _last->setData(QVariant(callbackIdx));

    _segmentParentName.push_back(_name);

    Ui::RaytracerGui* ui = _ui;
    _segmentModels.push_back([this, ui, &path](){
        ui->durationSpin->setValue(path.duration());

        setupTable(ui->segmentTable, path.ctrlPts().size());

        int row = 0;
        for(Data& pt : path.ctrlPts())
            putValue(ui->segmentTable, row++, pt);
    });
}

template<typename Data>
void TreeBuilder<Data>::visit(BasisSplinePath<Data>& path)
{
    _last = new QStandardItem(QString("Cubic Spline [%1s]").arg(path.duration()));
    int callbackIdx = _segmentModels.size();
    _last->setData(QVariant(callbackIdx));

    _segmentParentName.push_back(_name);

    Ui::RaytracerGui* ui = _ui;
    _segmentModels.push_back([this, ui, &path](){
        ui->durationSpin->setValue(path.duration());

        setupTable(ui->segmentTable, path.ctrlPts().size());

        int row = 0;
        for(Data& pt : path.ctrlPts())
            putValue(ui->segmentTable, row++, pt);
    });
}


template<typename Data>
void TreeBuilder<Data>::visit(CompositePath<Data> &path)
{
    QStandardItem* item = new QStandardItem(QString("Composite [%1s]").arg(path.duration()));
    item->setSelectable(false);

    for(const auto& sub : path.paths())
    {
        sub->accept(*this);
        item->appendRow(_last);
    }

    _last  = item;
}


PathManager::PathManager(Ui::RaytracerGui* ui) :
    _ui(ui),
    _selectedPathName(),
    _isSelectedPathVisible(false)
{
    _doubleTreeBuilders.reserve(10);
    _dvec3TreeBuilders.reserve(10);

    _pathTreeModel = new QStandardItemModel();
    _ui->pathsTree->setModel(_pathTreeModel);

    connect(_ui->pathsTree->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this, &PathManager::selectionChanged);

    connect(_ui->displayDebugCheck, &QCheckBox::toggled,
            this, &PathManager::displayDebugToggled);
}

PathManager::~PathManager()
{
    delete _pathTreeModel;
}

void PathManager::setStageSet(
    const std::shared_ptr<prop3::StageSet>& stageSet)
{
    _stageSet = stageSet;
}

void PathManager::setChoreographer(
    const std::shared_ptr<TheFruitChoreographer>& choreographer)
{
    _choreographer = choreographer;

    clearPaths();
    PathModel& pathModel = *choreographer->pathModel();
    appendPath(pathModel.cameraTo,   "Camera To");
    appendPath(pathModel.cameraEye,  "Camera Eye");
    appendPath(pathModel.cameraFoV,  "Camera FoV");
    appendPath(pathModel.theFruit,   "The Fruit");
    appendPath(pathModel.clouds,     "Clouds");
    appendPath(pathModel.dayTime,    "Day Time");
}

void PathManager::clearPaths()
{
    _pathTreeModel->clear();
    _ui->segmentTable->clear();
    _segmentModels.clear();
    _segmentParentName.clear();

    _doubleTreeBuilders.clear();
    _dvec3TreeBuilders.clear();
}

void PathManager::appendPath(const std::shared_ptr<AbstractPath<double> > &path, const std::string& name)
{
    _doubleTreeBuilders.emplace_back(
        _segmentModels,
        _segmentParentName,
        std::bind(&PathManager::controlPointMoved, this),
        _ui, path.get(), name);

    _pathTreeModel->appendRow(_doubleTreeBuilders.back().getRoot());
}

void PathManager::appendPath(const std::shared_ptr<AbstractPath<glm::dvec3>>& path, const std::string& name)
{
    _dvec3TreeBuilders.emplace_back(
        _segmentModels,
        _segmentParentName,
        std::bind(&PathManager::controlPointMoved, this),
        _ui, path.get(), name);

    _pathTreeModel->appendRow(_dvec3TreeBuilders.back().getRoot());
}

void PathManager::selectionChanged(const QItemSelection& selected,
                                   const QItemSelection& deselected)
{
    for(const QItemSelectionRange& range : selected)
    {
        const QAbstractItemModel* model = range.model();
        for(const QModelIndex& id :range.indexes())
        {
            QVariant var = model->itemData(id)[Qt::UserRole+1];
            _segmentModels[var.toInt()]();

            _selectedPathName = _segmentParentName[var.toInt()];
            _isSelectedPathVisible = _choreographer->pathModel()
                    ->isDebugLineVisible(_selectedPathName);
            _ui->displayDebugCheck->setChecked(
                _isSelectedPathVisible);
        }
    }
}

void PathManager::displayDebugToggled(bool display)
{
    if(!_selectedPathName.empty())
    {
        _isSelectedPathVisible = display;
        _choreographer->pathModel()->setDebugLineVisibility(
                    _selectedPathName, display);
    }
}

void PathManager::controlPointMoved()
{
    if(_isSelectedPathVisible)
    {
        _choreographer->pathModel()->refreshDebugLines();
    }
}

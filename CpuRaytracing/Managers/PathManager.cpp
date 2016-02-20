#include "PathManager.h"

#include <CellarWorkbench/Path/PathVisitor.h>
#include <CellarWorkbench/Path/LinearPath.h>
#include <CellarWorkbench/Path/CubicSplinePath.h>
#include <CellarWorkbench/Path/BasisSplinePath.h>
#include <CellarWorkbench/Path/CompositePath.h>

#include "ui_RaytracerGui.h"
#include "../TheFruitChoreographer.h"

using namespace cellar;

Q_DECLARE_METATYPE(AbstractPath<double>*)
Q_DECLARE_METATYPE(AbstractPath<glm::dvec3>*)

class Spin : public QDoubleSpinBox
{
public:
    Spin(double value)
    {
        setValue(value);
    }
};

template<typename Data>
class TreeBuilder : public PathVisitor<Data>
{
public:
    TreeBuilder(std::vector<std::function<void(void)>>& segmentModels,
                Ui::RaytracerGui* ui,
                AbstractPath<Data>* path,
                const std::string& name) :
        _segmentModels(segmentModels),
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

    virtual void visit(CompositePath<Data>& path) override;



    QStandardItem* getRoot() {return _root;}

private:
    std::vector<std::function<void(void)>> &_segmentModels;
    Ui::RaytracerGui* _ui;
    AbstractPath<Data>* _path;
    std::string _name;

    QStandardItem* _root;
    QStandardItem* _last;
};


template<>
void TreeBuilder<double>::visit(LinearPath<double>& path)
{
    _last = new QStandardItem(QString("Linear [%1s]").arg(path.duration()));
    int callbackIdx = _segmentModels.size();
    _last->setData(QVariant(callbackIdx));

    Ui::RaytracerGui* ui = _ui;
    _segmentModels.push_back([ui,&path](){
        ui->durationSpin->setValue(path.duration());

        ui->segmentTable->clear();
        ui->segmentTable->setRowCount(2);
        ui->segmentTable->setColumnCount(2);
        ui->segmentTable->setItem(0, 0, new QTableWidgetItem("Begin"));
        ui->segmentTable->setCellWidget(0, 1, new Spin(path.begin()));
        ui->segmentTable->setItem(1, 0, new QTableWidgetItem("End"));
        ui->segmentTable->setCellWidget(1, 1, new Spin(path.end()));
    });
}

template<>
void TreeBuilder<glm::dvec3>::visit(LinearPath<glm::dvec3>& path)
{
    _last = new QStandardItem(QString("Linear [%1s]").arg(path.duration()));
    int callbackIdx = _segmentModels.size();
    _last->setData(QVariant(callbackIdx));

    Ui::RaytracerGui* ui = _ui;
    _segmentModels.push_back([ui,&path](){
        ui->durationSpin->setValue(path.duration());

        ui->segmentTable->clear();
        ui->segmentTable->setRowCount(2);
        ui->segmentTable->setColumnCount(4);
        ui->segmentTable->setItem(0, 0, new QTableWidgetItem("Begin"));
        ui->segmentTable->setCellWidget(0, 1, new Spin(path.begin().x));
        ui->segmentTable->setCellWidget(0, 2, new Spin(path.begin().y));
        ui->segmentTable->setCellWidget(0, 3, new Spin(path.begin().z));
        ui->segmentTable->setItem(1, 0, new QTableWidgetItem("End"));
        ui->segmentTable->setCellWidget(1, 1, new Spin(path.end().x));
        ui->segmentTable->setCellWidget(1, 2, new Spin(path.end().y));
        ui->segmentTable->setCellWidget(1, 3, new Spin(path.end().z));
    });
}

template<>
void TreeBuilder<double>::visit(CubicSplinePath<double>& path)
{
    _last = new QStandardItem(QString("Cubic Spline [%1s]").arg(path.duration()));
    int callbackIdx = _segmentModels.size();
    _last->setData(QVariant(callbackIdx));

    Ui::RaytracerGui* ui = _ui;
    _segmentModels.push_back([ui,&path](){
        ui->durationSpin->setValue(path.duration());

        ui->segmentTable->clear();
        ui->segmentTable->setRowCount(path.ctrlPts().size());
        ui->segmentTable->setColumnCount(1);

        int row = 0;
        for(double pt : path.ctrlPts())
        {
            ui->segmentTable->setCellWidget(row, 0, new Spin(pt));
            ++row;
        }
    });
}

template<>
void TreeBuilder<glm::dvec3>::visit(CubicSplinePath<glm::dvec3>& path)
{
    _last = new QStandardItem(QString("Cubic Spline [%1s]").arg(path.duration()));
    int callbackIdx = _segmentModels.size();
    _last->setData(QVariant(callbackIdx));

    Ui::RaytracerGui* ui = _ui;
    _segmentModels.push_back([ui,&path](){
        ui->durationSpin->setValue(path.duration());

        ui->segmentTable->clear();
        ui->segmentTable->setRowCount(path.ctrlPts().size());
        ui->segmentTable->setColumnCount(3);

        int row = 0;
        for(const glm::dvec3& pt : path.ctrlPts())
        {
            ui->segmentTable->setCellWidget(row, 0, new Spin(pt.x));
            ui->segmentTable->setCellWidget(row, 1, new Spin(pt.y));
            ui->segmentTable->setCellWidget(row, 2, new Spin(pt.z));
            ++row;
        }
    });
}

template<>
void TreeBuilder<double>::visit(BasisSplinePath<double>& path)
{
    _last = new QStandardItem(QString("Basis Spline [%1s]").arg(path.duration()));
    int callbackIdx = _segmentModels.size();
    _last->setData(QVariant(callbackIdx));

    Ui::RaytracerGui* ui = _ui;
    _segmentModels.push_back([ui,&path](){
        ui->durationSpin->setValue(path.duration());

        ui->segmentTable->clear();
        ui->segmentTable->setRowCount(path.ctrlPts().size());
        ui->segmentTable->setColumnCount(1);

        int row = 0;
        for(double pt : path.ctrlPts())
        {
            ui->segmentTable->setCellWidget(row, 0, new Spin(pt));
            ++row;
        }
    });
}

template<>
void TreeBuilder<glm::dvec3>::visit(BasisSplinePath<glm::dvec3>& path)
{
    _last = new QStandardItem(QString("Basis Spline [%1s]").arg(path.duration()));
    int callbackIdx = _segmentModels.size();
    _last->setData(QVariant(callbackIdx));

    Ui::RaytracerGui* ui = _ui;
    _segmentModels.push_back([ui,&path](){
        ui->durationSpin->setValue(path.duration());

        ui->segmentTable->clear();
        ui->segmentTable->setRowCount(path.ctrlPts().size());
        ui->segmentTable->setColumnCount(3);

        int row = 0;
        for(const glm::dvec3& pt : path.ctrlPts())
        {
            ui->segmentTable->setCellWidget(row, 0, new Spin(pt.x));
            ui->segmentTable->setCellWidget(row, 1, new Spin(pt.y));
            ui->segmentTable->setCellWidget(row, 2, new Spin(pt.z));
            ++row;
        }
    });
}

template<typename Data>
void TreeBuilder<Data>::visit(CompositePath<Data>& path)
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
    _ui(ui)
{
    _pathModel = new QStandardItemModel();
    _ui->pathsTree->setModel(_pathModel);

    connect(_ui->pathsTree->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this, &PathManager::selectionChanged);
}

PathManager::~PathManager()
{
    delete _pathModel;
}

void PathManager::setChoreographer(
    const std::shared_ptr<TheFruitChoreographer>& choreographer)
{
    _choreographer = choreographer;
    _choreographer->displayPaths(*this);
}

void PathManager::clearPaths()
{
    _pathModel->clear();
    _ui->segmentTable->clear();
    _segmentModels.clear();
}

void PathManager::appendPath(const std::shared_ptr<AbstractPath<glm::dvec3>>& path, const std::string& name)
{
    TreeBuilder<glm::dvec3> builder(_segmentModels, _ui, path.get(), name);

    _pathModel->appendRow(builder.getRoot());
}

void PathManager::appendPath(const std::shared_ptr<AbstractPath<double> > &path, const std::string& name)
{
    TreeBuilder<double> builder(_segmentModels, _ui, path.get(), name);

    _pathModel->appendRow(builder.getRoot());
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
        }
    }
}

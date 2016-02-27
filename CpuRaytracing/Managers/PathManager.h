#ifndef PATHMANAGER_H
#define PATHMANAGER_H

#include <memory>
#include <functional>

#include <QWidget>
#include <QDoubleSpinBox>
#include <QStandardItemModel>
#include <QItemSelectionModel>

#include <CellarWorkbench/Path/AbstractPath.h>

namespace Ui
{
    class RaytracerGui;
}

namespace prop3
{
    class StageSet;
}

class TheFruitChoreographer;

template<typename Data>
class TreeBuilder;


class PathManager : public QObject
{
    Q_OBJECT

public:
    PathManager(Ui::RaytracerGui* ui);
    virtual ~PathManager();

    virtual void setStageSet(
        const std::shared_ptr<prop3::StageSet>& stageSet);
    virtual void setChoreographer(
        const std::shared_ptr<TheFruitChoreographer>& choreographer);

    virtual void clearPaths();
    virtual void appendPath(const std::shared_ptr<cellar::AbstractPath<glm::dvec3>>& path, const std::string& name);
    virtual void appendPath(const std::shared_ptr<cellar::AbstractPath<double>>& path, const std::string& name);


private slots:
    virtual void selectionChanged(const QItemSelection& selected,
                                 const QItemSelection& deselected);
    virtual void displayDebugToggled(bool display);

    virtual void controlPointMoved();

private:
    Ui::RaytracerGui* _ui;
    QStandardItemModel* _pathTreeModel;
    std::shared_ptr<prop3::StageSet> _stageSet;
    std::shared_ptr<TheFruitChoreographer> _choreographer;

    std::vector<TreeBuilder<double>> _doubleTreeBuilders;
    std::vector<TreeBuilder<glm::dvec3>> _dvec3TreeBuilders;

    std::vector<std::function<void(void)>> _segmentModels;
    std::vector<std::string> _segmentParentName;
    std::string _selectedPathName;
    bool _isSelectedPathVisible;
};

class Spin : public QDoubleSpinBox
{
    Q_OBJECT

public:
    Spin(double value);
};

#endif // PATHMANAGER_H

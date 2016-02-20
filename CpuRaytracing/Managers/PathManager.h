#ifndef PATHMANAGER_H
#define PATHMANAGER_H

#include <memory>
#include <functional>

#include <QWidget>
#include <QStandardItemModel>
#include <QItemSelectionModel>

#include <CellarWorkbench/Path/AbstractPath.h>

namespace Ui
{
    class RaytracerGui;
}

class TheFruitChoreographer;


class PathManager : public QObject
{
    Q_OBJECT

public:
    PathManager(Ui::RaytracerGui* ui);
    virtual ~PathManager();

    virtual void setChoreographer(
        const std::shared_ptr<TheFruitChoreographer>& choreographer);

    virtual void clearPaths();
    virtual void appendPath(const std::shared_ptr<cellar::AbstractPath<glm::dvec3>>& path, const std::string& name);
    virtual void appendPath(const std::shared_ptr<cellar::AbstractPath<double>>& path, const std::string& name);


private slots:
    virtual void selectionChanged(const QItemSelection& selected,
                                 const QItemSelection& deselected);


private:
    Ui::RaytracerGui* _ui;
    QStandardItemModel* _pathModel;
    std::vector<std::function<void(void)>> _segmentModels;
    std::shared_ptr<TheFruitChoreographer> _choreographer;
};

#endif // PATHMANAGER_H

#ifndef CPURAYTRACING_QPOSTPRODMANAGER_H
#define CPURAYTRACING_QPOSTPRODMANAGER_H

#include <memory>

#include <GLM/glm.hpp>

#include <QWidget>
#include <QTableWidget>

#include <PropRoom3D/Team/ArtDirector/GlPostProdUnit.h>


namespace Ui
{
    class QPostProdManager;
}


class QPostProdManager :
        public QWidget,
        public prop3::PostProdManager
{
    Q_OBJECT

public:
    QPostProdManager();
    virtual ~QPostProdManager();

    virtual void setup() override;
    virtual void setPostProdUnit(
        std::shared_ptr<prop3::GlPostProdUnit> unitBackend) override;

protected:

private slots:
    virtual void activateLowPassChecked(int state);
    virtual void lowpassSizeChanged(int sizeIndex);
    virtual void lowpassVarianceChanged(double variance);
    virtual void useAdaptativeFilteringChecked(int state);
    virtual void adaptativeFilteringFactorChanged(int factor);
    virtual void temperatureChanged(int kelvin);
    virtual void temperatureDefaultClicked();
    virtual void contrastChanged(int contrast);
    virtual void luminosityChanged(int luminosity);
    virtual void saveOutputImage();

private:
    static float computeLuminosityFactor(int luminosity);
    static float computeContrastFactor(int contrast);
    static float computeAdaptativeFactor(int factor);
    static void updateLowpassKernelTable(QTableWidget* widget, const float* kernel);


private:
    bool _isSetup;
    std::shared_ptr<Ui::QPostProdManager> _ui;
    std::shared_ptr<prop3::GlPostProdUnit> _unitBackend;
};

#endif // CPURAYTRACING_QPOSTPRODMANAGER_H

#ifndef DEMOCHOOSERDIALOG_H
#define DEMOCHOOSERDIALOG_H

#include <set>
#include <memory>

#include <QDialog>

namespace Ui {
class DemoChooserDialog;
}

namespace scaena
{
    class AbstractPlay;
}

class DemoChooserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DemoChooserDialog(const std::vector<std::shared_ptr<scaena::AbstractPlay>>& demos,QWidget *parent = 0);
    virtual ~DemoChooserDialog();

    bool userQuit();
    std::shared_ptr<scaena::AbstractPlay> selectedDemo();

private slots:
    void Quit();
    void Launch();

private:
    Ui::DemoChooserDialog *_ui;
    bool _userQuit;
    std::shared_ptr<scaena::AbstractPlay> _selectedDemo;
};

#endif // DEMOCHOOSERDIALOG_H

#ifndef DEMOCHOOSERDIALOG_H
#define DEMOCHOOSERDIALOG_H

#include <set>
#include <memory>
#include <functional>

#include <QDialog>

namespace Ui
{
    class DemoChooserDialog;
}


namespace scaena
{
    class Play;

    typedef std::function<std::shared_ptr<Play>()> DemoFunc;
    typedef std::pair<std::string, DemoFunc> Demo;
    typedef std::vector<Demo> DemoVec;

    class DemoChooserDialog : public QDialog
    {
        Q_OBJECT

    public:
        explicit DemoChooserDialog(const DemoVec& demos , QWidget *parent = 0);
        virtual ~DemoChooserDialog();

        bool userQuit();
        Demo selectedDemo();

    private slots:
        void Quit();
        void Launch();

    private:
        Ui::DemoChooserDialog *_ui;
        Demo _selectedDemo;
        bool _userQuit;
    };
}

#endif // DEMOCHOOSERDIALOG_H

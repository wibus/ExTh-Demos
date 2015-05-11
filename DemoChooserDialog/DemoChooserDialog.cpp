#include "DemoChooserDialog.h"
#include "ui_DemoChooserDialog.h"

#include <iostream>
using namespace std;

#include <QString>
#include <QPushButton>

#include <Scaena/Play/Play.h>
using namespace scaena;


DemoChooserDialog::DemoChooserDialog(
        const DemoVec& demos,
        QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::DemoChooserDialog),
    _userQuit(true)
{
    _ui->setupUi(this);
    connect(_ui->buttonBox, SIGNAL(accepted()), this, SLOT(Launch()));
    connect(_ui->buttonBox, SIGNAL(rejected()), this, SLOT(Quit()));

    bool isFirst = true;
    for(const auto& demo : demos)
    {
        QPushButton* playButton = new QPushButton(demo.first.c_str());
        _ui->groupBox->layout()->addWidget(playButton);
        playButton->setCheckable(true);
        playButton->setChecked(isFirst);

        connect(playButton, &QPushButton::clicked, [&]() {
            _selectedDemo = demo;
            cout << _selectedDemo.first << endl;
        });

        if(isFirst)
        {
            playButton->click();
            playButton->setFocus();
        }

        isFirst = false;
    }
}

DemoChooserDialog::~DemoChooserDialog()
{
    delete _ui;
}

bool DemoChooserDialog::userQuit()
{
    return _userQuit;
}

Demo DemoChooserDialog::selectedDemo()
{
    return _selectedDemo;
}

void DemoChooserDialog::Quit()
{
    _userQuit = true;
}

void DemoChooserDialog::Launch()
{
    _userQuit = false;
}

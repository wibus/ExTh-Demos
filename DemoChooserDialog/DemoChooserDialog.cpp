#include "DemoChooserDialog.h"
#include "ui_DemoChooserDialog.h"

#include <iostream>

#include <QString>
#include <QPushButton>

#include <Play/AbstractPlay.h>

using namespace std;
using namespace scaena;

DemoChooserDialog::DemoChooserDialog(const set<shared_ptr<AbstractPlay>>& demos, QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::DemoChooserDialog),
    _userQuit(true),
    _selectedDemo()
{
    _ui->setupUi(this);
    connect(_ui->buttonBox, SIGNAL(accepted()), this, SLOT(Launch()));
    connect(_ui->buttonBox, SIGNAL(rejected()), this, SLOT(Quit()));

    // TODO : Remove this horrible hard coded selection
    _selectedDemo = *demos.begin();

    bool isFirst = true;
    for(auto demo : demos)
    {
        string playId = demo->id();
        QPushButton* playButton = new QPushButton(playId.c_str());
        _ui->groupBox->layout()->addWidget(playButton);
        playButton->setCheckable(true);
        playButton->setChecked(isFirst);
        isFirst = false;

        connect(playButton, &QPushButton::clicked, [=]() {
            _selectedDemo = demo;
            cout << _selectedDemo->id() << endl;
        });
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

std::shared_ptr<scaena::AbstractPlay> DemoChooserDialog::selectedDemo()
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

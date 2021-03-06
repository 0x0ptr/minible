#include "emulator_ui.h"

#include <QFormLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSlider>
#include <QMenu>
#include <QFileDialog>
#include <QCheckBox>
#include <QMutex>
#include <QTimer>

#include "emulator.h"
#include "emu_smartcard.h"

EmuWindow::EmuWindow()
{
    auto layout = new QFormLayout(this);
    auto smartcard = createSmartcardUi();
    layout->addRow("Smartcard", smartcard);

    auto battery = createBatteryUi();
    layout->addRow("Battery", battery);

    auto charger = createChargerUi();
    layout->addRow("USB", charger);

    auto accel = createAccelerometerUi();
    layout->addRow("Orientation", accel);
    
    auto fail = createFailuresUi();
    layout->addRow("Failures", fail);
}

QWidget *EmuWindow::createSmartcardUi() 
{
    auto row_smartcard = new QWidget();
    auto smartcard_layout = new QHBoxLayout(row_smartcard);

    auto btn_insert = new QPushButton("Insert");
    auto btn_remove = new QPushButton("Remove");
    if(emu_is_smartcard_inserted())
        btn_insert->setEnabled(false);
    else
        btn_remove->setEnabled(false);

    auto btn_insert_menu = new QMenu();
    auto act_invalid = btn_insert_menu->addAction("Invalid");
    QObject::connect(act_invalid, &QAction::triggered, this, [=]() {
        if(emu_insert_new_smartcard(QString(), EMU_SMARTCARD_INVALID)) {
            btn_remove->setEnabled(true);
            btn_insert->setEnabled(false);
        }
    });

    auto act_broken = btn_insert_menu->addAction("Broken");
    QObject::connect(act_broken, &QAction::triggered, this, [=]() {
        if(emu_insert_new_smartcard(QString(), EMU_SMARTCARD_BROKEN)) {
            btn_remove->setEnabled(true);
            btn_insert->setEnabled(false);
        }
    });

    auto act_new = btn_insert_menu->addAction("New (blank)");
    QObject::connect(act_new, &QAction::triggered, this, [=]() {
        QFileDialog dialog(this, "Create smartcard file", QDir::currentPath(), "Smartcard Image Files (*.smc)");
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setDefaultSuffix(".smc");

        if (dialog.exec() == QDialog::Accepted) {
            auto fileName = dialog.selectedFiles().value(0);
            if(emu_insert_new_smartcard(fileName, EMU_SMARTCARD_REGULAR)) {
                btn_remove->setEnabled(true);
                btn_insert->setEnabled(false);
            }
        }
    });

    auto act_existing = btn_insert_menu->addAction("Existing");
    QObject::connect(act_existing, &QAction::triggered, this, [=]() {
        auto fileName = QFileDialog::getOpenFileName(this, "Select smartcard file", "", "Smartcard Image Files (*.smc)");
        if(fileName.isEmpty())
            return;

        if(emu_insert_smartcard(fileName)) {
            btn_remove->setEnabled(true);
            btn_insert->setEnabled(false);
        }
    });

    btn_insert->setMenu(btn_insert_menu);

    QObject::connect(btn_remove, &QPushButton::clicked, this, [=]() {
        emu_remove_smartcard();
        btn_remove->setEnabled(false);
        btn_insert->setEnabled(true);
    });

    smartcard_layout->addWidget(btn_insert);
    smartcard_layout->addWidget(btn_remove);

    return row_smartcard;
}

static QMutex ui_mutex;
static int battery_level = 75;
static bool usb_powered = true;
static bool usb_charging = false;

int emu_get_battery_level() 
{
    ui_mutex.lock();
    int ret = battery_level;
    ui_mutex.unlock();
    return ret;
}

BOOL emu_get_usb_charging() 
{
    ui_mutex.lock();
    bool ret = usb_powered;
    ui_mutex.unlock();
    return ret;
}

void emu_charger_enable(BOOL en)
{
    ui_mutex.lock();
    usb_charging = en;
    ui_mutex.unlock();
}

QWidget *EmuWindow::createBatteryUi() 
{
    auto slider = new QSlider(Qt::Horizontal);
    slider->setMaximum(100);
    slider->setValue(battery_level);

    auto charger = new QTimer(this);
    QObject::connect(charger, &QTimer::timeout, this, [slider]() {
        ui_mutex.lock();
        bool charge = usb_charging && usb_powered;
        ui_mutex.unlock();

        if(charge)
            slider->setValue(slider->value() + 5);
    });
    charger->start(500);

    QObject::connect(slider, &QSlider::valueChanged, this, [](int value) {
        ui_mutex.lock();
        battery_level = value;
        ui_mutex.unlock();
    });
    return slider;
}

QWidget *EmuWindow::createChargerUi()
{
    auto checkbox = new QCheckBox("powered");
    checkbox->setCheckState(usb_powered ? Qt::Checked : Qt::Unchecked);
    QObject::connect(checkbox, &QCheckBox::stateChanged, this, [=](int state) {
        ui_mutex.lock();
        usb_powered = state == Qt::Checked;
        ui_mutex.unlock();
    });
    return checkbox;
}

static bool left_handed = false;
BOOL emu_get_lefthanded()
{
    ui_mutex.lock();
    bool ret = left_handed;
    ui_mutex.unlock();
    return ret;
}

QWidget *EmuWindow::createAccelerometerUi()
{
    auto checkbox = new QCheckBox("left-handed");
    checkbox->setCheckState(left_handed ? Qt::Checked : Qt::Unchecked);
    QObject::connect(checkbox, &QCheckBox::stateChanged, this, [=](int state) {
        ui_mutex.lock();
        left_handed = state == Qt::Checked;
        ui_mutex.unlock();
    });
    return checkbox;

}

static int failure_flags = 0;

int emu_get_failure_flags()
{
    ui_mutex.lock();
    int ret = failure_flags;
    ui_mutex.unlock();
    return ret;
}

QWidget *EmuWindow::createFailuresUi()
{
    auto col_failures = new QWidget(this);
    auto layout = new QBoxLayout(QBoxLayout::TopToBottom, col_failures);
    layout->setContentsMargins(0,0,0,0);

    const char *labels[] = {
        "smartcard insecure",
        "dbflash full",
        "eeprom full",
        NULL
    };

    for(int i=0;labels[i] != NULL;i++) {
        auto cb = new QCheckBox(labels[i]);
        int flag = 1 << i;

        QObject::connect(cb, &QCheckBox::stateChanged, this, [flag](int state) {
            ui_mutex.lock();
            if(state == Qt::Checked)
                failure_flags |= flag;
            else
               failure_flags &= ~flag;
            ui_mutex.unlock();
        });
        layout->addWidget(cb);
    }

    return col_failures;
}

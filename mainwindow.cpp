#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QFile"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_open_triggered()
{
    QFileDialog open(this);
    QStringList filters;
    filters << "Текстовые файлы (*.txt)" << "Все файлы (*)";
    open.setNameFilters(filters);
    if(open.exec())
    {
        QString file_path = open.selectedFiles().at(0);
    }
}

void MainWindow::on_save_triggered()
{
    QFileDialog save(this);
    save.setNameFilter("Текстовые файлы (*.txt)");
    save.setDefaultSuffix("txt");
    save.setAcceptMode(QFileDialog::AcceptSave);
    if(save.exec())
    {
        QString file_path = save.selectedFiles().at(0);
    }
}

void MainWindow::on_processButton_clicked()
{
    QVector<float> value;
    QVector<int> count;
    QStringList lst = ui->source->toPlainText().split("/n");
    foreach (QString i, lst)
    {
        bool *ok;
        ok = false;
        int value = i.toFloat(ok);
        if(ok)
        {
            int z = 0;
        }
    }
}

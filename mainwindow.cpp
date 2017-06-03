#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QFile"
#include "QMessageBox"
#include "cmath"

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_open_triggered()
{
    QFileDialog open(this);                     //Настраиваем окно для выбора открываемого файла
    QStringList filters;
    filters << "Текстовые файлы (*.txt)" << "Все файлы (*)";
    open.setNameFilters(filters);
    if(open.exec())                             //Показыаем окно
    {
        QFile file(open.selectedFiles().at(0));
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            while(!file.atEnd())
            {
                ui->source->insertPlainText(file.readLine());
            }
            file.close();
        }
        else
        {
            QMessageBox mbox;
            mbox.setText("Ошибка при открытии файла.");
            mbox.setIcon(QMessageBox::Critical);
            mbox.exec();
            return;
        }
    }
}

void MainWindow::on_save_triggered()
{
    QFileDialog save(this);                     //Настраиваем окно для выбора файла сохранения
    save.setNameFilter("Текстовые файлы (*.txt)");
    save.setDefaultSuffix("txt");
    save.setAcceptMode(QFileDialog::AcceptSave);
    if(save.exec())                             //Показываем окно и сохраняем файл
    {
        QFile file(save.selectedFiles().at(0));
        if(file.open(QIODevice::WriteOnly | QIODevice::Truncate |  QIODevice::Text))
        {
            file.write(QString("Исходные данные:\n\n").toUtf8());
            file.write(ui->source->toPlainText().toUtf8());
            file.write(QString("\n\nОбработанные данные:\n\n").toUtf8());
            file.write(ui->result->toPlainText().toUtf8());
            file.close();
        }
        else
        {
            QMessageBox mbox;
            mbox.setText("Ошибка при сохранении файла.");
            mbox.setIcon(QMessageBox::Critical);
            mbox.exec();
            return;
        }
    }
}

void MainWindow::on_processButton_clicked()
{
    bool containComma = false;
    QString text = ui->source->toPlainText();
    if(text.at(text.length() - 1) == "\n")
    {
        text.remove(text.length() - 1, 1);
    }
    if(text.contains(","))                      //Если в числах в качестве рзделителя дробой части используется запятая
    {
        text = text.replace(",",".");           //Заменяем на точки
        containComma = true;                    //И активируем флаг. чтобы в выходных данных соверщить обратную замену
    }
    QStringList lst = text.split("\n");         //Разбиваем единую строку на массив
    QList<float> ta;
    float taF;
    QString deleted = "\n\nОтброшены: ";
    switch (ui->comboBox->currentIndex())
    {
    case 0:
        ta << 1.5 << 1.7 << 1.84 << 1.94 << 2.02 << 2.09 << 2.15 << 2.2 << 2.24 << 2.28 << 2.32 << 2.35 << 2.38 << 2.41 << 2.43 << 2.46 << 2.48 << 2.5 << 2.52 << 2.54 << 2.56 << 2.57 << 2.59;
        taF = 0.3053*log(lst.count()) + 1.6513;
        break;
    case 1:
        ta << 1.74 << 1.94 << 2.08 << 2.18 << 2.27 << 2.33 << 2.39 << 2.44 << 2.48 << 2.52 << 2.56 << 2.59 << 2.62 << 2.64 << 2.67 << 2.69 << 2.71 << 2.73 << 2.75 << 2.77 << 2.78 << 2.8 << 2.82;
        taF = 0.2849*log(lst.count()) + 1.9517;
        break;
    case 2:
        ta << 2.22 << 2.43 << 2.57 << 2.68 << 2.76 << 2.83 << 2.88 << 2.93 << 2.97 << 3.01 << 3.04 << 3.07 << 3.1 << 3.12 << 3.15 << 3.17 << 3.19 << 3.21 << 3.22 << 3.24 << 3.26 << 3.27 << 3.28;
        taF = 0.2648 * log(lst.count()) + 2.4839;
        break;
    default:
        break;
    }
    double sum = 0;
    float min = QString(lst.at(0)).toFloat();
    float max = min;
    int imax, imin;
    for(int i = 0; i < lst.count(); i++)
    {
        bool ok = false;                        //Флаг для проверки корректности преобразования
        float value = lst.at(i).toFloat(&ok);           //Пытаемся выполгнить преоразование
        if(ok)                                  //Если успешно
        {
            if(value > max)
            {
                max = value;
                imax = i;
            }
            else if (value < min)
            {
                min = value;
                imin = i;
            }
            sum =+ value;
        }
        else                                    //Если встретилось не число. показыаем информацонное сообщение
        {
            QMessageBox mbox;
            mbox.setText("Входные данные должны содержать только числа.");
            mbox.setIcon(QMessageBox::Warning);
            mbox.exec();
            return;                             //И завершаем работу функции
        }
    }
    float avg = sum/lst.count();
    sum = 0;
    foreach (QString i, lst)
    {
        sum =+ pow(i.toFloat() - avg, 2);
    }
    double quadDev = sqrt(sum/lst.count());
    float taMin = abs(min - avg)/quadDev;
    float taMax = abs(max - avg)/quadDev;
    if(lst.count() <= 25)
    {
    if(taMax > ta.at(lst.count() - 3))
    {
        lst.removeAt(imax);
        deleted.append(QString::number(max) + " ");
    }
    if(taMin > ta.at((lst.count() - 3)))
    {
        lst.removeAt(imin);
        deleted.append(QString::number(min) + " ");
    }
    }

    else
    {
        if(taMax > taF)
        {
            lst.removeAt(imax);
            deleted.append(QString::number(max) + " ");
        }
        if(taMin > taF)
        {
            lst.removeAt(imin);
            deleted.append(QString::number(min) + " ");
        }
    }

    text = lst.join("\n");
    text.append(deleted);
    if(containComma)
    {
        text = text.replace(".",",");
    }
    ui->result->setPlainText(text);
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QFile"
#include "QMessageBox"
#include "qmath.h"

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_open_triggered()            //Функция для загрузки данных из файлы
{
    QFileDialog open(this);                     //Настраиваем окно для выбора открываемого файла
    QStringList filters;                        
    filters << "Текстовые файлы (*.txt)" << "Все файлы (*)";
    open.setNameFilters(filters);
    if(open.exec())                             //Показыаем окно
    {
        QFile file(open.selectedFiles().at(0));
        ui->source->clear();
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            while(!file.atEnd())                //Считывание файла
            {
                ui->source->insertPlainText(file.readLine());
            }
            file.close();                       //Закрытией файлы
        }
        else                                    //Вывод окна об ошибке
        {
            QMessageBox mbox;
            mbox.setText("Ошибка при открытии файла.");
            mbox.setIcon(QMessageBox::Critical);
            mbox.exec();
            return;
        }
    }
}

void MainWindow::on_save_triggered()            //Функция для сохранения результатов в файл
{
    QFileDialog save(this);                     //Настраиваем окно для выбора файла сохранения
    save.setNameFilter("Текстовые файлы (*.txt)");
    save.setDefaultSuffix("txt");
    save.setAcceptMode(QFileDialog::AcceptSave);
    if(save.exec())                             //Показываем окно
    {
        QFile file(save.selectedFiles().at(0));
        if(file.open(QIODevice::WriteOnly | QIODevice::Truncate |  QIODevice::Text))//Открываем и очищаем файл
        {
            file.write(QString("Исходные данные:\n\n").toUtf8());
            file.write(ui->source->toPlainText().toUtf8());         //Запись исходных данных в файл
            file.write(QString("\n\nОбработанные данные:\n\n").toUtf8());
            file.write(ui->result->toPlainText().toUtf8());         //Запись результата в файл
            file.close();
        }
        else                                    //Вывод окна об ошибке
        {
            QMessageBox mbox;
            mbox.setText("Ошибка при сохранении файла.");
            mbox.setIcon(QMessageBox::Critical);
            mbox.exec();
            return;
        }
    }
}

void MainWindow::on_processButton_clicked()     //Функция обработки результатов вычислений
{
    bool containComma = false;                  //Флаг для поверки разделителя дробной части
    QString text = ui->source->toPlainText();
    if(text.at(text.length() - 1) == "\n")      //Если последний символ - символ перевода строки
    {
        text.remove(text.length() - 1, 1);      //Удаляем его (необходимо для правильной обработки)
    }
    if(text.contains(","))                      //Если в числах в качестве рзделителя дробной части используется запятая
    {
        text = text.replace(",",".");           //Заменяем на точки
        containComma = true;                    //И активируем флаг. чтобы в выходных данных соверщить обратную замену
    }
    QStringList lst = text.split("\n");         //Разбиваем единую строку на массив
    QList<float> ta;                            //Список значений ta
    float taF;                                  //Рассчетное значение ta при n > 25
    QString deleted = "\n\nОтброшены: ";
    QString calc = "\n\nПромежуточные вычисления: ";
    switch (ui->comboBox->currentIndex())       //В зависимости от выборанного ктитерия значимости выполняется определенная ветка case
    {
    case 0:     //a = 0.1
        ta << 1.5 << 1.7 << 1.84 << 1.94 << 2.02 << 2.09 << 2.15 << 2.2 << 2.24 << 2.28 << 2.32 << 2.35 << 2.38 << 2.41 << 2.43 << 2.46 << 2.48 << 2.5 << 2.52 << 2.54 << 2.56 << 2.57 << 2.59;
        taF = 0.3053*log(lst.count()) + 1.6513;
        break;
    case 1:     //a = 0.05
        ta << 1.74 << 1.94 << 2.08 << 2.18 << 2.27 << 2.33 << 2.39 << 2.44 << 2.48 << 2.52 << 2.56 << 2.59 << 2.62 << 2.64 << 2.67 << 2.69 << 2.71 << 2.73 << 2.75 << 2.77 << 2.78 << 2.8 << 2.82;
        taF = 0.2849*log(lst.count()) + 1.9517;
        break;
    case 2:     //a= 0.01
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
    for(int i = 0; i < lst.count(); i++)        //Цикл получения чисел из строки
    {
        bool ok = false;                        //Флаг для проверки корректности преобразования
        float value = lst.at(i).toFloat(&ok);           //Пытаемся выполгнить преоразование
        if(ok)                                  //Если успешно
        {
            if(value > max)                    //Проверка на новое максимальное значение
            {
                max = value;
                imax = i;
            }
            else if (value < min)               //проверка на новое минимальное значение
            {
                min = value;
                imin = i;
            }
            sum += value;                       //Суммируем
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
    float avg = sum/lst.count();                //Рассчитываем среднее значение
    calc.append("\nСумма: " + QString::number(sum));
    calc.append("\nСреднее: " + QString::number(avg));
    calc.append("\nМинимум: " + QString::number(min));
    calc.append("\nМаксимум: " + QString::number(max));
    sum = 0;
    foreach (QString i, lst)
    {
        sum += qPow(i.toFloat() - avg, 2);
    }
    double quadDev = sqrt(sum/lst.count());     //Рассчет среднеквадраичного отклонения отклонения
    calc.append("\nСреднеквадратичное отклонение: " + QString::number(quadDev));
    double taMin = qAbs(min - avg)/quadDev;     //Рассчет t для минимума
    double taMax = qAbs(max - avg)/quadDev;     //Рассчет t для максимума
    calc.append("\nt минимума: " + QString::number(taMin));
    calc.append("\nt максимума: " + QString::number(taMax));
    if(lst.count() <= 25)       //Далее вся ветка if-else является проверкой на соответсвие критерию
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
        calc.append("\nta: " + QString::number(ta.at(lst.count() - 3)));
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
        calc.append("\nta: " + QString::number(taF));
    }

    text = lst.join("\n");
    text.append(deleted);
    text.append(calc);
    if(containComma)        //Если использовалась запятая в качестве разделителя
    {
        text = text.replace(".",",");       //Выполняем обратное преобразование
    }
    ui->result->setPlainText(text);         //Выводим данные
}

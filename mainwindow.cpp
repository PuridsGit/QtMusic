#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "configdialog.h"
#include "about.h"

#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery.h>
#include <QtSql/QSqlError>

#include <QtDebug>
#include <QFileInfo>

#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMediaService>
#include <QMediaMetaData>

#include <QWidget>
#include <QtWidgets>

#include <QFileDialog>

//Datenbank vorbereiten
bool prepareDB()
{
    QSqlQuery query;
    if(!query.exec("CREATE TABLE Music ("
                   "FID INTEGER PRIMARY KEY,"
                   "URL TEXT,"
                   "TITLE TEXT,"
                   "ARTIST TEXT,"
                   "ALBUM TEXT"
                   ")"))
        return false;

    if(!query.exec("CREATE TABLE Dir ("
                   "Dir TEXT,"
                   "id INTEGER PRIMARY KEY"
                   ")"))
        return false;

    return true;
}

//Datenbank oeffnen
bool openDB()
{
    //Variablen
    bool createnew = false;

    //Datenbankfile erstellen und pruefen
    QFileInfo dbfile(QDir::homePath() + "/Musicplayer.sqlite");
    if(!dbfile.exists() || dbfile.size() <= 10) createnew = true;

    //Db driver
    QSqlDatabase tmp = QSqlDatabase::addDatabase("QSQLITE");
    tmp.setDatabaseName(QDir::homePath() + "/Musicplayer.sqlite");
    if(!tmp.open())
    {
        QMessageBox::critical(0, "Error, Datenbank!", "Db prepare! ("+tmp.lastError().text()+QString::number(tmp.lastError().number())+")");
    }
    if(createnew)
    {
        if(!prepareDB())
        {
            QMessageBox::critical(0, "Error, Datenbank", "Eb prepare! ("+tmp.lastError().text()+")");
            return false;
        }
    }
    return true;
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(!openDB())
    {
        this->close();
    }



    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);
    player->setPlaylist(playlist);


    QString query =  "select distinct Artist from Music";

    qDebug() << aQString;

    QStringList artistList = preparePlaylistURL(query);

    for(int i = 0; i < artistList.size(); i++)
    {
        ui->listWidget_artist->addItem(artistList.at(i));
        qDebug() << artistList.at(i);
    }
    ui->listWidget_artist->repaint();


    //connect Menubar
    connect(ui->actionOptionen, SIGNAL(triggered()), this, SLOT(on_ActionOptionenTriggered()));

    //connects player
    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::on_positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::on_durationChanged);

    //conect Qlistwidget clicks
    connect(ui->listWidget_artist, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(on_listWidget_artist_itemClicked(QListWidgetItem*)));
    connect(ui->listWidget_Album, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(on_listWidget_Album_itemClicked(QListWidgetItem*)));
    connect(ui->listWidget_playlist, SIGNAL(itemClicked(QListWidgetItem*)),this ,SLOT(on_listWidget_playlist_itemClicked(QListWidgetItem*)));

    //connect Qlistwidget double clicks
    connect(ui->listWidget_artist, QListWidget::doubleClicked, this, MainWindow::on_listWidget_artist_itemDoubleClicked);
    connect(ui->listWidget_Album, QListWidget::doubleClicked, this, MainWindow::on_listWidget_album_itemDoubleClicked);
    connect(ui->listWidget_title, QListWidget::doubleClicked, this, MainWindow::on_listWidget_title_itemDoubleClicked);
    connect(ui->listWidget_playlist, QListWidget::doubleClicked, this, MainWindow::on_listWidget_playlist_itemDoubleClicked);

}



MainWindow::~MainWindow()
{
    delete ui;
}


//gathers data from table and compiles it into a list
QStringList MainWindow::querytoStringlist(QString query)
{
    qDebug() << query;
    QSqlQuery q;
    q.prepare(query);
    if(!q.exec())
    {
        qDebug() << q.lastError().text();
    }

    QStringList list;

    while(q.next())
    {
        list.append(q.value(0).toString());
    }

    return list;
}

//adds the data to the playlist
void MainWindow::addtoPlaylist(QStringList titles, QStringList URLs)
{
    URLslength = URLs.length();
    for(int i = 0; i < URLslength; i++)
    {
        URLs.at(i).replace("/","\\\\");
        playlist->addMedia(QUrl::fromLocalFile(URLs.at(i)));
    }

    ui->listWidget_playlist->addItems(titles);
}


//routine fuer push button stop
void MainWindow::on_pushButton_stop_clicked()
{
    player->stop();
    ui->pushButton_pause->setEnabled(false);
    ui->pushButton_play->setEnabled(true);
}

//play file
void MainWindow::on_pushButton_play_clicked()
{
    player->play();
    ui->pushButton_play->setEnabled(false);
    ui->pushButton_pause->setEnabled(true);
}


//spulen via progressbar erlmoeglichen
void MainWindow::on_horizontalSlider_progress_sliderMoved(int position)
{
    player->setPosition(position);
}

//volume bar
void MainWindow::on_horizontalSlider_volume_sliderMoved(int position)
{
    player->setVolume(position);
}

//progressbar richtige werte geben
void MainWindow::on_positionChanged(qint64 position)
{
    ui->horizontalSlider_progress->setValue(position);
}

void MainWindow::on_durationChanged(qint64 position)
{
    ui->horizontalSlider_progress->setMaximum(position);
}

//push button pause
void MainWindow::on_pushButton_pause_clicked()
{
    player->pause();
    ui->pushButton_play->setEnabled(true);
    ui->pushButton_pause->setEnabled(false);
}

void MainWindow::on_ActionOptionenTriggered()
{
    configdialog cfg;
    cfg.exec();
}



void MainWindow::on_listWidget_artist_itemClicked(QListWidgetItem *item)
{
    QString itemString = "select distinct ALBUM from Music where ARTIST = '" + item->text() + "'";
    qDebug() << itemString;


    QStringList alist = preparePlaylistURL(itemString);

    ui->listWidget_Album->clear();

    ui->listWidget_Album->addItems(alist);
}

void MainWindow::on_listWidget_Album_itemClicked(QListWidgetItem *item)
{
    QString itemString = "select TITLE from Music where ALBUM ='" + item->text() + "'";
    qDebug() << itemString;

    QStringList alist = preparePlaylistURL(itemString);

    ui->listWidget_title->clear();

    ui->listWidget_title->addItems(alist);

}

//zur playlist hinzufuegen
//double clicked slots

//double clicked listwidget title
void MainWindow::on_listWidget_title_itemDoubleClicked(QListWidgetItem *item)
{
    QString query ="select URL from Music where TITLE = '" +item->text()+ "'";
    QStringList urls = querytoStringlist(query);
    QStringList titles = titles.append(item->text());

    addtoPlaylist(titles, urls);

}

//double clicked listwidget artist
void MainWindow::on_listWidget_artist_itemDoubleClicked(QListWidgetItem *item)
{
    QString query ="select URL from Music where ARTIST = '" +item->text()+ "'";
    QStringList urls = querytoStringlist(query);
    query = "select TITLE from Music where ARTIST = '" +item->text()+ "'";
    QStringList titles = querytoStringlist(query);

    addtoPlaylist(titles, urls);
}

//double cliked listwidget album
void MainWindow::on_listWidget_album_itemDoubleClicked(QListWidgetItem *item)
{
    QString query = "select URL from Music where ALBUM = '" +item->text()+ "'";
    QStringList urls = querytoStringlist(query);
    query = "select TITLE from Music where ALBUM = '" +item->text()+ "'";
    QStringList titles = querytoStringlist(query);

    addtoPlaylist(titles, urls);
}

//double clicked listwidget playlist
void MainWindow::on_listWidget_playlist_itemDoubleClicked(QListWidgetItem *item)
{
    int index = ui->listWidget_playlist->currentRow();
    playlist->setCurrentIndex(index);
    player->play();
}

//clear playlist
void MainWindow::on_pushButton_clearplaylist_clicked()
{
    ui->listWidget_playlist->clear();
    playlist->clear();
}




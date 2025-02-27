#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>
#include<QFileDialog>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  firstLoad=true;
  ui->setupUi(this);
  setUpHighlighter();
  //init status bar
  ui->outputText->parentWindow=this;
  ui->statusBar->showMessage(tr("Ready"));
  //--------init toolbar------------
  //ui->statusBar->setStyleSheet("QStatusBar{background:rgb(50,50,50);}");
  ui->mainToolBar->setMovable(false);
  ui->mainToolBar->setStyleSheet("QToolButton:hover {background-color:darkgray} QToolBar {background: rgb(82,82,82);border: none;}");
  //--------------------------------

  runIcon.addPixmap(QPixmap(":./image/Run.png"));
  stopIcon.addPixmap(QPixmap(":./image/stop.png"));

  //---------窗口背景颜色-------------
  QPalette windowPalette=this->palette();
  windowPalette.setColor(QPalette::Active,QPalette::Window,QColor(34,40,49));
  windowPalette.setColor(QPalette::Inactive,QPalette::Window,QColor(34,40,49));
  this->setPalette(windowPalette);
  //--------------------------------
  initFileData();
  connect(ui->actionNewFile,SIGNAL(triggered(bool)),this,SLOT(newFile()));
  connect(ui->actionOpen,SIGNAL(triggered(bool)),this,SLOT(openFile()));
  connect(ui->actionSave_File,SIGNAL(triggered(bool)),this,SLOT(saveFile()));
  connect(ui->actionUndo,SIGNAL(triggered(bool)),this,SLOT(undo()));
  connect(ui->actionRedo,SIGNAL(triggered(bool)),this,SLOT(redo()));
  connect(ui->editor,SIGNAL(textChanged()),this,SLOT(changeSaveState()));
  connect(ui->actionRun,SIGNAL(triggered(bool)),this,SLOT(run()));
  connect(&process,SIGNAL(finished(int)),this,SLOT(runFinished(int)));
  connect(&process,SIGNAL(readyReadStandardOutput()),this,SLOT(updateOutput()));
  connect(&process,SIGNAL(readyReadStandardError()),this,SLOT(updateError()));
  connect(ui->actionAbout,SIGNAL(triggered(bool)),this,SLOT(about()));
  fileSaved=true;
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::setUpHighlighter(){
  QFont font;
  font.setFamily("Courier");
  font.setFixedPitch(true);
  //font.setPointSize(20);
  ui->editor->setFont(font);
  ui->editor->setTabStopWidth(fontMetrics().width(QLatin1Char('9'))*4);
  highlighter=new Highlighter(ui->editor->document());
}

void MainWindow::resizeEvent(QResizeEvent *event){
  QMainWindow::resizeEvent(event);
  ui->editor->setGeometry(10,0,width()-20,height()-ui->statusBar->height()-ui->mainToolBar->height()-80-15);
  ui->outputText->setGeometry(10,ui->editor->height()+10,this->width()-20,80);
}

void MainWindow::initFileData(){
  fileName=tr("Untitled.cpp");
  filePath=tr("~/Desktop/Untitled.cpp");
  fileSaved=true;
  isRunning=false;
}

void MainWindow::undo(){
  ui->editor->undo();
}
void MainWindow::redo(){
  ui->editor->redo();
}
void MainWindow::saveFile(){
  QString savePath=QFileDialog::getSaveFileName(this,tr("choose your path and file name"),fileName,tr("Cpp File(*.cpp *.c *.h)"));
  if(!savePath.isEmpty()){
      QFile out(savePath);
      out.open(QIODevice::WriteOnly|QIODevice::Text);
      QTextStream str(&out);
      str<<ui->editor->toPlainText();
      out.close();
      fileSaved=true;
      QRegularExpression re(tr("(?<=\\/)\\w+\\.cpp|(?<=\\/)\\w+\\.c|(?<=\\/)\\w+\\.h"));
      fileName=re.match(savePath).captured();
      filePath=savePath;
      this->setWindowTitle(tr("Editor - ")+fileName);
    }
}
void MainWindow::newFile(){
  MainWindow *newWindow=new MainWindow();
  QRect newPos=this->geometry();
  newWindow->setGeometry(newPos.x()+10,newPos.y()+10,newPos.width(),newPos.height());
  newWindow->show();
}
void MainWindow::openFile(){
  if(!fileSaved){
      if(QMessageBox::Save==QMessageBox::question(this,tr("Files not Saved"),tr("Save or Not？"),QMessageBox::Save,QMessageBox::Cancel))
        saveFile();
    }
  QString openPath=QFileDialog::getOpenFileName(this,tr("choose file to open"),filePath,tr("Cpp File(*.cpp *.c *.h)"));
  if(!openPath.isEmpty()){
      QFile in(openPath);
      in.open(QIODevice::ReadOnly|QIODevice::Text);
      QTextStream str(&in);
      ui->editor->setPlainText(str.readAll());
      QRegularExpression re(tr("(?<=\\/)\\w+\\.cpp|(?<=\\/)\\w+\\.c|(?<=\\/)\\w+\\.h"));
      fileName=re.match(openPath).captured();
      this->setWindowTitle(tr("Editor - ")+fileName);
      filePath=openPath;
      fileSaved=true;
    }
}
void MainWindow::run(){
  if(isRunning){
      process.terminate();
      ui->actionRun->setIcon(runIcon);
      return;
    }
  if(!fileSaved){
      if(QMessageBox::Save==QMessageBox::question(this,tr("Files not Saved"),tr("Files can be run after Saving ,Save or Not？"),QMessageBox::Save,QMessageBox::Cancel))
        saveFile();
    }
  if(fileSaved){
    //if(process!=nullptr)delete process;
    isRunning=true;
    ui->statusBar->showMessage(tr("running..."));
    ui->outputText->clear();
    output.clear();
    error.clear();
    QString buildPath;
    QRegularExpression re(tr(".*(?=\\.cpp)|.*(?=\\.c)|.*(?=\\.h)"));
    buildPath=re.match(filePath).captured();
    //qDebug()<<buildPath;
    //此处调用电脑本身mingw进行编译
    //process.start("bash", QStringList() << "-c" << QString(tr("g++ ")+filePath+tr(" -o ")+buildPath+tr(";")+buildPath));

    //QString strPath = "E:/demo/Compiler-all/Test-Editor/Test/Compiler.exe";
    //process.start(strPath);
    buildPath+=".cpp";
    QString program =  "./Compiler.exe";
    QStringList arguments;
    arguments<<buildPath;//传递到exe的参数
    process.start(program,arguments);
    process.waitForStarted();
    //run();
    ui->outputText->setFocus();
    ui->actionRun->setIcon(stopIcon);
    }
}
void MainWindow::runFinished(int code){
  ui->actionRun->setIcon(runIcon);
  isRunning=false;
  qDebug()<<tr("exit code=")<<code;
  ui->statusBar->showMessage(tr("Ready"));
}
void MainWindow::updateOutput(){
  output=QString::fromLocal8Bit(process.readAllStandardOutput());
  //ui->outputText->setPlainText(output+tr("\n")+error);
  ui->outputText->setPlainText(ui->outputText->toPlainText()+output);//+tr("\n"));
}
void MainWindow::updateError(){
  error=QString::fromLocal8Bit(process.readAllStandardError());
  //ui->outputText->setPlainText(output+tr("\n")+error);
  ui->outputText->setPlainText(ui->outputText->toPlainText()+error);//+tr("\n"));
  process.terminate();
  isRunning=false;
}
void MainWindow::inputData(QString data){
  if(isRunning)process.write(data.toLocal8Bit());
}
void MainWindow::closeEvent(QCloseEvent *event){
  if(!fileSaved){
      if(QMessageBox::Save==QMessageBox::question(this,tr("Files not Saved？"),tr("Save or Not?"),QMessageBox::Save,QMessageBox::Cancel))
        saveFile();
      fileSaved=true;
    }
}
void MainWindow::about(){

  QMessageBox::information(this,tr("About"),"Team 12\n1651390 Liu Siyuan\n1753642 Zhao Deze\n1751796 Li Chunbo",QMessageBox::Ok);
}

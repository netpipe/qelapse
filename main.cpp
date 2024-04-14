#include <QApplication>
#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QTime>
#include <QListWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QFileDialog>
#include <QTextStream>
#include <QCloseEvent>

   #define etimes 10000 // elapsed ammount max

class Stopwatch : public QMainWindow {
    Q_OBJECT
public:
    Stopwatch(QWidget *parent = nullptr) : QMainWindow(parent) {
        timerLabel = new QLabel("00:00:00", this);
        timerLabel->setAlignment(Qt::AlignCenter);
        timerLabel->setGeometry(100, 50, 200, 50);

        realTimeLabel = new QLabel(QTime::currentTime().toString("hh:mm:ss AP"), this);
        realTimeLabel->setAlignment(Qt::AlignCenter);
        realTimeLabel->setGeometry(100, 100, 200, 20);

        startButton = new QPushButton("Start", this);
        startButton->setGeometry(100, 150, 80, 30);
        connect(startButton, &QPushButton::clicked, this, &Stopwatch::startTimer);

        stopButton = new QPushButton("Stop", this);
        stopButton->setGeometry(200, 150, 80, 30);
        connect(stopButton, &QPushButton::clicked, this, &Stopwatch::stopTimer);

        resetButton = new QPushButton("Reset", this);
        resetButton->setGeometry(150, 200, 80, 30);
        connect(resetButton, &QPushButton::clicked, this, &Stopwatch::resetTimer);

        exportButton = new QPushButton("Export", this);
        exportButton->setGeometry(150, 250, 80, 30);
        connect(exportButton, &QPushButton::clicked, this, &Stopwatch::exportResults);

        elapsedTimesList = new QListWidget(this);
        elapsedTimesList->setGeometry(100, 300, 200, 150);

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &Stopwatch::updateTime);

        elapsedTimer = new QTime();
        elapsedTimesCount = 0;

        createTrayIcon();
        startRealTimeTimer();
    }

    ~Stopwatch() {
        delete trayIcon;
    }

private slots:
    void startTimer() {
        startTime = QTime::currentTime();
        elapsedTimer->start();
        timer->start(100);
    }

    void stopTimer() {
        int elapsed = elapsedTimer->elapsed();
        if (elapsedTimesCount < etimes) {
            elapsedTimes[elapsedTimesCount++] = elapsed;
            displayElapsedTimes();
        }
        timerLabel->setText(QTime(0, 0).addMSecs(elapsed).toString("hh:mm:ss"));
    }

    void resetTimer() {
        timer->stop();
        elapsedTimer->restart();
        elapsedTimesCount = 0;
        timerLabel->setText("00:00:00");
        elapsedTimesList->clear();
    }

    void updateTime() {
        int elapsed = elapsedTimer->elapsed();
        QTime displayTime(0, 0, 0);
        displayTime = displayTime.addMSecs(elapsed);
        timerLabel->setText(displayTime.toString("hh:mm:ss"));
    }

    void displayElapsedTimes() {
        QTime currentTime = QTime::currentTime();
        QString timeString = currentTime.toString("hh:mm:ss AP");
        elapsedTimesList->addItem(timeString+" "+ timerLabel->text());
    }

    void createTrayIcon() {
        trayIconMenu = new QMenu(this);
        QAction *showAction = new QAction(tr("&Show"), this);
        connect(showAction, &QAction::triggered, this, &Stopwatch::show);
        trayIconMenu->addAction(showAction);

        QAction *quitAction = new QAction(tr("&Quit"), this);
        connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
        trayIconMenu->addAction(quitAction);

        trayIcon = new QSystemTrayIcon(this);
        trayIcon->setIcon(QIcon(":/images/icon.png"));
        trayIcon->setToolTip("Stopwatch");
        trayIcon->setContextMenu(trayIconMenu);
        trayIcon->show();
    }

    void startRealTimeTimer() {
        QTimer *realTimeTimer = new QTimer(this);
        connect(realTimeTimer, &QTimer::timeout, this, [this]() {
            realTimeLabel->setText(QTime::currentTime().toString("hh:mm:ss AP"));
        });
        realTimeTimer->start(1000);
    }

    void exportResults() {
        QString fileName = QFileDialog::getSaveFileName(this, "Export Results", QDir::homePath(), "Text Files (*.txt)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                for (int i = 0; i < elapsedTimesList->count(); ++i) {
                    out << elapsedTimesList->item(i)->text() << endl;
                }
                file.close();
            }
        }
    }

protected:
    void closeEvent(QCloseEvent *event) override {
        hide();
        event->ignore();
    }

private:
    QLabel *timerLabel;
    QLabel *realTimeLabel;
    QPushButton *startButton;
    QPushButton *stopButton;
    QPushButton *resetButton;
    QPushButton *exportButton;
    QListWidget *elapsedTimesList;
    QTimer *timer;
    QTime *elapsedTimer;
    QTime startTime;
    qint64 elapsedTimes[etimes];
    int elapsedTimesCount;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/icon.png"));

    Stopwatch stopwatch;
    stopwatch.setWindowTitle("Stopwatch");
    stopwatch.setGeometry(100, 100, 400, 450);
    stopwatch.show();

    return app.exec();
}

#include "main.moc"

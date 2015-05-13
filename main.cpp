/**
 * \file main.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2012-2015 Thorsten Roth <elthoro@gmx.de>
 *
 * This file is part of iQPuzzle.
 *
 * iQPuzzle is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * iQPuzzle is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with iQPuzzle.  If not, see <http://www.gnu.org/licenses/>.
 *
 * \section DESCRIPTION
 * Main function, start application, loading translation.
 */

/** \mainpage
 * \section Introduction
 * iQPuzzle is a diverting I.Q. challenging pentomino puzzle.<br />
 * Launchpad: https://launchpad.net/iqpuzzle
 */

#include <QApplication>
#include <QTextStream>

#include "./CIQPuzzle.h"

QFile logfile;
QTextStream out(&logfile);

QString getLanguage(const QString &sSharePath);
void setupLogger(const QString &sDebugFilePath,
                 const QString &sAppName,
                 const QString &sVersion);

#if QT_VERSION >= 0x050000
    void LoggingHandler(QtMsgType type,
                        const QMessageLogContext &context,
                        const QString &sMsg);
#else
    void LoggingHandler(QtMsgType type, const char *sMsg);
#endif

int main(int argc, char *argv[]) {
    Q_INIT_RESOURCE(iqpuzzle_resources);

    QApplication app(argc, argv);
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(APP_VERSION);

    if (app.arguments().contains("-v")
            || app.arguments().contains("--version")) {
        qDebug() << app.arguments()[0] << "\t"
                 << app.applicationVersion() << "\n";
        exit(0);
    }

    // Default share data path (Windows and debugging)
    QString sSharePath = app.applicationDirPath();
    // Standard installation path (Linux)
    QDir tmpDir(app.applicationDirPath() + "/../share/games/"
                + app.applicationName().toLower());
    if (!app.arguments().contains("--debug") && tmpDir.exists()) {
        sSharePath = app.applicationDirPath() + "/../share/games/"
                       + app.applicationName().toLower();
    }

    const QString sLang(getLanguage(sSharePath));
    QTranslator qtTranslator;
    QTranslator AppTranslator;

    // Setup gui translation (Qt)
        if (!qtTranslator.load("qt_" + sLang,
                               QLibraryInfo::location(
                                   QLibraryInfo::TranslationsPath))) {
            // If it fails, search in application directory
            if (!qtTranslator.load("qt_" + sLang, sSharePath + "/lang")) {
                qWarning() << "Could not load Qt translations:" << "qt_" + sLang;
            }
        }
        app.installTranslator(&qtTranslator);

        // Setup gui translation (app)
        if (!AppTranslator.load(app.applicationName().toLower() + "_" + sLang,
                                sSharePath + "/lang")) {
            qWarning() << "Could not load application translation:"
                       << qAppName() + "_" + sLang;
        }
        app.installTranslator(&AppTranslator);

#if QT_VERSION >= 0x050000
    QStringList sListPaths = QStandardPaths::standardLocations(
                QStandardPaths::DataLocation);
    if (sListPaths.isEmpty()) {
        qCritical() << "Error while getting data standard path.";
        sListPaths << app.applicationDirPath();
    }
    const QDir userDataDir(sListPaths[0].toLower());
#else
    const QDir userDataDir(
                QDesktopServices::storageLocation(
                    QDesktopServices::DataLocation).toLower());
#endif
    // Create folder including possible parent directories (mkPATH)
    if (!userDataDir.exists()) {
        userDataDir.mkpath(userDataDir.absolutePath());
    }

    const QString sDebugFile("Debug.log");
    setupLogger(userDataDir.absolutePath() + "/" + sDebugFile,
                app.applicationName(), app.applicationVersion());

    CIQPuzzle myIQPuzzle(userDataDir, sSharePath);
    myIQPuzzle.show();
    int nRet = app.exec();

    logfile.close();
    return nRet;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void setupLogger(const QString &sDebugFilePath,
                 const QString &sAppName,
                 const QString &sVersion) {
    // Remove old debug file
    if (QFile(sDebugFilePath).exists()) {
        QFile(sDebugFilePath).remove();
    }

    // Create new file
    logfile.setFileName(sDebugFilePath);
    if (!logfile.open(QIODevice::WriteOnly)) {
        qWarning() << "Couldn't create logging file: " << sDebugFilePath;
    } else {
#if QT_VERSION >= 0x050000
        qInstallMessageHandler(LoggingHandler);
#else
        qInstallMsgHandler(LoggingHandler);
#endif
    }

    qDebug() << sAppName << sVersion;
    qDebug() << "Compiled with Qt" << QT_VERSION_STR;
    qDebug() << "Qt runtime" <<  qVersion();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#if QT_VERSION >= 0x050000
void LoggingHandler(QtMsgType type,
                    const QMessageLogContext &context,
                    const QString &sMsg) {
    QString sMsg2(sMsg);
    QString sContext = sMsg + " (" +
            QString(context.file) + ":" +
            QString::number(context.line) + ", " +
            QString(context.function) + ")";
#else
void LoggingHandler(QtMsgType type, const char *sMsg) {
    QString sMsg2(sMsg);
    QString sContext(sMsg);
#endif
    QString sTime(QTime::currentTime().toString());

    switch (type) {
    case QtDebugMsg:
        out << sTime << " Debug: " << sMsg2 << "\n";
        out.flush();
        break;
    case QtWarningMsg:
        out << sTime << " Warning: " << sContext << "\n";
        out.flush();
        break;
    case QtCriticalMsg:
        out << sTime << " Critical: " << sContext << "\n";
        out.flush();
        break;
    case QtFatalMsg:
        out << sTime << " Fatal: " << sContext << "\n";
        out.flush();
        logfile.close();
        abort();
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString getLanguage(const QString &sSharePath) {
#if defined _WIN32
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       qAppName().toLower(), qAppName().toLower());
#else
    QSettings settings(QSettings::NativeFormat, QSettings::UserScope,
                       qApp->applicationName().toLower(),
                       qApp->applicationName().toLower());
#endif

    QString sLang = settings.value("GuiLanguage", "auto").toString();
    if ("auto" == sLang) {
        #ifdef Q_OS_UNIX
        QByteArray lang = qgetenv("LANG");
        if (!lang.isEmpty()) {
            return QLocale(lang).name();
        }
        #endif
        return QLocale::system().name();
    } else if (!QFile(sSharePath + "/lang/"
                      + qAppName() + "_" + sLang + ".qm").exists()) {
        settings.setValue("GuiLanguage", "en");
        return "en";
    }
    return sLang;
}